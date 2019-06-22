#include "immediate_renderer.hpp"
#include <math.h>

namespace AgpuCommon
{
#include "shaders/compiled.inc"

agpu_vertex_attrib_description ImmediateVertexAttributes[] = {
    {0, 0, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, 1, offsetof(ImmediateRendererVertex, position), 0},
    {0, 1, AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(ImmediateRendererVertex, color), 0},
    {0, 2, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, 1, offsetof(ImmediateRendererVertex, normal), 0},
    {0, 3, AGPU_TEXTURE_FORMAT_R32G32_FLOAT, 1, offsetof(ImmediateRendererVertex, texcoord), 0},
};



inline size_t nextPowerOfTwo(size_t v)
{
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

static agpu::shader_ref loadSpirVShader(const agpu::device_ref &device, agpu_shader_type shaderType, const uint8_t *data, size_t dataSize)
{
    auto shader = agpu::shader_ref(device->createShader(shaderType));
    shader->setShaderSource(AGPU_SHADER_LANGUAGE_SPIR_V, (agpu_string)data, dataSize);
    auto error = shader->compileShader("");
    if(error)
        return agpu::shader_ref();

    return shader;
}

bool StateTrackerCache::ensureImmediateRendererObjectsExists()
{
    std::unique_lock<std::mutex> l(immediateRendererObjectsMutex);

    if(immediateRendererObjectsInitialized)
        return true;

    // Create the shader signature.
    {
        auto builder = agpu::shader_signature_builder_ref(device->createShaderSignatureBuilder());
        if(!builder) return false;

        builder->beginBindingBank(1);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLER, 2);

        builder->beginBindingBank(100);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER, 1);

        builder->beginBindingBank(1000);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1);

        for(size_t i = 0; i < sizeof(ImmediatePushConstants)/4; ++i)
            builder->addBindingConstant();

        immediateShaderSignature = agpu::shader_signature_ref(builder->build());
        if(!immediateShaderSignature) return false;
    }

    // Create the immediate shader library.
    {
        immediateShaderLibrary.reset(new ImmediateShaderLibrary());
        if(!immediateShaderLibrary) return false;

        immediateShaderLibrary->flatColorVertex = loadSpirVShader(device, AGPU_VERTEX_SHADER, flatColor_vert_spv, flatColor_vert_spv_len);
        immediateShaderLibrary->flatColorFragment = loadSpirVShader(device, AGPU_FRAGMENT_SHADER, flatColor_frag_spv, flatColor_frag_spv_len);

        immediateShaderLibrary->smoothColorVertex = loadSpirVShader(device, AGPU_VERTEX_SHADER, smoothColor_vert_spv, smoothColor_vert_spv_len);
        immediateShaderLibrary->smoothColorFragment = loadSpirVShader(device, AGPU_FRAGMENT_SHADER, smoothColor_frag_spv, smoothColor_frag_spv_len);
    }

    // Create the immediate vertex layout.
    {
        immediateVertexLayout = agpu::vertex_layout_ref(device->createVertexLayout());
        if(!immediateVertexLayout) return false;

        agpu_size strides = sizeof(ImmediateRendererVertex);
        auto error = immediateVertexLayout->addVertexAttributeBindings(1, &strides,
            sizeof(ImmediateVertexAttributes) / sizeof(ImmediateVertexAttributes[0]),
            ImmediateVertexAttributes);
        if(error) return false;
    }

    immediateRendererObjectsInitialized = true;
    return true;
}

ImmediateRenderer::ImmediateRenderer(const agpu::state_tracker_cache_ref &stateTrackerCache)
    : stateTrackerCache(stateTrackerCache)
{
    vertexBufferCapacity = 0;
    matrixBufferCapacity = 0;
    activeMatrixStack = nullptr;

    auto impl = stateTrackerCache.as<StateTrackerCache> ();
    device = impl->device;

    immediateShaderSignature = impl->immediateShaderSignature;
    immediateShaderLibrary = impl->immediateShaderLibrary.get();
    immediateVertexLayout = impl->immediateVertexLayout;
}

ImmediateRenderer::~ImmediateRenderer()
{
}

agpu::immediate_renderer_ref ImmediateRenderer::create(const agpu::state_tracker_cache_ref &cache)
{
    auto cacheImpl = cache.as<StateTrackerCache> ();
    if(!cacheImpl->ensureImmediateRendererObjectsExists())
        return agpu::immediate_renderer_ref();

    auto vertexBinding = agpu::vertex_binding_ref(cacheImpl->device->createVertexBinding(cacheImpl->immediateVertexLayout));
    if(!vertexBinding)
        return agpu::immediate_renderer_ref();

    auto uniformResourceBindings = agpu::shader_resource_binding_ref(cacheImpl->immediateShaderSignature->createShaderResourceBinding(1));
    if(!uniformResourceBindings)
        return agpu::immediate_renderer_ref();

    auto result = agpu::makeObject<ImmediateRenderer> (cache);
    result.as<ImmediateRenderer> ()->vertexBinding = vertexBinding;
    result.as<ImmediateRenderer> ()->uniformResourceBindings = uniformResourceBindings;

    return result;
}

