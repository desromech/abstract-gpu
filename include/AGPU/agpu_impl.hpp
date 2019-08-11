
#ifndef AGPU_HPP_
#define AGPU_HPP_

#include "agpu.h"
#include <stdexcept>
#include <memory>
#include <atomic>

namespace agpu
{

extern agpu_icd_dispatch cppRefcountedDispatchTable;

/**
 * Phanapi reference counter
 */
template <typename T>
class ref_counter
{
public:
    ref_counter(T *cobject)
        : dispatchTable(&cppRefcountedDispatchTable), object(cobject), strongCount(1), weakCount(0)
    {
        object->setRefCounterPointer(this);
    }

    agpu_error retain()
    {
        // Check once before doing the increase.
        if(strongCount == 0)
            return AGPU_INVALID_OPERATION;

        // Increase the referenece count.
        auto old = strongCount.fetch_add(1, std::memory_order_relaxed);

        // Check again, for concurrency reasons.
        if(old == 0)
            return AGPU_INVALID_OPERATION;

        return AGPU_OK;
    }

    agpu_error release()
    {
        // First sanity check.
        if(strongCount == 0)
            return AGPU_INVALID_OPERATION;

        // Decrease the strong count.
        auto old = strongCount.fetch_sub(1, std::memory_order_relaxed);

        // Check again, for concurrency reasons.
        if(old == 0)
            return AGPU_INVALID_OPERATION;

        // Should I delete the object?
        if(old == 1)
        {
            delete object;

            // Should I delete myself?
            if(weakCount == 0)
                delete this;
        }

        return AGPU_OK;
    }

    bool weakLock()
    {
        unsigned int oldCount;
        while((oldCount = strongCount.load()) != 0)
        {
            if(strongCount.compare_exchange_weak(oldCount, oldCount + 1))
                return true;
        }

        return false;
    }

    void weakRetain()
    {
    }

    void weakRelease()
    {
    }

    agpu_icd_dispatch *dispatchTable;
    T * object;
    std::atomic_uint strongCount;
    std::atomic_uint weakCount;
};

template<typename T>
class weak_ref;

/**
 * Phanapi strong reference
 */
template<typename T>
class ref
{
public:
    typedef ref_counter<T> Counter;
    typedef ref<T> StrongRef;
    typedef weak_ref<T> WeakRef;

private:
    Counter *counter;
    friend WeakRef;

public:
    ref() : counter(nullptr) {}

    ref(const StrongRef &other)
        : counter(nullptr)
    {
        *this = other;
    }

    explicit ref(Counter *theCounter)
        : counter(theCounter)
    {
    }

    ~ref()
    {
        if(counter)
            counter->release();
    }

    static StrongRef import(void *rawCounter)
    {
        auto castedCounter = reinterpret_cast<Counter*> (rawCounter);
        if(castedCounter)
            castedCounter->retain();
        return StrongRef(castedCounter);
    }

    StrongRef &operator=(const StrongRef &other)
    {
        auto newCounter = other.counter;
        if(newCounter)
            newCounter->retain();
        if(counter)
            counter->release();
        counter = newCounter;
        return *this;
    }

    void reset(Counter *newCounter = nullptr)
    {
        if(counter)
            counter->release();
        counter = newCounter;
    }

    Counter *disown()
    {
        Counter *result = counter;
        counter = nullptr;
        return result;
    }

    Counter *disownedNewRef() const
    {
        if(counter)
            counter->retain();
        return counter;
    }

    Counter *asPtrWithoutNewRef() const
    {
        return counter;
    }

    template<typename U>
    U *as() const
    {
        return static_cast<U*> (counter->object);
    }

    T *get() const
    {
        return counter ? counter->object : nullptr;
    }

    T *operator->() const
    {
        return counter->object;
    }

    operator bool() const
    {
        return counter != nullptr;
    }

    bool operator==(const StrongRef &other) const
    {
        return counter == other.counter;
    }

    bool operator<(const StrongRef &other) const
    {
        return counter < other.counter;
    }

    size_t hash() const
    {
        return std::hash<Counter*> () (counter);
    }
};

/**
 * Phanapi weak reference
 */
template<typename T>
class weak_ref
{
public:
    typedef ref_counter<T> Counter;
    typedef ref<T> StrongRef;
    typedef weak_ref<T> WeakRef;

private:
    Counter *counter;

public:
    weak_ref()
        : counter(nullptr) {}

    explicit weak_ref(const StrongRef &ref)
    {
        counter = ref.counter;
        if(counter)
            counter->weakRetain();
    }

    weak_ref(const WeakRef &ref)
    {
        counter = ref.counter;
        if(counter)
            counter->weakRetain();
    }

    ~weak_ref()
    {
        if(counter)
            counter->weakRelease();
    }

