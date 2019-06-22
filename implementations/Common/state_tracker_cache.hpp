#ifndef AGPU_STATE_TRACKER_CACHE_HPP
#define AGPU_STATE_TRACKER_CACHE_HPP

#include <AGPU/agpu_impl.hpp>
#include <unordered_map>
#include <array>
#include <mutex>
#include <string>

namespace AgpuCommon
{

/**
 * I am a description of a shader stage binding.
 */
struct ShaderStageDescription
{
    ShaderStageDescription();
    ~ShaderStageDescription();

    bool set(const agpu::shader_ref &newShader, agpu_cstring newEntryPoint)
    {
        if(shader == newShader && entryPoint == newEntryPoint)
            return false;

        shader = newShader;
        entryPoint = newEntryPoint;
        return true;
    }

    void reset();

    bool operator==(const ShaderStageDescription &o) const;
    size_t hash() const;

    template<typename BT>
    agpu_error attachTo(agpu_shader_type type, const BT &builder) const
    {
        if(!shader) return AGPU_OK;
        return builder->attachShaderWithEntryPoint(shader, type, entryPoint.c_str());
    }


    agpu::shader_ref shader;
    std::string entryPoint;
};

/**
 * I am a description for a render target color attachment
 */
struct RenderTargetColorAttachmentDescription
{
    RenderTargetColorAttachmentDescription();
    ~RenderTargetColorAttachmentDescription();

    void reset();

    bool operator==(const RenderTargetColorAttachmentDescription &o) const;
    size_t hash() const;

    agpu_error applyToBuilder(agpu_int index, const agpu::pipeline_builder_ref &builder) const;

    // Attachment description.
    agpu_texture_format textureFormat;

    // Blending
    bool blendingEnabled;
    agpu_blending_factor sourceColorBlendingFactor;
    agpu_blending_factor destColorBlendingFactor;
    agpu_blending_operation colorBlendingOperation;
    agpu_blending_factor sourceAlphaBlendingFactor;
    agpu_blending_factor destAlphaBlendingFactor;
    agpu_blending_operation alphaBlendingOperation;

    // Color mask.
    bool redColorMask;
    bool greenColorMask;
    bool blueColorMask;
    bool alphaColorMask;

};

/**
 * I am a description for a graphics pipeline state.
 */
struct GraphicsPipelineStateDescription
{
    GraphicsPipelineStateDescription();
    ~GraphicsPipelineStateDescription();

    void reset();
    agpu_error applyToBuilder(const agpu::pipeline_builder_ref &builder) const;

    bool operator==(const GraphicsPipelineStateDescription &o) const;
    size_t hash() const;

    agpu::shader_signature_ref shaderSignature;
    ShaderStageDescription vertexStage;
    ShaderStageDescription fragmentStage;
    ShaderStageDescription geometryStage;
    ShaderStageDescription tessellationControlStage;
    ShaderStageDescription tessellationEvaluationStage;

    // Color attachments
    static constexpr size_t MaxRenderTargetAttachmentCount = 16;
    size_t renderTargetColorAttachmentCount;
    std::array<RenderTargetColorAttachmentDescription, MaxRenderTargetAttachmentCount> renderTargetColorAttachments;

    // Depth stencil.
    agpu_texture_format depthStencilFormat;

    bool depthTestingEnabled;
    bool depthWriteMask;
    agpu_compare_function depthCompareFunction;

    bool depthBiasEnabled;
    agpu_float depthBiasConstantFactor;
    agpu_float depthBiasClamp;
    agpu_float depthBiasSlopeFactor;

    agpu_bool stencilTestingEnabled;
    agpu_int stencilWriteMask;
    agpu_int stencilReadMask;

    agpu_stencil_operation frontStencilFailOperation;
    agpu_stencil_operation frontStencilDepthFailOperation;
    agpu_stencil_operation frontStencilDepthPassOperation;
    agpu_compare_function frontStencilCompareFunction;

    agpu_stencil_operation backStencilFailOperation;
    agpu_stencil_operation backStencilDepthFailOperation;
    agpu_stencil_operation backStencilDepthPassOperation;
    agpu_compare_function backStencilCompareFunction;

