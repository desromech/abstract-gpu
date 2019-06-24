#include "state_tracker_cache.hpp"
#include "state_tracker.hpp"
#include "immediate_renderer.hpp"

#define CHECK_ERROR() if(error) return error

namespace AgpuCommon
{

template<typename T>
size_t hashOf(const T &v)
{
    return std::hash<T> ()(v);
}

size_t hashOfEnum(uint32_t v)
{
    return hashOf(v);
}

// ShaderStageDescription
ShaderStageDescription::ShaderStageDescription()
{
    reset();
}

ShaderStageDescription::~ShaderStageDescription()
{
}

void ShaderStageDescription::reset()
{
    shader.reset();
    entryPoint.clear();
}

bool ShaderStageDescription::operator==(const ShaderStageDescription &o) const
{
    return shader == o.shader && entryPoint == o.entryPoint;
}

size_t ShaderStageDescription::hash() const
{
    return hashOf(shader) ^ hashOf(entryPoint);
}

// RenderTargetColorAttachmentDescription
RenderTargetColorAttachmentDescription::RenderTargetColorAttachmentDescription()
{
    reset();
}

RenderTargetColorAttachmentDescription::~RenderTargetColorAttachmentDescription()
{
}

void RenderTargetColorAttachmentDescription::reset()
{
    // Attachment description.
    textureFormat = AGPU_TEXTURE_FORMAT_UNKNOWN;

    // Blending
    blendingEnabled = false;
    sourceColorBlendingFactor = AGPU_BLENDING_ONE;
    destColorBlendingFactor = AGPU_BLENDING_ZERO;
    colorBlendingOperation = AGPU_BLENDING_OPERATION_ADD;
    sourceAlphaBlendingFactor = AGPU_BLENDING_ONE;
    destAlphaBlendingFactor = AGPU_BLENDING_ZERO;
    alphaBlendingOperation = AGPU_BLENDING_OPERATION_ADD;

    // Color mask.
    redColorMask = true;
    greenColorMask = true;
    blueColorMask = true;
    alphaColorMask = true;
}

agpu_error RenderTargetColorAttachmentDescription::applyToBuilder(agpu_int index, const agpu::pipeline_builder_ref &builder) const
{
    auto renderTargetMask = 1<<index;
    auto error = builder->setRenderTargetFormat(index, textureFormat); CHECK_ERROR();
    error = builder->setBlendState(renderTargetMask, blendingEnabled); CHECK_ERROR();
    error = builder->setBlendFunction(renderTargetMask,
                sourceColorBlendingFactor, destColorBlendingFactor, colorBlendingOperation,
                sourceAlphaBlendingFactor, destAlphaBlendingFactor, alphaBlendingOperation); CHECK_ERROR();
    error = builder->setColorMask(renderTargetMask, redColorMask, greenColorMask, blueColorMask, alphaColorMask); CHECK_ERROR();
    return AGPU_OK;
}

bool RenderTargetColorAttachmentDescription::operator==(const RenderTargetColorAttachmentDescription &o) const
{
    return
        textureFormat == o.textureFormat &&

        // Blending
        blendingEnabled == o.blendingEnabled &&
        sourceColorBlendingFactor == o.sourceColorBlendingFactor &&
        destColorBlendingFactor == o.destColorBlendingFactor &&
        colorBlendingOperation == o.colorBlendingOperation &&
        sourceAlphaBlendingFactor == o.sourceAlphaBlendingFactor &&
        destAlphaBlendingFactor == o.destAlphaBlendingFactor &&
        alphaBlendingOperation == o.alphaBlendingOperation &&

        // Color mask.
        redColorMask == o.redColorMask &&
        greenColorMask == o.greenColorMask &&
        blueColorMask == o.blueColorMask &&
        alphaColorMask == o.alphaColorMask;
}

size_t RenderTargetColorAttachmentDescription::hash() const
{
    return
        hashOfEnum(textureFormat) ^

        // Blending
        hashOf(blendingEnabled) ^
        hashOfEnum(sourceColorBlendingFactor) ^
        hashOfEnum(destColorBlendingFactor) ^
        hashOfEnum(colorBlendingOperation) ^
        hashOfEnum(sourceAlphaBlendingFactor) ^
        hashOfEnum(destAlphaBlendingFactor) ^
        hashOfEnum(alphaBlendingOperation) ^

        // Color mask.
        hashOf(redColorMask) ^
        hashOf(greenColorMask) ^
        hashOf(blueColorMask) ^
        hashOf(alphaColorMask);
}

// GraphicsPipelineStateDescription
GraphicsPipelineStateDescription::GraphicsPipelineStateDescription()
{
}

GraphicsPipelineStateDescription::~GraphicsPipelineStateDescription()
{
}

void GraphicsPipelineStateDescription::reset()
{
    shaderSignature.reset();
    vertexStage.reset();
    fragmentStage.reset();
    geometryStage.reset();
    tessellationControlStage.reset();
    tessellationEvaluationStage.reset();

    // Color attachments
    renderTargetColorAttachmentCount = 0;
    for(size_t i = 0; i < MaxRenderTargetAttachmentCount; ++i)
        renderTargetColorAttachments[i].reset();

    // Depth stencil.
    depthStencilFormat = AGPU_TEXTURE_FORMAT_UNKNOWN;

    depthTestingEnabled = false;
    depthWriteMask = false;
    depthCompareFunction = AGPU_LESS_EQUAL;

    depthBiasEnabled = false;
    depthBiasConstantFactor = 0.0f;
    depthBiasClamp = 0.0f;
    depthBiasSlopeFactor = 0.0f;

    stencilTestingEnabled = false;
    stencilWriteMask = ~0;
    stencilReadMask = ~0;

    frontStencilFailOperation = AGPU_KEEP;
    frontStencilDepthFailOperation = AGPU_KEEP;
    frontStencilDepthPassOperation = AGPU_KEEP;
    frontStencilCompareFunction = AGPU_ALWAYS;

    backStencilFailOperation = AGPU_KEEP;
    backStencilDepthFailOperation = AGPU_KEEP;
    backStencilDepthPassOperation = AGPU_KEEP;
    backStencilCompareFunction = AGPU_ALWAYS;

    // Face culling
    frontFaceWinding = AGPU_COUNTER_CLOCKWISE;
    faceCullingMode = AGPU_CULL_MODE_NONE;

    // Rasterization
    polygonMode = AGPU_POLYGON_MODE_FILL;
    primitiveType = AGPU_POINTS;
    vertexLayout.reset();
    sampleCount = 1;
    sampleQuality = 1;
}

agpu_error GraphicsPipelineStateDescription::applyToBuilder(const agpu::pipeline_builder_ref &builder) const
{
    // Setup the shaders.
    auto error = builder->setShaderSignature(shaderSignature); CHECK_ERROR();
    error = vertexStage.attachTo(AGPU_VERTEX_SHADER, builder); CHECK_ERROR();
    error = fragmentStage.attachTo(AGPU_FRAGMENT_SHADER, builder); CHECK_ERROR();
    error = geometryStage.attachTo(AGPU_GEOMETRY_SHADER, builder); CHECK_ERROR();
    error = tessellationControlStage.attachTo(AGPU_TESSELLATION_CONTROL_SHADER, builder); CHECK_ERROR();
    error = tessellationEvaluationStage.attachTo(AGPU_TESSELLATION_EVALUATION_SHADER, builder); CHECK_ERROR();

    // Setup the color render target.
    error = builder->setRenderTargetCount(renderTargetColorAttachmentCount); CHECK_ERROR();
    for(size_t i = 0; i < renderTargetColorAttachmentCount; ++i)
    {
        error = renderTargetColorAttachments[i].applyToBuilder(i, builder); CHECK_ERROR();
    }

    // Depth stencil.
    error = builder->setDepthStencilFormat(depthStencilFormat); CHECK_ERROR();
    if(depthStencilFormat == AGPU_TEXTURE_FORMAT_UNKNOWN)
    {
        error = builder->setDepthState(false, false, AGPU_ALWAYS); CHECK_ERROR();
        error = builder->setStencilState(false, ~0, ~0); CHECK_ERROR();
    }
    else
    {
        error = builder->setDepthState(depthTestingEnabled, depthWriteMask, depthCompareFunction); CHECK_ERROR();

        error = builder->setDepthBias(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor); CHECK_ERROR();
        error = builder->setStencilState(stencilTestingEnabled, stencilWriteMask, stencilReadMask); CHECK_ERROR();

        error = builder->setStencilFrontFace(frontStencilFailOperation, frontStencilDepthFailOperation, frontStencilDepthPassOperation, frontStencilCompareFunction); CHECK_ERROR();
        error = builder->setStencilBackFace(backStencilFailOperation, backStencilDepthFailOperation, backStencilDepthPassOperation, backStencilCompareFunction); CHECK_ERROR();
    }

    // Face culling
    error = builder->setFrontFace(frontFaceWinding); CHECK_ERROR();
    error = builder->setCullMode(faceCullingMode); CHECK_ERROR();

    // Rasterization
    error = builder->setPolygonMode(polygonMode); CHECK_ERROR();
    error = builder->setPrimitiveType(primitiveType); CHECK_ERROR();
    error = builder->setVertexLayout(vertexLayout); CHECK_ERROR();
    error = builder->setSampleDescription(sampleCount, sampleQuality); CHECK_ERROR();
    return AGPU_OK;
}

bool GraphicsPipelineStateDescription::operator==(const GraphicsPipelineStateDescription &o) const
{
    auto result =
        shaderSignature == o.shaderSignature &&
        vertexStage == o.vertexStage &&
        fragmentStage == o.fragmentStage &&
        geometryStage == o.geometryStage &&
        tessellationControlStage == o.tessellationControlStage &&
        tessellationEvaluationStage == o.tessellationEvaluationStage &&

        depthStencilFormat == o.depthStencilFormat &&

        // Depth stencil.
        depthTestingEnabled == o.depthTestingEnabled &&
        depthWriteMask == o.depthWriteMask &&
        depthCompareFunction == o.depthCompareFunction &&

        depthBiasEnabled == o.depthBiasEnabled &&
        depthBiasConstantFactor == o.depthBiasConstantFactor &&
        depthBiasClamp == o.depthBiasClamp &&
        depthBiasSlopeFactor == o.depthBiasSlopeFactor &&

        stencilTestingEnabled == o.stencilTestingEnabled &&
        stencilWriteMask == o.stencilWriteMask &&
        stencilReadMask == o.stencilReadMask &&

        frontStencilFailOperation == o.frontStencilFailOperation &&
        frontStencilDepthFailOperation == o.frontStencilDepthFailOperation &&
        frontStencilDepthPassOperation == o.frontStencilDepthPassOperation &&
        frontStencilCompareFunction == o.frontStencilCompareFunction &&

        backStencilFailOperation == o.backStencilFailOperation &&
        backStencilDepthFailOperation == o.backStencilDepthFailOperation &&
        backStencilDepthPassOperation == o.backStencilDepthPassOperation &&
        backStencilCompareFunction == o.backStencilCompareFunction &&

        // Face culling
        frontFaceWinding == o.frontFaceWinding &&
        faceCullingMode == o.faceCullingMode &&

        // Rasterization
        polygonMode == o.polygonMode &&
        primitiveType == o.primitiveType &&
        vertexLayout == o.vertexLayout &&
        sampleCount == o.sampleCount &&
        sampleQuality == o.sampleQuality &&

        renderTargetColorAttachmentCount == o.renderTargetColorAttachmentCount;

    if(!result) return false;

    for(size_t i = 0; i < renderTargetColorAttachmentCount; ++i)
    {
        if(!(renderTargetColorAttachments[i] == o.renderTargetColorAttachments[i]))
            return false;
    }

    return true;
}

size_t GraphicsPipelineStateDescription::hash() const
{
    auto result =
        shaderSignature.hash() ^
        vertexStage.hash() ^
        fragmentStage.hash() ^
        geometryStage.hash() ^
        tessellationControlStage.hash() ^
        tessellationEvaluationStage.hash() ^

        // Depth stencil
        hashOfEnum(depthStencilFormat) ^

        hashOf(depthTestingEnabled) ^
        hashOf(depthWriteMask) ^
        hashOfEnum(depthCompareFunction) ^

        hashOf(depthBiasEnabled) ^
        hashOf(depthBiasConstantFactor) ^
        hashOf(depthBiasClamp) ^
        hashOf(depthBiasSlopeFactor) ^

        hashOf(stencilTestingEnabled) ^
        hashOf(stencilWriteMask) ^
        hashOf(stencilReadMask) ^

        hashOfEnum(frontStencilFailOperation) ^
        hashOfEnum(frontStencilDepthFailOperation) ^
        hashOfEnum(frontStencilDepthPassOperation) ^
        hashOfEnum(frontStencilCompareFunction) ^

        hashOfEnum(backStencilFailOperation) ^
        hashOfEnum(backStencilDepthFailOperation) ^
        hashOfEnum(backStencilDepthPassOperation) ^
        hashOfEnum(backStencilCompareFunction) ^

        // Face culling
        hashOfEnum(frontFaceWinding) ^
        hashOfEnum(faceCullingMode) ^

        // Rasterization
        hashOfEnum(polygonMode) ^
        hashOfEnum(primitiveType) ^
        vertexLayout.hash() ^
        hashOf(sampleCount) ^
        hashOf(sampleQuality) ^

        hashOf(renderTargetColorAttachmentCount);

    for(size_t i = 0; i < renderTargetColorAttachmentCount; ++i)
        result ^= renderTargetColorAttachments[i].hash();

    return result;
}

// ComputePipelineStateDescription
ComputePipelineStateDescription::ComputePipelineStateDescription()
{
    reset();
}

ComputePipelineStateDescription::~ComputePipelineStateDescription()
{
}

void ComputePipelineStateDescription::reset()
{
    shaderSignature.reset();
    computeStage.reset();
}

agpu_error ComputePipelineStateDescription::applyToBuilder(const agpu::compute_pipeline_builder_ref &builder) const
{
    auto error = builder->setShaderSignature(shaderSignature); CHECK_ERROR();
    error = computeStage.attachTo(AGPU_COMPUTE_SHADER, builder); CHECK_ERROR();
    return AGPU_OK;
}

bool ComputePipelineStateDescription::operator==(const ComputePipelineStateDescription &o) const
{
    return shaderSignature == o.shaderSignature && computeStage == o.computeStage;
}

size_t ComputePipelineStateDescription::hash() const
{
    return hashOf(shaderSignature) ^ computeStage.hash();
}

// StateTrackerCache
StateTrackerCache::StateTrackerCache(const agpu::device_ref &device, uint32_t queueFamilyType)
    : device(device), queueFamilyType(queueFamilyType)
{
    immediateRendererObjectsInitialized = false;
}

StateTrackerCache::~StateTrackerCache()
{
}

agpu::state_tracker_cache_ref StateTrackerCache::create(const agpu::device_ref &device, uint32_t queueFamilyType)
{
    return agpu::makeObject<StateTrackerCache> (device, queueFamilyType);
}

agpu::state_tracker_ptr StateTrackerCache::createStateTracker(agpu_command_list_type type, const agpu::command_queue_ref & command_queue)
{
    auto commandAllocator = agpu::command_allocator_ref(device->createCommandAllocator(type, command_queue));
    if(!commandAllocator) return nullptr;

    return DirectStateTracker::create(refFromThis<agpu::state_tracker_cache> (), device, type, command_queue, commandAllocator, true).disown();
}

agpu::state_tracker_ptr StateTrackerCache::createStateTrackerWithCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, const agpu::command_allocator_ref & command_allocator)
{
    return DirectStateTracker::create(refFromThis<agpu::state_tracker_cache> (), device, type, command_queue, command_allocator, false).disown();
}