    WeakRef &operator=(const StrongRef &other)
    {
        auto newCounter = other.counter;
        if(newCounter)
            newCounter->weakRetain();
        if(counter)
            counter->weakRelease();
        counter = newCounter;
        return *this;
    }

    WeakRef &operator=(const WeakRef &other)
    {
        auto newCounter = other.counter;
        if(newCounter)
            newCounter->weakRetain();
        if(counter)
            counter->weakRelease();
        counter = newCounter;
        return *this;
    }

    StrongRef lock()
    {
        if(!counter)
            return StrongRef();

        return counter->weakLock() ? StrongRef(counter) : StrongRef();
    }

    bool operator==(const WeakRef &other) const
    {
        return counter == other.counter;
    }

    bool operator<(const WeakRef &other) const
    {
        return counter < other.counter;
    }

    size_t hash() const
    {
        return std::hash<Counter*> () (counter);
    }
};

template<typename I, typename T, typename...Args>
inline ref<I> makeObjectWithInterface(Args... args)
{
    std::unique_ptr<T> object(new T(args...));
    std::unique_ptr<ref_counter<I>> counter(new ref_counter<I> (object.release()));
    return ref<I> (counter.release());
}

template<typename T, typename...Args>
inline ref<typename T::main_interface> makeObject(Args... args)
{
   return makeObjectWithInterface<typename T::main_interface, T> (args...);
}

/**
 * Phanapi base interface
 */
class base_interface
{
public:
    virtual ~base_interface() {}

    void setRefCounterPointer(void *newPointer)
    {
        myRefCounter = newPointer;
    }

    template<typename T=base_interface>
    const ref<T> &refFromThis()
    {
        return reinterpret_cast<const ref<T> &> (myRefCounter);
    }

private:
    void *myRefCounter;
};

} // End of namespace agpu

namespace std
{
template<typename T>
struct hash<agpu::ref<T>>
{
    size_t operator()(const agpu::ref<T> &ref) const
    {
        return ref.hash();
    }
};

template<typename T>
struct hash<agpu::weak_ref<T>>
{
    size_t operator()(const agpu::ref<T> &ref) const
    {
        return ref.hash();
    }
};

}

