#ifndef AGPU_D3D12_SHADER_SIGNATURE_HPP
#define AGPU_D3D12_SHADER_SIGNATURE_HPP

#include <vector>
#include "device.hpp"
#include "shader_signature_builder.hpp"

class DescriptorAllocator
{
public:
    DescriptorAllocator(int startIndex, int descriptorCount);
    ~DescriptorAllocator();

    int getStartIndex();
    int allocate();
    void free(int descriptorIndex);

private:
    int startIndex;
    int descriptorCount;
    int freeCount;
    std::vector<int> freeList;
};

struct _agpu_shader_signature : public Object<_agpu_shader_signature>
{
public:
    _agpu_shader_signature();

    void lostReferences();

    static _agpu_shader_signature *create(agpu_device *device, const ComPtr<ID3D12RootSignature> &rootSignature, ShaderSignatureElementDescription elementsDescription[16], agpu_uint maxBindingsCount[AGPU_SHADER_BINDING_TYPE_COUNT]);

    agpu_shader_resource_binding* createShaderResourceBinding(agpu_uint element);

public:
    agpu_device *device;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12DescriptorHeap> shaderResourceViewHeap;
    ComPtr<ID3D12DescriptorHeap> samplerHeap;

    ShaderSignatureElementDescription elementsDescription[16];

private:

    agpu_uint maxBindingsCount[AGPU_SHADER_BINDING_TYPE_COUNT];
    DescriptorAllocator *descriptorAllocators[16];

    UINT shaderResourceViewDescriptorSize;
    UINT samplerDescriptorSize;

    std::mutex allocationMutex;
};

#endif //AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
