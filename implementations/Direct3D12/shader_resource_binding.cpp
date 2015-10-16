#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"

_agpu_shader_resource_binding::_agpu_shader_resource_binding()
{
}

void _agpu_shader_resource_binding::lostReferences()
{
    if (signature)
        signature->release();

    for (auto &buffer : buffers)
    {
        if (buffer)
            buffer->release();
    }

    for (auto &texture : textures)
    {
        if (texture)
            texture->release();
    }
}

agpu_shader_resource_binding *_agpu_shader_resource_binding::create(agpu_shader_signature *signature, agpu_uint element, UINT descriptorOffset)
{
    std::unique_ptr<agpu_shader_resource_binding> binding(new agpu_shader_resource_binding());
    binding->device = signature->device;
    binding->signature = signature;
    signature->retain();
    binding->element= element;
    binding->descriptorOffset = descriptorOffset;

    auto &elementDesc = signature->elementsDescription[element];
    binding->isBank = elementDesc.bank;
    binding->type = elementDesc.type;
    
    if (binding->type == AGPU_SHADER_BINDING_TYPE_SAMPLER)
    {
        binding->samplerCount = elementDesc.bindingPointCount;
    }
    else
    {
        binding->buffers.resize(elementDesc.bindingPointCount);
        binding->textures.resize(elementDesc.bindingPointCount);
    }

    return binding.release();
}

agpu_error _agpu_shader_resource_binding::bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(uniform_buffer);
    return bindUniformBufferRange(location, uniform_buffer, 0, uniform_buffer->view.constantBuffer.SizeInBytes);
}

agpu_error _agpu_shader_resource_binding::bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    std::unique_lock<std::mutex> (bindMutex);
    if (type != AGPU_SHADER_BINDING_TYPE_CBV)
        return AGPU_INVALID_OPERATION;

    if (uniform_buffer->description.binding != AGPU_UNIFORM_BUFFER)
        return AGPU_ERROR;

    if (location < 0)
        return AGPU_OK;
    if (location >= (int)buffers.size())
        return AGPU_OUT_OF_BOUNDS;

    // Store the uniform buffer
    uniform_buffer->retain();
    if (buffers[location])
        buffers[location]->release();
    buffers[location] = uniform_buffer;

    // Compute the actual view.
    auto view = uniform_buffer->view.constantBuffer;
    view.BufferLocation += offset,
    view.SizeInBytes = (size + 255) & (~255);

    // Set the descriptor location
    auto desc = signature->shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
    desc.ptr += descriptorOffset + location*signature->shaderResourceViewDescriptorSize;
    device->d3dDevice->CreateConstantBufferView(&view, desc);

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::bindTexture(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodClamp)
{
    return bindTextureArrayRange(location, texture, startMiplevel, miplevels, -1, -1, lodClamp);
}