namespace agpu
{
struct platform;
typedef ref_counter<platform> *platform_ptr;
typedef ref<platform> platform_ref;
typedef weak_ref<platform> platform_weakref;

struct device;
typedef ref_counter<device> *device_ptr;
typedef ref<device> device_ref;
typedef weak_ref<device> device_weakref;

struct vr_system;
typedef ref_counter<vr_system> *vr_system_ptr;
typedef ref<vr_system> vr_system_ref;
typedef weak_ref<vr_system> vr_system_weakref;

struct swap_chain;
typedef ref_counter<swap_chain> *swap_chain_ptr;
typedef ref<swap_chain> swap_chain_ref;
typedef weak_ref<swap_chain> swap_chain_weakref;

struct compute_pipeline_builder;
typedef ref_counter<compute_pipeline_builder> *compute_pipeline_builder_ptr;
typedef ref<compute_pipeline_builder> compute_pipeline_builder_ref;
typedef weak_ref<compute_pipeline_builder> compute_pipeline_builder_weakref;

struct pipeline_builder;
typedef ref_counter<pipeline_builder> *pipeline_builder_ptr;
typedef ref<pipeline_builder> pipeline_builder_ref;
typedef weak_ref<pipeline_builder> pipeline_builder_weakref;

struct pipeline_state;
typedef ref_counter<pipeline_state> *pipeline_state_ptr;
typedef ref<pipeline_state> pipeline_state_ref;
typedef weak_ref<pipeline_state> pipeline_state_weakref;

struct command_queue;
typedef ref_counter<command_queue> *command_queue_ptr;
typedef ref<command_queue> command_queue_ref;
typedef weak_ref<command_queue> command_queue_weakref;

struct command_allocator;
typedef ref_counter<command_allocator> *command_allocator_ptr;
typedef ref<command_allocator> command_allocator_ref;
typedef weak_ref<command_allocator> command_allocator_weakref;

struct command_list;
typedef ref_counter<command_list> *command_list_ptr;
typedef ref<command_list> command_list_ref;
typedef weak_ref<command_list> command_list_weakref;

struct texture;
typedef ref_counter<texture> *texture_ptr;
typedef ref<texture> texture_ref;
typedef weak_ref<texture> texture_weakref;

struct texture_view;
typedef ref_counter<texture_view> *texture_view_ptr;
typedef ref<texture_view> texture_view_ref;
typedef weak_ref<texture_view> texture_view_weakref;

struct sampler;
typedef ref_counter<sampler> *sampler_ptr;
typedef ref<sampler> sampler_ref;
typedef weak_ref<sampler> sampler_weakref;

struct buffer;
typedef ref_counter<buffer> *buffer_ptr;
typedef ref<buffer> buffer_ref;
typedef weak_ref<buffer> buffer_weakref;

struct vertex_binding;
typedef ref_counter<vertex_binding> *vertex_binding_ptr;
typedef ref<vertex_binding> vertex_binding_ref;
typedef weak_ref<vertex_binding> vertex_binding_weakref;

struct vertex_layout;
typedef ref_counter<vertex_layout> *vertex_layout_ptr;
typedef ref<vertex_layout> vertex_layout_ref;
typedef weak_ref<vertex_layout> vertex_layout_weakref;

struct shader;
typedef ref_counter<shader> *shader_ptr;
typedef ref<shader> shader_ref;
typedef weak_ref<shader> shader_weakref;

struct framebuffer;
typedef ref_counter<framebuffer> *framebuffer_ptr;
typedef ref<framebuffer> framebuffer_ref;
typedef weak_ref<framebuffer> framebuffer_weakref;

struct renderpass;
typedef ref_counter<renderpass> *renderpass_ptr;
typedef ref<renderpass> renderpass_ref;
typedef weak_ref<renderpass> renderpass_weakref;

struct shader_signature_builder;
typedef ref_counter<shader_signature_builder> *shader_signature_builder_ptr;
typedef ref<shader_signature_builder> shader_signature_builder_ref;
typedef weak_ref<shader_signature_builder> shader_signature_builder_weakref;

struct shader_signature;
typedef ref_counter<shader_signature> *shader_signature_ptr;
typedef ref<shader_signature> shader_signature_ref;
typedef weak_ref<shader_signature> shader_signature_weakref;

struct shader_resource_binding;
typedef ref_counter<shader_resource_binding> *shader_resource_binding_ptr;
typedef ref<shader_resource_binding> shader_resource_binding_ref;
typedef weak_ref<shader_resource_binding> shader_resource_binding_weakref;

struct fence;
typedef ref_counter<fence> *fence_ptr;
typedef ref<fence> fence_ref;
typedef weak_ref<fence> fence_weakref;

struct offline_shader_compiler;
typedef ref_counter<offline_shader_compiler> *offline_shader_compiler_ptr;
typedef ref<offline_shader_compiler> offline_shader_compiler_ref;
typedef weak_ref<offline_shader_compiler> offline_shader_compiler_weakref;

struct state_tracker_cache;
typedef ref_counter<state_tracker_cache> *state_tracker_cache_ptr;
typedef ref<state_tracker_cache> state_tracker_cache_ref;
typedef weak_ref<state_tracker_cache> state_tracker_cache_weakref;

struct state_tracker;
typedef ref_counter<state_tracker> *state_tracker_ptr;
typedef ref<state_tracker> state_tracker_ref;
typedef weak_ref<state_tracker> state_tracker_weakref;

struct immediate_renderer;
typedef ref_counter<immediate_renderer> *immediate_renderer_ptr;
typedef ref<immediate_renderer> immediate_renderer_ref;
typedef weak_ref<immediate_renderer> immediate_renderer_weakref;

// Interface wrapper for agpu_platform.
struct platform : base_interface
{
public:
	typedef platform main_interface;
	virtual device_ptr openDevice(agpu_device_open_info* openInfo) = 0;
	virtual agpu_cstring getName() = 0;
	virtual agpu_int getVersion() = 0;
	virtual agpu_int getImplementationVersion() = 0;
	virtual agpu_bool hasRealMultithreading() = 0;
	virtual agpu_bool isNative() = 0;
	virtual agpu_bool isCrossPlatform() = 0;
	virtual offline_shader_compiler_ptr createOfflineShaderCompiler() = 0;
};


// Interface wrapper for agpu_device.
struct device : base_interface
{
public:
	typedef device main_interface;
	virtual command_queue_ptr getDefaultCommandQueue() = 0;
	virtual swap_chain_ptr createSwapChain(const command_queue_ref & commandQueue, agpu_swap_chain_create_info* swapChainInfo) = 0;
	virtual buffer_ptr createBuffer(agpu_buffer_description* description, agpu_pointer initial_data) = 0;
	virtual vertex_layout_ptr createVertexLayout() = 0;
	virtual vertex_binding_ptr createVertexBinding(const vertex_layout_ref & layout) = 0;
	virtual shader_ptr createShader(agpu_shader_type type) = 0;
	virtual shader_signature_builder_ptr createShaderSignatureBuilder() = 0;
	virtual pipeline_builder_ptr createPipelineBuilder() = 0;
	virtual compute_pipeline_builder_ptr createComputePipelineBuilder() = 0;
	virtual command_allocator_ptr createCommandAllocator(agpu_command_list_type type, const command_queue_ref & queue) = 0;
	virtual command_list_ptr createCommandList(agpu_command_list_type type, const command_allocator_ref & allocator, const pipeline_state_ref & initial_pipeline_state) = 0;
	virtual agpu_shader_language getPreferredShaderLanguage() = 0;
	virtual agpu_shader_language getPreferredIntermediateShaderLanguage() = 0;
	virtual agpu_shader_language getPreferredHighLevelShaderLanguage() = 0;
	virtual framebuffer_ptr createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, texture_view_ref* colorViews, const texture_view_ref & depthStencilView) = 0;
	virtual renderpass_ptr createRenderPass(agpu_renderpass_description* description) = 0;
	virtual texture_ptr createTexture(agpu_texture_description* description) = 0;
	virtual sampler_ptr createSampler(agpu_sampler_description* description) = 0;
	virtual fence_ptr createFence() = 0;
	virtual agpu_int getMultiSampleQualityLevels(agpu_uint sample_count) = 0;
	virtual agpu_bool hasTopLeftNdcOrigin() = 0;
	virtual agpu_bool hasBottomLeftTextureCoordinates() = 0;
	virtual agpu_bool isFeatureSupported(agpu_feature feature) = 0;
	virtual vr_system_ptr getVRSystem() = 0;
	virtual offline_shader_compiler_ptr createOfflineShaderCompiler() = 0;
	virtual state_tracker_cache_ptr createStateTrackerCache(const command_queue_ref & command_queue_family) = 0;
};