agpu_error ImmediateRenderer::beginRendering(const agpu::state_tracker_ref &state_tracker)
{
    if(!state_tracker)
        return AGPU_NULL_POINTER;
    if(currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentStateTracker = state_tracker;
    activeMatrixStack = nullptr;
    activeMatrixStackDirtyFlag = nullptr;

    // Reset the matrices.
    projectionMatrixStack.clear();
    projectionMatrixStack.push_back(Matrix4F::identity());
    projectionMatrixStackDirtyFlag = false;

    modelViewMatrixStack.clear();
    modelViewMatrixStack.push_back(Matrix4F::identity());
    modelViewMatrixStackDirtyFlag = false;

    matrixBufferData.clear();
    matrixBufferData.push_back(Matrix4F::identity());

    currentPushConstants.projectionMatrixIndex = 0;
    currentPushConstants.modelViewMatrixIndex = 0;

    // Reset the rendering state.
    currentRenderingState.activePrimitiveTopology = AGPU_POINTS;
    currentRenderingState.flatShading = false;
    currentRenderingState.lightingEnabled = false;
    currentRenderingState.texturingEnabled = false;

    // Reset the vertices.
    lastDrawnVertexIndex = 0;
    vertices.clear();
    currentVertex.color = Vector4F(1.0f, 1.0f, 1.0f, 1.0f);
    currentVertex.normal = Vector3F(0.0f, 0.0f, 1.0f);
    currentVertex.texcoord = Vector2F(0.0f, 0.0f);

    return AGPU_OK;
}

agpu_error ImmediateRenderer::endRendering()
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    flushRenderingData();
    for(auto &command : pendingRenderingCommands)
        command();

    pendingRenderingCommands.clear();
    currentStateTracker.reset();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setBlendState(renderTargetMask, enabled);
    });
}

agpu_error ImmediateRenderer::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
    });
}

agpu_error ImmediateRenderer::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
    });
}

agpu_error ImmediateRenderer::setFrontFace(agpu_face_winding winding)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setFrontFace(winding);
    });
}

agpu_error ImmediateRenderer::setCullMode(agpu_cull_mode mode)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setCullMode(mode);
    });
}

agpu_error ImmediateRenderer::setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setDepthBias(constant_factor, clamp, slope_factor);
    });
}

agpu_error ImmediateRenderer::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setDepthState(enabled, writeMask, function);
    });
}

agpu_error ImmediateRenderer::setPolygonMode(agpu_polygon_mode mode)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setPolygonMode(mode);
    });
}

agpu_error ImmediateRenderer::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilState(enabled, writeMask, readMask);
    });
}

agpu_error ImmediateRenderer::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
    });
}

agpu_error ImmediateRenderer::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
    });
}

agpu_error ImmediateRenderer::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setViewport(x, y, w, h);
    });
}

agpu_error ImmediateRenderer::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setScissor(x, y, w, h);
    });
}

agpu_error ImmediateRenderer::setStencilReference(agpu_uint reference)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilReference(reference);
    });
}

