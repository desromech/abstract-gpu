#ifndef AGPU_PIPELINE_STATE_HPP_
#define AGPU_PIPELINE_STATE_HPP_

#include "device.hpp"
#include "shader.hpp"

namespace AgpuGL
{

struct CommandListExecutionContext;
struct GLShaderForSignature;
typedef agpu::ref<GLShaderForSignature> GLShaderForSignatureRef;

enum class AgpuPipelineStateType
{
	Graphics = 0,
	Compute
};

class AgpuPipelineStateData
{
public:
	virtual ~AgpuPipelineStateData() {}

	virtual void activate() = 0;
	virtual void updateStencilReference(int reference) = 0;
	virtual void setBaseInstance(agpu_uint base_instance) {}

	virtual agpu_primitive_topology getPrimitiveTopology()
	{
		return AGPU_POINTS;
	}
};

class AgpuGraphicsPipelineStateData : public AgpuPipelineStateData
{
public:
	virtual void activate() override;
	virtual void updateStencilReference(int reference) override;
	virtual void setBaseInstance(agpu_uint base_instance) override;
	void enableState(bool enabled, GLenum state);

	virtual agpu_primitive_topology getPrimitiveTopology() override
	{
		return primitiveTopology;
	}

	agpu::device_ref device;

	// States
	agpu_bool depthEnabled;
	agpu_bool depthWriteMask;
	GLenum depthFunction;

	// Face culling
	GLenum frontFaceWinding;
	GLenum cullingMode;

	// Color buffer
	agpu_bool blendingEnabled;
	agpu_bool redMask;
	agpu_bool greenMask;
	agpu_bool blueMask;
	agpu_bool alphaMask;
	GLenum sourceBlendFactor;
	GLenum destBlendFactor;
	GLenum blendOperation;
	GLenum sourceBlendFactorAlpha;
	GLenum destBlendFactorAlpha;
	GLenum blendOperationAlpha;

	// Stencil testing
	agpu_bool stencilEnabled;
	int stencilWriteMask;
	int stencilReadMask;

	GLenum stencilFrontFailOp;
	GLenum stencilFrontDepthFailOp;
	GLenum stencilFrontDepthPassOp;
	GLenum stencilFrontFunc;

	GLenum stencilBackFailOp;
	GLenum stencilBackDepthFailOp;
	GLenum stencilBackDepthPassOp;
	GLenum stencilBackFunc;

	// Multisampling
    agpu_uint sampleCount;
    agpu_uint sampleQuality;

	// Alpha testing
	bool alphaTestEnabled;
	GLenum alphaTestFunction;

	// Miscellaneous
	int renderTargetCount;
	bool hasSRGBTarget;
	agpu_primitive_topology primitiveTopology;

	// The instance base index
	GLint baseInstanceUniformIndex;
};

struct GLPipelineState: public agpu::pipeline_state
{
public:
    GLPipelineState();
	~GLPipelineState();

    agpu_int getUniformLocation ( agpu_cstring name );
    void activateShaderResourcesOn(CommandListExecutionContext *context, agpu::shader_resource_binding_ref *shaderResources);
	void uploadPushConstants(const uint8_t *pushConstantBuffer, size_t pushConstantBufferSize);
	void setBaseInstance(agpu_uint base_instance);

public:
    agpu::device_ref device;
    agpu::shader_signature_ref shaderSignature;
    std::vector<GLShaderForSignatureRef> shaderInstances;
    std::vector<MappedTextureWithSamplerCombination> mappedTextureWithSamplerCombinations;
    GLuint programHandle;

	AgpuPipelineStateType type;
	AgpuPipelineStateData *extraStateData;

public:
    void activate();
    void updateStencilReference(int reference);
};

} // End of namespace AgpuGL

#endif //AGPU_PIPELINE_STATE_HPP_