// Interface wrapper for agpu_vr_system.
struct vr_system : base_interface
{
public:
	typedef vr_system main_interface;
	virtual agpu_cstring getVRSystemName() = 0;
	virtual agpu_pointer getNativeHandle() = 0;
	virtual agpu_error getRecommendedRenderTargetSize(agpu_size2d* size) = 0;
	virtual agpu_error getEyeToHeadTransform(agpu_vr_eye eye, agpu_matrix4x4f* transform) = 0;
	virtual agpu_error getProjectionMatrix(agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix) = 0;
	virtual agpu_error getProjectionFrustumTangents(agpu_vr_eye eye, agpu_frustum_tangents* frustum) = 0;
	virtual agpu_error submitEyeRenderTargets(const texture_ref & left_eye, const texture_ref & right_eye) = 0;
	virtual agpu_error waitAndFetchPoses() = 0;
	virtual agpu_size getValidTrackedDevicePoseCount() = 0;
	virtual agpu_error getValidTrackedDevicePoseInto(agpu_size index, agpu_vr_tracked_device_pose* dest) = 0;
	virtual agpu_size getValidRenderTrackedDevicePoseCount() = 0;
	virtual agpu_error getValidRenderTrackedDevicePoseInto(agpu_size index, agpu_vr_tracked_device_pose* dest) = 0;
	virtual agpu_bool pollEvent(agpu_vr_event* event) = 0;
};


// Interface wrapper for agpu_swap_chain.
struct swap_chain : base_interface
{
public:
	typedef swap_chain main_interface;
	virtual agpu_error swapBuffers() = 0;
	virtual framebuffer_ptr getCurrentBackBuffer() = 0;
	virtual agpu_size getCurrentBackBufferIndex() = 0;
	virtual agpu_size getFramebufferCount() = 0;
};


// Interface wrapper for agpu_compute_pipeline_builder.
struct compute_pipeline_builder : base_interface
{
public:
	typedef compute_pipeline_builder main_interface;
	virtual pipeline_state_ptr build() = 0;
	virtual agpu_error attachShader(const shader_ref & shader) = 0;
	virtual agpu_error attachShaderWithEntryPoint(const shader_ref & shader, agpu_shader_type type, agpu_cstring entry_point) = 0;
	virtual agpu_size getBuildingLogLength() = 0;
	virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) = 0;
	virtual agpu_error setShaderSignature(const shader_signature_ref & signature) = 0;
};


// Interface wrapper for agpu_pipeline_builder.
struct pipeline_builder : base_interface
{
public:
	typedef pipeline_builder main_interface;
	virtual pipeline_state_ptr build() = 0;
	virtual agpu_error attachShader(const shader_ref & shader) = 0;
	virtual agpu_error attachShaderWithEntryPoint(const shader_ref & shader, agpu_shader_type type, agpu_cstring entry_point) = 0;
	virtual agpu_size getBuildingLogLength() = 0;
	virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) = 0;
	virtual agpu_error setBlendState(agpu_int renderTargetMask, agpu_bool enabled) = 0;
	virtual agpu_error setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation) = 0;
	virtual agpu_error setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled) = 0;
	virtual agpu_error setFrontFace(agpu_face_winding winding) = 0;
	virtual agpu_error setCullMode(agpu_cull_mode mode) = 0;
	virtual agpu_error setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor) = 0;
	virtual agpu_error setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function) = 0;
	virtual agpu_error setPolygonMode(agpu_polygon_mode mode) = 0;
	virtual agpu_error setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask) = 0;
	virtual agpu_error setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) = 0;
	virtual agpu_error setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) = 0;
	virtual agpu_error setRenderTargetCount(agpu_int count) = 0;
	virtual agpu_error setRenderTargetFormat(agpu_uint index, agpu_texture_format format) = 0;
	virtual agpu_error setDepthStencilFormat(agpu_texture_format format) = 0;
	virtual agpu_error setPrimitiveType(agpu_primitive_topology type) = 0;
	virtual agpu_error setVertexLayout(const vertex_layout_ref & layout) = 0;
	virtual agpu_error setShaderSignature(const shader_signature_ref & signature) = 0;
	virtual agpu_error setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality) = 0;
};