agpu::state_tracker_ptr StateTrackerCache::createStateTrackerWithFrameBuffering(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, agpu_uint framebuffering_count)
{
    return FrameBufferredStateTracker::create(refFromThis<agpu::state_tracker_cache> (), device, type, command_queue, framebuffering_count).disown();
}

agpu::immediate_renderer_ptr StateTrackerCache::createImmediateRenderer()
{
    return ImmediateRenderer::create(refFromThis<agpu::state_tracker_cache> ()).disown();
}

agpu::pipeline_state_ref StateTrackerCache::getComputePipelineWithDescription(const ComputePipelineStateDescription &description, std::string &pipelineBuildErrorLog)
{
    std::unique_lock<std::mutex> l(computePipelineStateCacheMutex);

    // Find an existent
    {
        auto it = computePipelineStateCache.find(description);
        if(it != computePipelineStateCache.end())
            return it->second;
    }

    // Create the pipeline builder.
    auto builder = agpu::compute_pipeline_builder_ref(device->createComputePipelineBuilder());
    if(!builder)
    {
        pipelineBuildErrorLog += "Failed to create compute pipeline builder.\n";
        return agpu::pipeline_state_ref();
    }

    // Apply the graphics pipeline description.
    auto error = description.applyToBuilder(builder);
    if(error)
    {
        pipelineBuildErrorLog += "Failed to set some pipeline state builder parameters.\n";
        return agpu::pipeline_state_ref();
    }

    // Build the pipeline state object.
    auto pso = agpu::pipeline_state_ref(builder->build());
    if(!pso)
    {
        std::vector<char> psoBuildLog(builder->getBuildingLogLength() + 1);
        if(!psoBuildLog.empty())
        {
            builder->getBuildingLog(psoBuildLog.size(), &psoBuildLog[0]);
            pipelineBuildErrorLog += &psoBuildLog[0];
        }

        return agpu::pipeline_state_ref();
    }

    // Store a copy of the PSO.
    computePipelineStateCache.insert(std::make_pair(description, pso));

    // Return the PSO.
    return pso;
}