    // Face culling
    agpu_face_winding frontFaceWinding;
    agpu_cull_mode faceCullingMode;

    // Rasterization
    agpu_polygon_mode polygonMode;
    agpu_primitive_topology primitiveType;
    agpu::vertex_layout_ref vertexLayout;
    agpu_uint sampleCount;
    agpu_uint sampleQuality;

    template<typename F>
    void renderTargetsMatchingMaskDo(uint32_t renderTargetMask, const F &f)
    {
        auto currentMask = renderTargetMask;
        for(size_t i = 0; i < MaxRenderTargetAttachmentCount && currentMask != 0; currentMask >>=1, ++i)
        {
            if((currentMask & 1) == 0)
                continue;

            f(renderTargetColorAttachments[i]);
        }
    }
};

/**
 * I am a description for a compute pipeline state.
 */
struct ComputePipelineStateDescription
{
    ComputePipelineStateDescription();
    ~ComputePipelineStateDescription();

    void reset();
    agpu_error applyToBuilder(const agpu::compute_pipeline_builder_ref &builder) const;

    bool operator==(const ComputePipelineStateDescription &o) const;
    size_t hash() const;

    agpu::shader_signature_ref shaderSignature;
    ShaderStageDescription computeStage;
};

}

namespace std
{
template<>
struct hash<AgpuCommon::GraphicsPipelineStateDescription>
{
    size_t operator()(const AgpuCommon::GraphicsPipelineStateDescription &ref) const
    {
        return ref.hash();
    }
};

template<>
struct hash<AgpuCommon::ComputePipelineStateDescription>
{
    size_t operator()(const AgpuCommon::ComputePipelineStateDescription &ref) const
    {
        return ref.hash();
    }
};

}

namespace AgpuCommon
{
class ImmediateShaderLibrary;

/**
 * I am a cache for the on the fly generated pipeline state objects that are
 * required by an state tracker.
 */
class StateTrackerCache : public agpu::state_tracker_cache
{
public:
    StateTrackerCache(const agpu::device_ref &device, uint32_t queueFamilyType);
    ~StateTrackerCache();

    static agpu::state_tracker_cache_ref create(const agpu::device_ref &device, uint32_t queueFamilyType);

    virtual agpu::state_tracker_ptr createStateTracker(agpu_command_list_type type, const agpu::command_queue_ref & command_queue) override;
	virtual agpu::state_tracker_ptr createStateTrackerWithCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, const agpu::command_allocator_ref & command_allocator) override;
	virtual agpu::state_tracker_ptr createStateTrackerWithFrameBuffering(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, agpu_uint framebuffering_count) override;
    virtual agpu::immediate_renderer_ptr createImmediateRenderer() override;

    agpu::pipeline_state_ref getComputePipelineWithDescription(const ComputePipelineStateDescription &description, std::string &pipelineBuildErrorLog);
    agpu::pipeline_state_ref getGraphicsPipelineWithDescription(const GraphicsPipelineStateDescription &description, std::string &pipelineBuildErrorLog);

    agpu::device_ref device;
    uint32_t queueFamilyType;

    bool ensureImmediateRendererObjectsExists();

    agpu::shader_signature_ref immediateShaderSignature;
    std::unique_ptr<ImmediateShaderLibrary> immediateShaderLibrary;
    agpu::vertex_layout_ref immediateVertexLayout;

private:
    std::mutex computePipelineStateCacheMutex;
    std::unordered_map<ComputePipelineStateDescription, agpu::pipeline_state_ref> computePipelineStateCache;

    std::mutex graphicsPipelineStateCacheMutex;
    std::unordered_map<GraphicsPipelineStateDescription, agpu::pipeline_state_ref> graphicsPipelineStateCache;

    std::mutex immediateRendererObjectsMutex;
    bool immediateRendererObjectsInitialized;

};

} // End of namespace AgpuCommon

#endif //AGPU_STATE_TRACKER_CACHE_HPP