// Interface wrapper for agpu_pipeline_state.
struct pipeline_state : base_interface
{
public:
	typedef pipeline_state main_interface;
};


// Interface wrapper for agpu_command_queue.
struct command_queue : base_interface
{
public:
	typedef command_queue main_interface;
	virtual agpu_error addCommandList(const command_list_ref & command_list) = 0;
	virtual agpu_error finishExecution() = 0;
	virtual agpu_error signalFence(const fence_ref & fence) = 0;
	virtual agpu_error waitFence(const fence_ref & fence) = 0;
};


// Interface wrapper for agpu_command_allocator.
struct command_allocator : base_interface
{
public:
	typedef command_allocator main_interface;
	virtual agpu_error reset() = 0;
};


// Interface wrapper for agpu_command_list.
struct command_list : base_interface
{
public:
	typedef command_list main_interface;
	virtual agpu_error setShaderSignature(const shader_signature_ref & signature) = 0;
	virtual agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h) = 0;
	virtual agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h) = 0;
	virtual agpu_error usePipelineState(const pipeline_state_ref & pipeline) = 0;
	virtual agpu_error useVertexBinding(const vertex_binding_ref & vertex_binding) = 0;
	virtual agpu_error useIndexBuffer(const buffer_ref & index_buffer) = 0;
	virtual agpu_error useIndexBufferAt(const buffer_ref & index_buffer, agpu_size offset, agpu_size index_size) = 0;
	virtual agpu_error useDrawIndirectBuffer(const buffer_ref & draw_buffer) = 0;
	virtual agpu_error useComputeDispatchIndirectBuffer(const buffer_ref & buffer) = 0;
	virtual agpu_error useShaderResources(const shader_resource_binding_ref & binding) = 0;
	virtual agpu_error useComputeShaderResources(const shader_resource_binding_ref & binding) = 0;
	virtual agpu_error drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance) = 0;
	virtual agpu_error drawArraysIndirect(agpu_size offset, agpu_size drawcount) = 0;
	virtual agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) = 0;
	virtual agpu_error drawElementsIndirect(agpu_size offset, agpu_size drawcount) = 0;
	virtual agpu_error dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z) = 0;
	virtual agpu_error dispatchComputeIndirect(agpu_size offset) = 0;
	virtual agpu_error setStencilReference(agpu_uint reference) = 0;
	virtual agpu_error executeBundle(const command_list_ref & bundle) = 0;
	virtual agpu_error close() = 0;
	virtual agpu_error reset(const command_allocator_ref & allocator, const pipeline_state_ref & initial_pipeline_state) = 0;
	virtual agpu_error resetBundle(const command_allocator_ref & allocator, const pipeline_state_ref & initial_pipeline_state, agpu_inheritance_info* inheritance_info) = 0;
	virtual agpu_error beginRenderPass(const renderpass_ref & renderpass, const framebuffer_ref & framebuffer, agpu_bool bundle_content) = 0;
	virtual agpu_error endRenderPass() = 0;
	virtual agpu_error resolveFramebuffer(const framebuffer_ref & destFramebuffer, const framebuffer_ref & sourceFramebuffer) = 0;
	virtual agpu_error resolveTexture(const texture_ref & sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect) = 0;
	virtual agpu_error pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values) = 0;
	virtual agpu_error memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses) = 0;
};


// Interface wrapper for agpu_texture.
struct texture : base_interface
{
public:
	typedef texture main_interface;
	virtual agpu_error getDescription(agpu_texture_description* description) = 0;
	virtual agpu_pointer mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region) = 0;
	virtual agpu_error unmapLevel() = 0;
	virtual agpu_error readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer) = 0;
	virtual agpu_error uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data) = 0;
	virtual agpu_error uploadTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data) = 0;
	virtual agpu_error discardUploadBuffer() = 0;
	virtual agpu_error discardReadbackBuffer() = 0;
	virtual agpu_error getFullViewDescription(agpu_texture_view_description* result) = 0;
	virtual texture_view_ptr createView(agpu_texture_view_description* description) = 0;
	virtual texture_view_ptr getOrCreateFullView() = 0;
};


// Interface wrapper for agpu_texture_view.
struct texture_view : base_interface
{
public:
	typedef texture_view main_interface;
	virtual texture_ptr getTexture() = 0;
};