agpu_error ImmediateRenderer::setFlatShading(agpu_bool enabled)
{
    currentRenderingState.flatShading = enabled;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setLightingEnabled(agpu_bool enabled)
{
    currentRenderingState.lightingEnabled = enabled;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::clearLights()
{
    // TODO: Implement this.
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setAmbientLighting(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    // TODO: Implement this.
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setLight(agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state)
{
    // TODO: Implement this.
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setTexturingEnabled(agpu_bool enabled)
{
    currentRenderingState.texturingEnabled = enabled;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::bindTexture(const agpu::texture_ref &texture)
{
    // TODO: Implement this.
    return AGPU_OK;
}

agpu_error ImmediateRenderer::projectionMatrixMode()
{
    activeMatrixStack = &projectionMatrixStack;
    activeMatrixStackDirtyFlag = &projectionMatrixStackDirtyFlag;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::modelViewMatrixMode()
{
    activeMatrixStack = &modelViewMatrixStack;
    activeMatrixStackDirtyFlag = &modelViewMatrixStackDirtyFlag;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::loadIdentity()
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    activeMatrixStack->back() = Matrix4F::identity();
    invalidateMatrix();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::pushMatrix()
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    activeMatrixStack->push_back(activeMatrixStack->back());
    return AGPU_OK;
}

agpu_error ImmediateRenderer::popMatrix()
{
    if(!activeMatrixStack || activeMatrixStack->size() <= 1)
        return AGPU_INVALID_OPERATION;

    activeMatrixStack->pop_back();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::ortho(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    auto tx = -(right + left)/(right - left);
    auto ty = -(top + bottom)/(top - bottom);
    auto tz = -near/(far - near);

    auto matrix = Matrix4F(
        Vector4F(2.0f/(right - left), 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 2.0f/(top - bottom), 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, -1.0f/(far - near), 0.0f),
        Vector4F(tx, ty, tz, 1.0f)
    );

    // Flip the Y axis
    if(device->hasTopLeftNdcOrigin() != device->hasBottomLeftTextureCoordinates())
    {
        matrix.c2.y = -matrix.c2.y;
        matrix.c4.y = -matrix.c4.y;
    }

    applyMatrix(matrix);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::frustum(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    auto matrix = Matrix4F(
        Vector4F(2.0f*near / (right - left), 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 2.0*far / 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, -far/(far - near), -1.0f),
        Vector4F((right + left) / (right - left), (top + bottom) / (top - bottom),  -near * far / (far - near), 0.0f)
    );

    // Flip the Y axis
    if (device->hasTopLeftNdcOrigin() != device->hasBottomLeftTextureCoordinates())
    {
        matrix.c2.y = -matrix.c2.y;
        matrix.c3.y = -matrix.c3.y;
    }

    applyMatrix(matrix);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::perspective(agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far)
{
    auto radians = fovy*(M_PI/180.0f*0.5f);
    auto top = near * tan(radians);
    auto right = top * aspect;
    return frustum(-right, right, -top, top, near, far);
}

agpu_error ImmediateRenderer::rotate(agpu_float angle, agpu_float vx, agpu_float vy, agpu_float vz)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    auto radians = angle*M_PI/180.0f;
    auto c = cos(radians);
    auto s = sin(radians);

    auto l = sqrt(vx*vx + vy*vy + vz*vz);
    auto x = vx / l;
    auto y = vy / l;
    auto z = vz / l;

    // Formula from: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
    applyMatrix(Matrix4F(
        Vector4F(x*x*(1 - c) +   c, y*x*(1 - c) + z*s, z*x*(1 - c) - y*s, 0.0f),
        Vector4F(x*y*(1 - c) - z*s, y*y*(1 - c) +   c, z*y*(1 - c) + x*s, 0.0f),
        Vector4F(x*z*(1 - c) + y*s, y*z*(1 - c) - x*s, z*z*(1 - c) +   c, 0.0f),
        Vector4F(0.0f, 0.0f, 0.0f, 1.0f)
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::translate(agpu_float x, agpu_float y, agpu_float z)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    applyMatrix(Matrix4F(
        Vector4F(1.0f, 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 1.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, 1.0f, 0.0f),
        Vector4F(x, y, z, 1.0f)
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::scale(agpu_float x, agpu_float y, agpu_float z)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    applyMatrix(Matrix4F(
        Vector4F(x, 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, y, 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, z, 0.0f),
        Vector4F(0.0f, 0.0f, 0.0f, 1.0f)
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::beginPrimitives(agpu_primitive_topology type)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    auto error = validateMatrices();
    if(error) return error;
    currentRenderingState.activePrimitiveTopology = type;

    return AGPU_OK;
}

agpu_error ImmediateRenderer::endPrimitives()
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    auto vertexCount = vertices.size() - lastDrawnVertexIndex;
    if(vertexCount > 0)
    {
        auto vertexStart = lastDrawnVertexIndex;
        auto stateToRender = currentRenderingState;
        pendingRenderingCommands.push_back([=]{
            auto error = flushRenderingState(stateToRender);
            if(!error)
                currentStateTracker->drawArrays(vertexCount, 1, vertexStart, 0);
        });
    }

    lastDrawnVertexIndex = vertices.size();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::color(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.color = Vector4F(r, g, b, a);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::texcoord(agpu_float x, agpu_float y)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.texcoord = Vector2F(x, y);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::normal(agpu_float x, agpu_float y, agpu_float z)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.normal = Vector3F(x, y, z);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::vertex(agpu_float x, agpu_float y, agpu_float z)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.position = Vector3F(x, y, z);
    vertices.push_back(currentVertex);
    return AGPU_OK;
}

void ImmediateRenderer::applyMatrix(const Matrix4F &matrix)
{
    activeMatrixStack->back() *= matrix;
    invalidateMatrix();
}

void ImmediateRenderer::invalidateMatrix()
{
    *activeMatrixStackDirtyFlag = true;
}

agpu_error ImmediateRenderer::validateMatrices()
{
    bool matricesChanged = false;
    if(projectionMatrixStackDirtyFlag)
    {
        currentPushConstants.projectionMatrixIndex = matrixBufferData.size();
        matrixBufferData.push_back(projectionMatrixStack.back());
        projectionMatrixStackDirtyFlag = false;
        matricesChanged = true;
    }

    if(modelViewMatrixStackDirtyFlag)
    {
        currentPushConstants.modelViewMatrixIndex = matrixBufferData.size();
        matrixBufferData.push_back(modelViewMatrixStack.back());
        modelViewMatrixStackDirtyFlag = false;
        matricesChanged = true;
    }

    if(matricesChanged)
    {
        auto constantsToUpdate = currentPushConstants;
        pendingRenderingCommands.push_back([=]{
            currentStateTracker->setShaderSignature(immediateShaderSignature);
            currentStateTracker->pushConstants(0, sizeof(constantsToUpdate), (void*)&constantsToUpdate);
        });
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::flushRenderingState(const ImmediateRenderingState &state)
{
    currentStateTracker->setShaderSignature(immediateShaderSignature);
    if(state.flatShading)
    {
        currentStateTracker->setVertexStage(immediateShaderLibrary->flatColorVertex, "main");
        currentStateTracker->setFragmentStage(immediateShaderLibrary->flatColorFragment, "main");
    }
    else
    {
        currentStateTracker->setVertexStage(immediateShaderLibrary->smoothColorVertex, "main");
        currentStateTracker->setFragmentStage(immediateShaderLibrary->smoothColorFragment, "main");
    }

    currentStateTracker->setPrimitiveType(state.activePrimitiveTopology);
    currentStateTracker->setVertexLayout(immediateVertexLayout);
    currentStateTracker->useVertexBinding(vertexBinding);
    currentStateTracker->useShaderResources(uniformResourceBindings);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::flushRenderingData()
{
    // Upload the vertices.
    agpu_error error = AGPU_OK;
    if(!vertexBuffer || vertexBufferCapacity < vertices.size())
    {
        vertexBufferCapacity = nextPowerOfTwo(vertices.size());
        if(vertexBufferCapacity < 32)
            vertexBufferCapacity = 32;

        auto requiredSize = vertexBufferCapacity*sizeof(ImmediateRendererVertex);
        agpu_buffer_description bufferDescription = {};
        bufferDescription.size = requiredSize;
        bufferDescription.heap_type = requiredSize >= GpuBufferDataThreshold
            ? AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL : AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
        bufferDescription.binding = AGPU_ARRAY_BUFFER;
        bufferDescription.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT;
        bufferDescription.stride = sizeof(ImmediateRendererVertex);

        vertexBuffer = agpu::buffer_ref(device->createBuffer(&bufferDescription, nullptr));
        if(!vertexBuffer)
            return AGPU_OUT_OF_MEMORY;

        vertexBinding->bindVertexBuffers(1, &vertexBuffer);
    }

    if(!vertices.empty())
    {
        error = vertexBuffer->uploadBufferData(0, vertices.size()*sizeof(ImmediateRendererVertex), &vertices[0]);
        if(error)
            return error;
    }

    // Upload the matrices.
    if(!matrixBuffer || matrixBufferCapacity < matrixBufferData.size())
    {
        matrixBufferCapacity = nextPowerOfTwo(matrixBufferData.size());
        if(matrixBufferCapacity < 32)
            matrixBufferCapacity = 32;

        auto requiredSize = matrixBufferCapacity*sizeof(Matrix4F);
        agpu_buffer_description bufferDescription = {};
        bufferDescription.size = requiredSize;
        bufferDescription.heap_type = requiredSize >= GpuBufferDataThreshold
            ? AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL : AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
        bufferDescription.binding = AGPU_STORAGE_BUFFER;
        bufferDescription.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT;
        bufferDescription.stride = sizeof(Matrix4F);

        matrixBuffer = agpu::buffer_ref(device->createBuffer(&bufferDescription, nullptr));
        if(!matrixBuffer)
            return AGPU_OUT_OF_MEMORY;

        uniformResourceBindings->bindStorageBuffer(0, matrixBuffer);
    }

    if(!matrixBufferData.empty())
    {
        error = matrixBuffer->uploadBufferData(0, matrixBufferData.size()*sizeof(Matrix4F), &matrixBufferData[0]);
        if(error)
            return error;
    }

    return AGPU_OK;
}

} // End of namespace AgpuCommon