agpu::pipeline_state_ref StateTrackerCache::getGraphicsPipelineWithDescription(const GraphicsPipelineStateDescription &description, std::string &pipelineBuildErrorLog)
{
    std::unique_lock<std::mutex> l(graphicsPipelineStateCacheMutex);

    // Find an existent
    {
        auto it = graphicsPipelineStateCache.find(description);
        if(it != graphicsPipelineStateCache.end())
            return it->second;
    }

    // Create the pipeline builder.
    auto builder = agpu::pipeline_builder_ref(device->createPipelineBuilder());
    if(!builder)
    {
        pipelineBuildErrorLog += "Failed to create graphics pipeline builder.\n";
        return agpu::pipeline_state_ref();
    }

    // Apply the graphics pipeline description.
    auto error = description.applyToBuilder(builder);
    if(error)
    {
        pipelineBuildErrorLog += "Failed to set some pipeline state builder parameters.\n";
        return agpu::pipeline_state_ref();
    }

    // Build the pipeline state object.
    auto pso = agpu::pipeline_state_ref(builder->build());
    if(!pso)
    {
        std::vector<char> psoBuildLog(builder->getBuildingLogLength() + 1);
        if(!psoBuildLog.empty())
        {
            builder->getBuildingLog(psoBuildLog.size(), &psoBuildLog[0]);
            pipelineBuildErrorLog += &psoBuildLog[0];
        }

        return agpu::pipeline_state_ref();
    }

    // Store a copy of the PSO.
    graphicsPipelineStateCache.insert(std::make_pair(description, pso));

    // Return the PSO.
    return pso;
}

} // End of namespace AgpuCommon