// Interface wrapper for agpu_sampler.
struct sampler : base_interface
{
public:
	typedef sampler main_interface;
};


// Interface wrapper for agpu_buffer.
struct buffer : base_interface
{
public:
	typedef buffer main_interface;
	virtual agpu_pointer mapBuffer(agpu_mapping_access flags) = 0;
	virtual agpu_error unmapBuffer() = 0;
	virtual agpu_error getDescription(agpu_buffer_description* description) = 0;
	virtual agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data) = 0;
	virtual agpu_error readBufferData(agpu_size offset, agpu_size size, agpu_pointer data) = 0;
	virtual agpu_error flushWholeBuffer() = 0;
	virtual agpu_error invalidateWholeBuffer() = 0;
};


// Interface wrapper for agpu_vertex_binding.
struct vertex_binding : base_interface
{
public:
	typedef vertex_binding main_interface;
	virtual agpu_error bindVertexBuffers(agpu_uint count, buffer_ref* vertex_buffers) = 0;
	virtual agpu_error bindVertexBuffersWithOffsets(agpu_uint count, buffer_ref* vertex_buffers, agpu_size* offsets) = 0;
};


// Interface wrapper for agpu_vertex_layout.
struct vertex_layout : base_interface
{
public:
	typedef vertex_layout main_interface;
	virtual agpu_error addVertexAttributeBindings(agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes) = 0;
};


// Interface wrapper for agpu_shader.
struct shader : base_interface
{
public:
	typedef shader main_interface;
	virtual agpu_error setShaderSource(agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength) = 0;
	virtual agpu_error compileShader(agpu_cstring options) = 0;
	virtual agpu_size getCompilationLogLength() = 0;
	virtual agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer) = 0;
};


// Interface wrapper for agpu_framebuffer.
struct framebuffer : base_interface
{
public:
	typedef framebuffer main_interface;
};


// Interface wrapper for agpu_renderpass.
struct renderpass : base_interface
{
public:
	typedef renderpass main_interface;
	virtual agpu_error setDepthStencilClearValue(agpu_depth_stencil_value value) = 0;
	virtual agpu_error setColorClearValue(agpu_uint attachment_index, agpu_color4f value) = 0;
	virtual agpu_error setColorClearValueFrom(agpu_uint attachment_index, agpu_color4f* value) = 0;
	virtual agpu_error getColorAttachmentFormats(agpu_uint* color_attachment_count, agpu_texture_format* formats) = 0;
	virtual agpu_texture_format getDepthStencilAttachmentFormat() = 0;
};


// Interface wrapper for agpu_shader_signature_builder.
struct shader_signature_builder : base_interface
{
public:
	typedef shader_signature_builder main_interface;
	virtual shader_signature_ptr build() = 0;
	virtual agpu_error addBindingConstant() = 0;
	virtual agpu_error addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings) = 0;
	virtual agpu_error beginBindingBank(agpu_uint maxBindings) = 0;
	virtual agpu_error addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount) = 0;
};


// Interface wrapper for agpu_shader_signature.
struct shader_signature : base_interface
{
public:
	typedef shader_signature main_interface;
	virtual shader_resource_binding_ptr createShaderResourceBinding(agpu_uint element) = 0;
};


// Interface wrapper for agpu_shader_resource_binding.
struct shader_resource_binding : base_interface
{
public:
	typedef shader_resource_binding main_interface;
	virtual agpu_error bindUniformBuffer(agpu_int location, const buffer_ref & uniform_buffer) = 0;
	virtual agpu_error bindUniformBufferRange(agpu_int location, const buffer_ref & uniform_buffer, agpu_size offset, agpu_size size) = 0;
	virtual agpu_error bindStorageBuffer(agpu_int location, const buffer_ref & storage_buffer) = 0;
	virtual agpu_error bindStorageBufferRange(agpu_int location, const buffer_ref & storage_buffer, agpu_size offset, agpu_size size) = 0;
	virtual agpu_error bindSampledTextureView(agpu_int location, const texture_view_ref & view) = 0;
	virtual agpu_error bindStorageImageView(agpu_int location, const texture_view_ref & view) = 0;
	virtual agpu_error bindSampler(agpu_int location, const sampler_ref & sampler) = 0;
};


// Interface wrapper for agpu_fence.
struct fence : base_interface
{
public:
	typedef fence main_interface;
	virtual agpu_error waitOnClient() = 0;
};