agpu_error _agpu_shader_resource_binding::bindTextureArrayRange(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodClamp)
{
    CHECK_POINTER(texture);

    std::unique_lock<std::mutex> (bindMutex);
    if (type != AGPU_SHADER_BINDING_TYPE_SRV)
        return AGPU_INVALID_OPERATION;

    if (location < 0)
        return AGPU_OK;
    if (location >= (int)textures.size())
        return AGPU_OUT_OF_BOUNDS;

    if (texture->description.type == AGPU_TEXTURE_UNKNOWN)
        return AGPU_UNSUPPORTED;

    // Store the texture.
    texture->retain();
    if (textures[location])
        textures[location]->release();
    textures[location] = texture;

    // Create the shader resource view
    D3D12_SHADER_RESOURCE_VIEW_DESC view;
    view.Format = texture->resourceDescription.Format;
    view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    bool isArray = texture->isArray();
    UINT actualFirstElement = firstElement < 0 ? 0 : numberOfElements;
    UINT arraySize = numberOfElements < 0 ? texture->description.depthOrArraySize - actualFirstElement : numberOfElements;
    if(lodClamp < 0)
        lodClamp = 100000.0f;

    memset(&view, 0, sizeof(view));
    view.Format = texture->resourceDescription.Format;
    view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    switch (texture->description.type)
    {
    case AGPU_TEXTURE_BUFFER:
        return AGPU_UNIMPLEMENTED;
        break;
    case AGPU_TEXTURE_1D:
        if (isArray)
        {
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            view.Texture1DArray.MostDetailedMip = startMiplevel;
            view.Texture1DArray.MipLevels = miplevels;
            view.Texture1DArray.FirstArraySlice = actualFirstElement;
            view.Texture1DArray.ArraySize = arraySize;
            view.Texture1DArray.ResourceMinLODClamp = lodClamp;
        }
        else
        {
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            view.Texture1D.MostDetailedMip = startMiplevel;
            view.Texture1D.MipLevels = miplevels;
            view.Texture1D.ResourceMinLODClamp = lodClamp;
        }
        break;
    case AGPU_TEXTURE_2D:
        if (isArray)
        {
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            view.Texture2DArray.MostDetailedMip = startMiplevel;
            view.Texture2DArray.MipLevels = miplevels;
            view.Texture2DArray.FirstArraySlice = actualFirstElement;
            view.Texture2DArray.ArraySize = arraySize;
            view.Texture2DArray.ResourceMinLODClamp = lodClamp;
        }
        else
        {
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            view.Texture2D.MostDetailedMip = startMiplevel;
            view.Texture2D.MipLevels = miplevels;
            view.Texture2D.ResourceMinLODClamp = lodClamp;
        }
        break;
    case AGPU_TEXTURE_CUBE:
        if (isArray)
        {
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
            view.TextureCubeArray.MostDetailedMip = startMiplevel;
            view.TextureCubeArray.MipLevels = miplevels;
            view.TextureCubeArray.First2DArrayFace = actualFirstElement;
            view.TextureCubeArray.NumCubes= arraySize;
            view.TextureCubeArray.ResourceMinLODClamp = lodClamp;
        }
        else
        {
            view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            view.TextureCube.MostDetailedMip = startMiplevel;
            view.TextureCube.MipLevels = miplevels;
            view.TextureCube.ResourceMinLODClamp = lodClamp;
        }
        break;
    case AGPU_TEXTURE_3D:
        view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        view.Texture3D.MostDetailedMip = startMiplevel;
        view.Texture3D.MipLevels = miplevels;
        view.Texture3D.ResourceMinLODClamp = lodClamp;
        break;
    default:
        abort();
    }

    // Set the descriptor.
    auto cpuHandle = signature->shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += descriptorOffset + location*signature->shaderResourceViewDescriptorSize;
    device->d3dDevice->CreateShaderResourceView(texture->gpuResource.Get(), &view, cpuHandle);

    return AGPU_OK;
}

agpu_error _agpu_shader_resource_binding::createSampler(agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(description);

    std::unique_lock<std::mutex>(bindMutex);
    if (type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

    if (location < 0)
        return AGPU_OK;
    if (location >= samplerCount)
        return AGPU_OUT_OF_BOUNDS;

    // Create the sampler description
    D3D12_SAMPLER_DESC sampler;
    memset(&sampler, 0, sizeof(sampler));
    sampler.Filter = (D3D12_FILTER)description->filter;
    sampler.AddressU = (D3D12_TEXTURE_ADDRESS_MODE)description->address_u;
    sampler.AddressV = (D3D12_TEXTURE_ADDRESS_MODE)description->address_v;
    sampler.AddressW = (D3D12_TEXTURE_ADDRESS_MODE)description->address_w;
    sampler.MipLODBias = description->mip_lod_bias;
    sampler.MaxAnisotropy= description->maxanisotropy;
    sampler.BorderColor[0]= description->border_color.r;
    sampler.BorderColor[1] = description->border_color.g;
    sampler.BorderColor[2] = description->border_color.b;
    sampler.BorderColor[3] = description->border_color.a;
    sampler.MinLOD = description->min_lod;
    sampler.MaxLOD = description->max_lod;

    // Set the descriptor.
    auto cpuHandle = signature->samplerHeap->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += descriptorOffset + location*signature->samplerDescriptorSize;
    device->d3dDevice->CreateSampler(&sampler, cpuHandle);

    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference(agpu_shader_resource_binding* shader_resource_binding)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding(agpu_shader_resource_binding* shader_resource_binding)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->release();
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBuffer(location, uniform_buffer);
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindUniformBufferRange(location, uniform_buffer, offset, size);
}

AGPU_EXPORT agpu_error agpuBindTexture(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindTexture(location, texture, startMiplevel, miplevels, lodclamp);
}

AGPU_EXPORT agpu_error agpuBindTextureArrayRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->bindTextureArrayRange(location, texture, startMiplevel, miplevels, firstElement, numberOfElements, lodclamp);
}

AGPU_EXPORT agpu_error agpuCreateSampler(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(shader_resource_binding);
    return shader_resource_binding->createSampler(location, description);
}