// Interface wrapper for agpu_offline_shader_compiler.
struct offline_shader_compiler : base_interface
{
public:
	typedef offline_shader_compiler main_interface;
	virtual agpu_bool isShaderLanguageSupported(agpu_shader_language language) = 0;
	virtual agpu_bool isTargetShaderLanguageSupported(agpu_shader_language language) = 0;
	virtual agpu_error setShaderSource(agpu_shader_language language, agpu_shader_type stage, agpu_string sourceText, agpu_string_length sourceTextLength) = 0;
	virtual agpu_error compileShader(agpu_shader_language target_language, agpu_cstring options) = 0;
	virtual agpu_size getCompilationLogLength() = 0;
	virtual agpu_error getCompilationLog(agpu_size buffer_size, agpu_string_buffer buffer) = 0;
	virtual agpu_size getCompilationResultLength() = 0;
	virtual agpu_error getCompilationResult(agpu_size buffer_size, agpu_string_buffer buffer) = 0;
	virtual shader_ptr getResultAsShader() = 0;
};


// Interface wrapper for agpu_state_tracker_cache.
struct state_tracker_cache : base_interface
{
public:
	typedef state_tracker_cache main_interface;
	virtual state_tracker_ptr createStateTracker(agpu_command_list_type type, const command_queue_ref & command_queue) = 0;
	virtual state_tracker_ptr createStateTrackerWithCommandAllocator(agpu_command_list_type type, const command_queue_ref & command_queue, const command_allocator_ref & command_allocator) = 0;
	virtual state_tracker_ptr createStateTrackerWithFrameBuffering(agpu_command_list_type type, const command_queue_ref & command_queue, agpu_uint framebuffering_count) = 0;
	virtual immediate_renderer_ptr createImmediateRenderer() = 0;
};


// Interface wrapper for agpu_state_tracker.
struct state_tracker : base_interface
{
public:
	typedef state_tracker main_interface;
	virtual agpu_error beginRecordingCommands() = 0;
	virtual command_list_ptr endRecordingCommands() = 0;
	virtual agpu_error endRecordingAndFlushCommands() = 0;
	virtual agpu_error reset() = 0;
	virtual agpu_error resetGraphicsPipeline() = 0;
	virtual agpu_error resetComputePipeline() = 0;
	virtual agpu_error setComputeStage(const shader_ref & shader, agpu_cstring entryPoint) = 0;
	virtual agpu_error setVertexStage(const shader_ref & shader, agpu_cstring entryPoint) = 0;
	virtual agpu_error setFragmentStage(const shader_ref & shader, agpu_cstring entryPoint) = 0;
	virtual agpu_error setGeometryStage(const shader_ref & shader, agpu_cstring entryPoint) = 0;
	virtual agpu_error setTessellationControlStage(const shader_ref & shader, agpu_cstring entryPoint) = 0;
	virtual agpu_error setTessellationEvaluationStage(const shader_ref & shader, agpu_cstring entryPoint) = 0;
	virtual agpu_error setBlendState(agpu_int renderTargetMask, agpu_bool enabled) = 0;
	virtual agpu_error setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation) = 0;
	virtual agpu_error setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled) = 0;
	virtual agpu_error setFrontFace(agpu_face_winding winding) = 0;
	virtual agpu_error setCullMode(agpu_cull_mode mode) = 0;
	virtual agpu_error setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor) = 0;
	virtual agpu_error setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function) = 0;
	virtual agpu_error setPolygonMode(agpu_polygon_mode mode) = 0;
	virtual agpu_error setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask) = 0;
	virtual agpu_error setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) = 0;
	virtual agpu_error setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) = 0;
	virtual agpu_error setPrimitiveType(agpu_primitive_topology type) = 0;
	virtual agpu_error setVertexLayout(const vertex_layout_ref & layout) = 0;
	virtual agpu_error setShaderSignature(const shader_signature_ref & signature) = 0;
	virtual agpu_error setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality) = 0;
	virtual agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h) = 0;
	virtual agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h) = 0;
	virtual agpu_error useVertexBinding(const vertex_binding_ref & vertex_binding) = 0;
	virtual agpu_error useIndexBuffer(const buffer_ref & index_buffer) = 0;
	virtual agpu_error useIndexBufferAt(const buffer_ref & index_buffer, agpu_size offset, agpu_size index_size) = 0;
	virtual agpu_error useDrawIndirectBuffer(const buffer_ref & draw_buffer) = 0;
	virtual agpu_error useComputeDispatchIndirectBuffer(const buffer_ref & buffer) = 0;
	virtual agpu_error useShaderResources(const shader_resource_binding_ref & binding) = 0;
	virtual agpu_error useComputeShaderResources(const shader_resource_binding_ref & binding) = 0;
	virtual agpu_error drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance) = 0;
	virtual agpu_error drawArraysIndirect(agpu_size offset, agpu_size drawcount) = 0;
	virtual agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) = 0;
	virtual agpu_error drawElementsIndirect(agpu_size offset, agpu_size drawcount) = 0;
	virtual agpu_error dispatchCompute(agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z) = 0;
	virtual agpu_error dispatchComputeIndirect(agpu_size offset) = 0;
	virtual agpu_error setStencilReference(agpu_uint reference) = 0;
	virtual agpu_error executeBundle(const command_list_ref & bundle) = 0;
	virtual agpu_error beginRenderPass(const renderpass_ref & renderpass, const framebuffer_ref & framebuffer, agpu_bool bundle_content) = 0;
	virtual agpu_error endRenderPass() = 0;
	virtual agpu_error resolveFramebuffer(const framebuffer_ref & destFramebuffer, const framebuffer_ref & sourceFramebuffer) = 0;
	virtual agpu_error resolveTexture(const texture_ref & sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, const texture_ref & destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect) = 0;
	virtual agpu_error pushConstants(agpu_uint offset, agpu_uint size, agpu_pointer values) = 0;
	virtual agpu_error memoryBarrier(agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses) = 0;
};


// Interface wrapper for agpu_immediate_renderer.
struct immediate_renderer : base_interface
{
public:
	typedef immediate_renderer main_interface;
	virtual agpu_error beginRendering(const state_tracker_ref & state_tracker) = 0;
	virtual agpu_error endRendering() = 0;
	virtual agpu_error setBlendState(agpu_int renderTargetMask, agpu_bool enabled) = 0;
	virtual agpu_error setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation) = 0;
	virtual agpu_error setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled) = 0;
	virtual agpu_error setFrontFace(agpu_face_winding winding) = 0;
	virtual agpu_error setCullMode(agpu_cull_mode mode) = 0;
	virtual agpu_error setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor) = 0;
	virtual agpu_error setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function) = 0;
	virtual agpu_error setPolygonMode(agpu_polygon_mode mode) = 0;
	virtual agpu_error setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask) = 0;
	virtual agpu_error setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) = 0;
	virtual agpu_error setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) = 0;
	virtual agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h) = 0;
	virtual agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h) = 0;
	virtual agpu_error setStencilReference(agpu_uint reference) = 0;
	virtual agpu_error projectionMatrixMode() = 0;
	virtual agpu_error modelViewMatrixMode() = 0;
	virtual agpu_error textureMatrixMode() = 0;
	virtual agpu_error loadIdentity() = 0;
	virtual agpu_error pushMatrix() = 0;
	virtual agpu_error popMatrix() = 0;
	virtual agpu_error loadMatrix(agpu_float* elements) = 0;
	virtual agpu_error loadTransposeMatrix(agpu_float* elements) = 0;
	virtual agpu_error multiplyMatrix(agpu_float* elements) = 0;
	virtual agpu_error multiplyTransposeMatrix(agpu_float* elements) = 0;
	virtual agpu_error ortho(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far) = 0;
	virtual agpu_error frustum(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far) = 0;
	virtual agpu_error perspective(agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far) = 0;
	virtual agpu_error rotate(agpu_float angle, agpu_float x, agpu_float y, agpu_float z) = 0;
	virtual agpu_error translate(agpu_float x, agpu_float y, agpu_float z) = 0;
	virtual agpu_error scale(agpu_float x, agpu_float y, agpu_float z) = 0;
	virtual agpu_error setFlatShading(agpu_bool enabled) = 0;
	virtual agpu_error setLightingEnabled(agpu_bool enabled) = 0;
	virtual agpu_error clearLights() = 0;
	virtual agpu_error setAmbientLighting(agpu_float r, agpu_float g, agpu_float b, agpu_float a) = 0;
	virtual agpu_error setLight(agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state) = 0;
	virtual agpu_error setTexturingEnabled(agpu_bool enabled) = 0;
	virtual agpu_error bindTexture(const texture_ref & texture) = 0;
	virtual agpu_error beginPrimitives(agpu_primitive_topology type) = 0;
	virtual agpu_error endPrimitives() = 0;
	virtual agpu_error color(agpu_float r, agpu_float g, agpu_float b, agpu_float a) = 0;
	virtual agpu_error texcoord(agpu_float x, agpu_float y) = 0;
	virtual agpu_error normal(agpu_float x, agpu_float y, agpu_float z) = 0;
	virtual agpu_error vertex(agpu_float x, agpu_float y, agpu_float z) = 0;
	virtual agpu_error beginMeshWithVertices(agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer vertices) = 0;
	virtual agpu_error setCurrentMeshColors(agpu_size stride, agpu_size elementCount, agpu_pointer colors) = 0;
	virtual agpu_error setCurrentMeshNormals(agpu_size stride, agpu_size elementCount, agpu_pointer normals) = 0;
	virtual agpu_error setCurrentMeshTexCoords(agpu_size stride, agpu_size elementCount, agpu_pointer texcoords) = 0;
	virtual agpu_error drawElementsWithIndices(agpu_primitive_topology mode, agpu_pointer indices, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) = 0;
	virtual agpu_error endMesh() = 0;
};


} // End of agpu

#endif /* AGPU_HPP_ */
