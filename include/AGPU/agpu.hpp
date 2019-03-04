
#ifndef AGPU_HPP_
#define AGPU_HPP_

#include <stdexcept>
#include "agpu.h"

/**
 * Abstract GPU exception.
 */
class agpu_exception : public std::runtime_error
{
public:
    explicit agpu_exception(agpu_error error)
        : std::runtime_error("Abstract GPU Error"), errorCode(error)
    {
    }

    agpu_error getErrorCode() const
    {
        return errorCode;
    }

private:
    agpu_error errorCode;
};

/**
 * Abstract GPU reference smart pointer.
 */
template<typename T>
class agpu_ref
{
public:
    agpu_ref()
        : pointer(0)
    {
    }

    agpu_ref(const agpu_ref<T> &other)
    {
        if(other.pointer)
            other.pointer->addReference();
        pointer = other.pointer;
    }

    agpu_ref(T* pointer)
        : pointer(pointer)
    {
    }

    ~agpu_ref()
    {
        if (pointer)
            pointer->release();
    }

    agpu_ref<T> &operator=(T *newPointer)
    {
        if (pointer)
            pointer->release();
        pointer = newPointer;
        return *this;
    }

    agpu_ref<T> &operator=(const agpu_ref<T> &other)
    {
        if(pointer != other.pointer)
        {
            if(other.pointer)
                other.pointer->addReference();
            if(pointer)
                pointer->release();
            pointer = other.pointer;
        }
        return *this;
    }

    operator bool() const
    {
        return pointer;
    }

    bool operator!() const
    {
        return !pointer;
    }

    T* get() const
    {
        return pointer;
    }

    T *operator->() const
    {
        return pointer;
    }

private:
    T *pointer;
};

/**
 * Helper function to convert an error code into an exception.
 */
inline void agpuThrowIfFailed(agpu_error error)
{
    if(error < 0)
        throw agpu_exception(error);
}

// Interface wrapper for agpu_platform.
struct _agpu_platform
{
private:
	_agpu_platform() {}

public:
	inline agpu_device* openDevice ( agpu_device_open_info* openInfo )
	{
		return agpuOpenDevice( this, openInfo );
	}

	inline agpu_cstring getName (  )
	{
		return agpuGetPlatformName( this );
	}

	inline agpu_int getVersion (  )
	{
		return agpuGetPlatformVersion( this );
	}

	inline agpu_int getImplementationVersion (  )
	{
		return agpuGetPlatformImplementationVersion( this );
	}

	inline agpu_bool hasRealMultithreading (  )
	{
		return agpuPlatformHasRealMultithreading( this );
	}

	inline agpu_bool isNative (  )
	{
		return agpuIsNativePlatform( this );
	}

	inline agpu_bool isCrossPlatform (  )
	{
		return agpuIsCrossPlatform( this );
	}

};

typedef agpu_ref<agpu_platform> agpu_platform_ref;

// Interface wrapper for agpu_device.
struct _agpu_device
{
private:
	_agpu_device() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddDeviceReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseDevice( this ));
	}

	inline agpu_command_queue* getDefaultCommandQueue (  )
	{
		return agpuGetDefaultCommandQueue( this );
	}

	inline agpu_swap_chain* createSwapChain ( agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo )
	{
		return agpuCreateSwapChain( this, commandQueue, swapChainInfo );
	}

	inline agpu_buffer* createBuffer ( agpu_buffer_description* description, agpu_pointer initial_data )
	{
		return agpuCreateBuffer( this, description, initial_data );
	}

	inline agpu_vertex_layout* createVertexLayout (  )
	{
		return agpuCreateVertexLayout( this );
	}

	inline agpu_vertex_binding* createVertexBinding ( agpu_vertex_layout* layout )
	{
		return agpuCreateVertexBinding( this, layout );
	}

	inline agpu_shader* createShader ( agpu_shader_type type )
	{
		return agpuCreateShader( this, type );
	}

	inline agpu_shader_signature_builder* createShaderSignatureBuilder (  )
	{
		return agpuCreateShaderSignatureBuilder( this );
	}

	inline agpu_pipeline_builder* createPipelineBuilder (  )
	{
		return agpuCreatePipelineBuilder( this );
	}

	inline agpu_compute_pipeline_builder* createComputePipelineBuilder (  )
	{
		return agpuCreateComputePipelineBuilder( this );
	}

	inline agpu_command_allocator* createCommandAllocator ( agpu_command_list_type type, agpu_command_queue* queue )
	{
		return agpuCreateCommandAllocator( this, type, queue );
	}

	inline agpu_command_list* createCommandList ( agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
	{
		return agpuCreateCommandList( this, type, allocator, initial_pipeline_state );
	}

	inline agpu_shader_language getPreferredShaderLanguage (  )
	{
		return agpuGetPreferredShaderLanguage( this );
	}

	inline agpu_shader_language getPreferredIntermediateShaderLanguage (  )
	{
		return agpuGetPreferredIntermediateShaderLanguage( this );
	}

	inline agpu_shader_language getPreferredHighLevelShaderLanguage (  )
	{
		return agpuGetPreferredHighLevelShaderLanguage( this );
	}

	inline agpu_framebuffer* createFrameBuffer ( agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView )
	{
		return agpuCreateFrameBuffer( this, width, height, colorCount, colorViews, depthStencilView );
	}

	inline agpu_renderpass* createRenderPass ( agpu_renderpass_description* description )
	{
		return agpuCreateRenderPass( this, description );
	}

	inline agpu_texture* createTexture ( agpu_texture_description* description )
	{
		return agpuCreateTexture( this, description );
	}

	inline agpu_fence* createFence (  )
	{
		return agpuCreateFence( this );
	}

	inline agpu_int getMultiSampleQualityLevels ( agpu_uint sample_count )
	{
		return agpuGetMultiSampleQualityLevels( this, sample_count );
	}

	inline agpu_bool hasTopLeftNdcOrigin (  )
	{
		return agpuHasTopLeftNdcOrigin( this );
	}

	inline agpu_bool hasBottomLeftTextureCoordinates (  )
	{
		return agpuHasBottomLeftTextureCoordinates( this );
	}

	inline agpu_bool isFeatureSupported ( agpu_feature feature )
	{
		return agpuIsFeatureSupportedOnDevice( this, feature );
	}

};

typedef agpu_ref<agpu_device> agpu_device_ref;

// Interface wrapper for agpu_swap_chain.
struct _agpu_swap_chain
{
private:
	_agpu_swap_chain() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddSwapChainReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseSwapChain( this ));
	}

	inline void swapBuffers (  )
	{
		agpuThrowIfFailed(agpuSwapBuffers( this ));
	}

	inline agpu_framebuffer* getCurrentBackBuffer (  )
	{
		return agpuGetCurrentBackBuffer( this );
	}

	inline agpu_size getCurrentBackBufferIndex (  )
	{
		return agpuGetCurrentBackBufferIndex( this );
	}

	inline agpu_size getFramebufferCount (  )
	{
		return agpuGetFramebufferCount( this );
	}

};

typedef agpu_ref<agpu_swap_chain> agpu_swap_chain_ref;

// Interface wrapper for agpu_compute_pipeline_builder.
struct _agpu_compute_pipeline_builder
{
private:
	_agpu_compute_pipeline_builder() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddComputePipelineBuilderReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseComputePipelineBuilder( this ));
	}

	inline agpu_pipeline_state* build (  )
	{
		return agpuBuildComputePipelineState( this );
	}

	inline void attachShader ( agpu_shader* shader )
	{
		agpuThrowIfFailed(agpuAttachComputeShader( this, shader ));
	}

	inline void attachShaderWithEntryPoint ( agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
	{
		agpuThrowIfFailed(agpuAttachComputeShaderWithEntryPoint( this, shader, type, entry_point ));
	}

	inline agpu_size getBuildingLogLength (  )
	{
		return agpuGetComputePipelineBuildingLogLength( this );
	}

	inline void getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
	{
		agpuThrowIfFailed(agpuGetComputePipelineBuildingLog( this, buffer_size, buffer ));
	}

	inline void setShaderSignature ( agpu_shader_signature* signature )
	{
		agpuThrowIfFailed(agpuSetComputePipelineShaderSignature( this, signature ));
	}

};

typedef agpu_ref<agpu_compute_pipeline_builder> agpu_compute_pipeline_builder_ref;

// Interface wrapper for agpu_pipeline_builder.
struct _agpu_pipeline_builder
{
private:
	_agpu_pipeline_builder() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddPipelineBuilderReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleasePipelineBuilder( this ));
	}

	inline agpu_pipeline_state* build (  )
	{
		return agpuBuildPipelineState( this );
	}

	inline void attachShader ( agpu_shader* shader )
	{
		agpuThrowIfFailed(agpuAttachShader( this, shader ));
	}

	inline void attachShaderWithEntryPoint ( agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
	{
		agpuThrowIfFailed(agpuAttachShaderWithEntryPoint( this, shader, type, entry_point ));
	}

	inline agpu_size getBuildingLogLength (  )
	{
		return agpuGetPipelineBuildingLogLength( this );
	}

	inline void getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
	{
		agpuThrowIfFailed(agpuGetPipelineBuildingLog( this, buffer_size, buffer ));
	}

	inline void setBlendState ( agpu_int renderTargetMask, agpu_bool enabled )
	{
		agpuThrowIfFailed(agpuSetBlendState( this, renderTargetMask, enabled ));
	}

	inline void setBlendFunction ( agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation )
	{
		agpuThrowIfFailed(agpuSetBlendFunction( this, renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation ));
	}

	inline void setColorMask ( agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled )
	{
		agpuThrowIfFailed(agpuSetColorMask( this, renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled ));
	}

	inline void setFrontFace ( agpu_face_winding winding )
	{
		agpuThrowIfFailed(agpuSetFrontFace( this, winding ));
	}

	inline void setCullMode ( agpu_cull_mode mode )
	{
		agpuThrowIfFailed(agpuSetCullMode( this, mode ));
	}

	inline void setDepthBias ( agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
	{
		agpuThrowIfFailed(agpuSetDepthBias( this, constant_factor, clamp, slope_factor ));
	}

	inline void setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
	{
		agpuThrowIfFailed(agpuSetDepthState( this, enabled, writeMask, function ));
	}

	inline void setPolygonMode ( agpu_polygon_mode mode )
	{
		agpuThrowIfFailed(agpuSetPolygonMode( this, mode ));
	}

	inline void setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
	{
		agpuThrowIfFailed(agpuSetStencilState( this, enabled, writeMask, readMask ));
	}

	inline void setStencilFrontFace ( agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
	{
		agpuThrowIfFailed(agpuSetStencilFrontFace( this, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction ));
	}

	inline void setStencilBackFace ( agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction )
	{
		agpuThrowIfFailed(agpuSetStencilBackFace( this, stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction ));
	}

	inline void setRenderTargetCount ( agpu_int count )
	{
		agpuThrowIfFailed(agpuSetRenderTargetCount( this, count ));
	}

	inline void setRenderTargetFormat ( agpu_uint index, agpu_texture_format format )
	{
		agpuThrowIfFailed(agpuSetRenderTargetFormat( this, index, format ));
	}

	inline void setDepthStencilFormat ( agpu_texture_format format )
	{
		agpuThrowIfFailed(agpuSetDepthStencilFormat( this, format ));
	}

	inline void setPrimitiveType ( agpu_primitive_topology type )
	{
		agpuThrowIfFailed(agpuSetPrimitiveType( this, type ));
	}

	inline void setVertexLayout ( agpu_vertex_layout* layout )
	{
		agpuThrowIfFailed(agpuSetVertexLayout( this, layout ));
	}

	inline void setShaderSignature ( agpu_shader_signature* signature )
	{
		agpuThrowIfFailed(agpuSetPipelineShaderSignature( this, signature ));
	}

	inline void setSampleDescription ( agpu_uint sample_count, agpu_uint sample_quality )
	{
		agpuThrowIfFailed(agpuSetSampleDescription( this, sample_count, sample_quality ));
	}

};

typedef agpu_ref<agpu_pipeline_builder> agpu_pipeline_builder_ref;

// Interface wrapper for agpu_pipeline_state.
struct _agpu_pipeline_state
{
private:
	_agpu_pipeline_state() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddPipelineStateReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleasePipelineState( this ));
	}

};

typedef agpu_ref<agpu_pipeline_state> agpu_pipeline_state_ref;

// Interface wrapper for agpu_command_queue.
struct _agpu_command_queue
{
private:
	_agpu_command_queue() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddCommandQueueReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseCommandQueue( this ));
	}

	inline void addCommandList ( agpu_command_list* command_list )
	{
		agpuThrowIfFailed(agpuAddCommandList( this, command_list ));
	}

	inline void finishExecution (  )
	{
		agpuThrowIfFailed(agpuFinishQueueExecution( this ));
	}

	inline void signalFence ( agpu_fence* fence )
	{
		agpuThrowIfFailed(agpuSignalFence( this, fence ));
	}

	inline void waitFence ( agpu_fence* fence )
	{
		agpuThrowIfFailed(agpuWaitFence( this, fence ));
	}

};

typedef agpu_ref<agpu_command_queue> agpu_command_queue_ref;

// Interface wrapper for agpu_command_allocator.
struct _agpu_command_allocator
{
private:
	_agpu_command_allocator() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddCommandAllocatorReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseCommandAllocator( this ));
	}

	inline void reset (  )
	{
		agpuThrowIfFailed(agpuResetCommandAllocator( this ));
	}

};

typedef agpu_ref<agpu_command_allocator> agpu_command_allocator_ref;

// Interface wrapper for agpu_command_list.
struct _agpu_command_list
{
private:
	_agpu_command_list() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddCommandListReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseCommandList( this ));
	}

	inline void setShaderSignature ( agpu_shader_signature* signature )
	{
		agpuThrowIfFailed(agpuSetShaderSignature( this, signature ));
	}

	inline void setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
	{
		agpuThrowIfFailed(agpuSetViewport( this, x, y, w, h ));
	}

	inline void setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
	{
		agpuThrowIfFailed(agpuSetScissor( this, x, y, w, h ));
	}

	inline void usePipelineState ( agpu_pipeline_state* pipeline )
	{
		agpuThrowIfFailed(agpuUsePipelineState( this, pipeline ));
	}

	inline void useVertexBinding ( agpu_vertex_binding* vertex_binding )
	{
		agpuThrowIfFailed(agpuUseVertexBinding( this, vertex_binding ));
	}

	inline void useIndexBuffer ( agpu_buffer* index_buffer )
	{
		agpuThrowIfFailed(agpuUseIndexBuffer( this, index_buffer ));
	}

	inline void useDrawIndirectBuffer ( agpu_buffer* draw_buffer )
	{
		agpuThrowIfFailed(agpuUseDrawIndirectBuffer( this, draw_buffer ));
	}

	inline void useComputeDispatchIndirectBuffer ( agpu_buffer* buffer )
	{
		agpuThrowIfFailed(agpuUseComputeDispatchIndirectBuffer( this, buffer ));
	}

	inline void useShaderResources ( agpu_shader_resource_binding* binding )
	{
		agpuThrowIfFailed(agpuUseShaderResources( this, binding ));
	}

	inline void useComputeShaderResources ( agpu_shader_resource_binding* binding )
	{
		agpuThrowIfFailed(agpuUseComputeShaderResources( this, binding ));
	}

	inline void drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
	{
		agpuThrowIfFailed(agpuDrawArrays( this, vertex_count, instance_count, first_vertex, base_instance ));
	}

	inline void drawArraysIndirect ( agpu_size offset, agpu_size drawcount )
	{
		agpuThrowIfFailed(agpuDrawArraysIndirect( this, offset, drawcount ));
	}

	inline void drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
	{
		agpuThrowIfFailed(agpuDrawElements( this, index_count, instance_count, first_index, base_vertex, base_instance ));
	}

	inline void drawElementsIndirect ( agpu_size offset, agpu_size drawcount )
	{
		agpuThrowIfFailed(agpuDrawElementsIndirect( this, offset, drawcount ));
	}

	inline void dispatchCompute ( agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z )
	{
		agpuThrowIfFailed(agpuDispatchCompute( this, group_count_x, group_count_y, group_count_z ));
	}

	inline void dispatchComputeIndirect ( agpu_size offset )
	{
		agpuThrowIfFailed(agpuDispatchComputeIndirect( this, offset ));
	}

	inline void setStencilReference ( agpu_uint reference )
	{
		agpuThrowIfFailed(agpuSetStencilReference( this, reference ));
	}

	inline void executeBundle ( agpu_command_list* bundle )
	{
		agpuThrowIfFailed(agpuExecuteBundle( this, bundle ));
	}

	inline void close (  )
	{
		agpuThrowIfFailed(agpuCloseCommandList( this ));
	}

	inline void reset ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
	{
		agpuThrowIfFailed(agpuResetCommandList( this, allocator, initial_pipeline_state ));
	}

	inline void resetBundle ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info )
	{
		agpuThrowIfFailed(agpuResetBundleCommandList( this, allocator, initial_pipeline_state, inheritance_info ));
	}

	inline void beginRenderPass ( agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content )
	{
		agpuThrowIfFailed(agpuBeginRenderPass( this, renderpass, framebuffer, bundle_content ));
	}

	inline void endRenderPass (  )
	{
		agpuThrowIfFailed(agpuEndRenderPass( this ));
	}

	inline void resolveFramebuffer ( agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer )
	{
		agpuThrowIfFailed(agpuResolveFramebuffer( this, destFramebuffer, sourceFramebuffer ));
	}

	inline void resolveTexture ( agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect )
	{
		agpuThrowIfFailed(agpuResolveTexture( this, sourceTexture, sourceLevel, sourceLayer, destTexture, destLevel, destLayer, levelCount, layerCount, aspect ));
	}

	inline void pushConstants ( agpu_uint offset, agpu_uint size, agpu_pointer values )
	{
		agpuThrowIfFailed(agpuPushConstants( this, offset, size, values ));
	}

};

typedef agpu_ref<agpu_command_list> agpu_command_list_ref;

// Interface wrapper for agpu_texture.
struct _agpu_texture
{
private:
	_agpu_texture() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddTextureReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseTexture( this ));
	}

	inline void getDescription ( agpu_texture_description* description )
	{
		agpuThrowIfFailed(agpuGetTextureDescription( this, description ));
	}

	inline agpu_pointer mapLevel ( agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region )
	{
		return agpuMapTextureLevel( this, level, arrayIndex, flags, region );
	}

	inline void unmapLevel (  )
	{
		agpuThrowIfFailed(agpuUnmapTextureLevel( this ));
	}

	inline void readTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer )
	{
		agpuThrowIfFailed(agpuReadTextureData( this, level, arrayIndex, pitch, slicePitch, buffer ));
	}

	inline void uploadTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
	{
		agpuThrowIfFailed(agpuUploadTextureData( this, level, arrayIndex, pitch, slicePitch, data ));
	}

	inline void uploadTextureSubData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data )
	{
		agpuThrowIfFailed(agpuUploadTextureSubData( this, level, arrayIndex, pitch, slicePitch, sourceSize, destRegion, data ));
	}

	inline void discardUploadBuffer (  )
	{
		agpuThrowIfFailed(agpuDiscardTextureUploadBuffer( this ));
	}

	inline void discardReadbackBuffer (  )
	{
		agpuThrowIfFailed(agpuDiscardTextureReadbackBuffer( this ));
	}

	inline void getFullViewDescription ( agpu_texture_view_description* result )
	{
		agpuThrowIfFailed(agpuGetTextureFullViewDescription( this, result ));
	}

};

typedef agpu_ref<agpu_texture> agpu_texture_ref;

// Interface wrapper for agpu_buffer.
struct _agpu_buffer
{
private:
	_agpu_buffer() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddBufferReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseBuffer( this ));
	}

	inline agpu_pointer mapBuffer ( agpu_mapping_access flags )
	{
		return agpuMapBuffer( this, flags );
	}

	inline void unmapBuffer (  )
	{
		agpuThrowIfFailed(agpuUnmapBuffer( this ));
	}

	inline void getDescription ( agpu_buffer_description* description )
	{
		agpuThrowIfFailed(agpuGetBufferDescription( this, description ));
	}

	inline void uploadBufferData ( agpu_size offset, agpu_size size, agpu_pointer data )
	{
		agpuThrowIfFailed(agpuUploadBufferData( this, offset, size, data ));
	}

	inline void readBufferData ( agpu_size offset, agpu_size size, agpu_pointer data )
	{
		agpuThrowIfFailed(agpuReadBufferData( this, offset, size, data ));
	}

	inline void flushWholeBuffer (  )
	{
		agpuThrowIfFailed(agpuFlushWholeBuffer( this ));
	}

	inline void invalidateWholeBuffer (  )
	{
		agpuThrowIfFailed(agpuInvalidateWholeBuffer( this ));
	}

};

typedef agpu_ref<agpu_buffer> agpu_buffer_ref;

// Interface wrapper for agpu_vertex_binding.
struct _agpu_vertex_binding
{
private:
	_agpu_vertex_binding() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddVertexBindingReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseVertexBinding( this ));
	}

	inline void bindVertexBuffers ( agpu_uint count, agpu_buffer** vertex_buffers )
	{
		agpuThrowIfFailed(agpuBindVertexBuffers( this, count, vertex_buffers ));
	}

	inline void bindVertexBuffersWithOffsets ( agpu_uint count, agpu_buffer** vertex_buffers, agpu_size* offsets )
	{
		agpuThrowIfFailed(agpuBindVertexBuffersWithOffsets( this, count, vertex_buffers, offsets ));
	}

};

typedef agpu_ref<agpu_vertex_binding> agpu_vertex_binding_ref;

// Interface wrapper for agpu_vertex_layout.
struct _agpu_vertex_layout
{
private:
	_agpu_vertex_layout() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddVertexLayoutReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseVertexLayout( this ));
	}

	inline void addVertexAttributeBindings ( agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
	{
		agpuThrowIfFailed(agpuAddVertexAttributeBindings( this, vertex_buffer_count, vertex_strides, attribute_count, attributes ));
	}

};

typedef agpu_ref<agpu_vertex_layout> agpu_vertex_layout_ref;

// Interface wrapper for agpu_shader.
struct _agpu_shader
{
private:
	_agpu_shader() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddShaderReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseShader( this ));
	}

	inline void setShaderSource ( agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength )
	{
		agpuThrowIfFailed(agpuSetShaderSource( this, language, sourceText, sourceTextLength ));
	}

	inline void compileShader ( agpu_cstring options )
	{
		agpuThrowIfFailed(agpuCompileShader( this, options ));
	}

	inline agpu_size getCompilationLogLength (  )
	{
		return agpuGetShaderCompilationLogLength( this );
	}

	inline void getCompilationLog ( agpu_size buffer_size, agpu_string_buffer buffer )
	{
		agpuThrowIfFailed(agpuGetShaderCompilationLog( this, buffer_size, buffer ));
	}

};

typedef agpu_ref<agpu_shader> agpu_shader_ref;

// Interface wrapper for agpu_framebuffer.
struct _agpu_framebuffer
{
private:
	_agpu_framebuffer() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddFramebufferReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseFramebuffer( this ));
	}

};

typedef agpu_ref<agpu_framebuffer> agpu_framebuffer_ref;

// Interface wrapper for agpu_renderpass.
struct _agpu_renderpass
{
private:
	_agpu_renderpass() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddRenderPassReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseRenderPass( this ));
	}

	inline void setDepthStencilClearValue ( agpu_depth_stencil_value value )
	{
		agpuThrowIfFailed(agpuSetDepthStencilClearValue( this, value ));
	}

	inline void setColorClearValue ( agpu_uint attachment_index, agpu_color4f value )
	{
		agpuThrowIfFailed(agpuSetColorClearValue( this, attachment_index, value ));
	}

	inline void setColorClearValueFrom ( agpu_uint attachment_index, agpu_color4f* value )
	{
		agpuThrowIfFailed(agpuSetColorClearValueFrom( this, attachment_index, value ));
	}

};

typedef agpu_ref<agpu_renderpass> agpu_renderpass_ref;

// Interface wrapper for agpu_shader_signature_builder.
struct _agpu_shader_signature_builder
{
private:
	_agpu_shader_signature_builder() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddShaderSignatureBuilderReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseShaderSignatureBuilder( this ));
	}

	inline agpu_shader_signature* build (  )
	{
		return agpuBuildShaderSignature( this );
	}

	inline void addBindingConstant (  )
	{
		agpuThrowIfFailed(agpuAddShaderSignatureBindingConstant( this ));
	}

	inline void addBindingElement ( agpu_shader_binding_type type, agpu_uint maxBindings )
	{
		agpuThrowIfFailed(agpuAddShaderSignatureBindingElement( this, type, maxBindings ));
	}

	inline void beginBindingBank ( agpu_uint maxBindings )
	{
		agpuThrowIfFailed(agpuBeginShaderSignatureBindingBank( this, maxBindings ));
	}

	inline void addBindingBankElement ( agpu_shader_binding_type type, agpu_uint bindingPointCount )
	{
		agpuThrowIfFailed(agpuAddShaderSignatureBindingBankElement( this, type, bindingPointCount ));
	}

};

typedef agpu_ref<agpu_shader_signature_builder> agpu_shader_signature_builder_ref;

// Interface wrapper for agpu_shader_signature.
struct _agpu_shader_signature
{
private:
	_agpu_shader_signature() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddShaderSignature( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseShaderSignature( this ));
	}

	inline agpu_shader_resource_binding* createShaderResourceBinding ( agpu_uint element )
	{
		return agpuCreateShaderResourceBinding( this, element );
	}

};

typedef agpu_ref<agpu_shader_signature> agpu_shader_signature_ref;

// Interface wrapper for agpu_shader_resource_binding.
struct _agpu_shader_resource_binding
{
private:
	_agpu_shader_resource_binding() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddShaderResourceBindingReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseShaderResourceBinding( this ));
	}

	inline void bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer )
	{
		agpuThrowIfFailed(agpuBindUniformBuffer( this, location, uniform_buffer ));
	}

	inline void bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
	{
		agpuThrowIfFailed(agpuBindUniformBufferRange( this, location, uniform_buffer, offset, size ));
	}

	inline void bindStorageBuffer ( agpu_int location, agpu_buffer* storage_buffer )
	{
		agpuThrowIfFailed(agpuBindStorageBuffer( this, location, storage_buffer ));
	}

	inline void bindStorageBufferRange ( agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size )
	{
		agpuThrowIfFailed(agpuBindStorageBufferRange( this, location, storage_buffer, offset, size ));
	}

	inline void bindTexture ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp )
	{
		agpuThrowIfFailed(agpuBindTexture( this, location, texture, startMiplevel, miplevels, lodclamp ));
	}

	inline void bindTextureArrayRange ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp )
	{
		agpuThrowIfFailed(agpuBindTextureArrayRange( this, location, texture, startMiplevel, miplevels, firstElement, numberOfElements, lodclamp ));
	}

	inline void bindImage ( agpu_int location, agpu_texture* texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format )
	{
		agpuThrowIfFailed(agpuBindImage( this, location, texture, level, layer, access, format ));
	}

	inline void createSampler ( agpu_int location, agpu_sampler_description* description )
	{
		agpuThrowIfFailed(agpuCreateSampler( this, location, description ));
	}

};

typedef agpu_ref<agpu_shader_resource_binding> agpu_shader_resource_binding_ref;

// Interface wrapper for agpu_fence.
struct _agpu_fence
{
private:
	_agpu_fence() {}

public:
	inline void addReference (  )
	{
		agpuThrowIfFailed(agpuAddFenceReference( this ));
	}

	inline void release (  )
	{
		agpuThrowIfFailed(agpuReleaseFenceReference( this ));
	}

	inline void waitOnClient (  )
	{
		agpuThrowIfFailed(agpuWaitOnClient( this ));
	}

};

typedef agpu_ref<agpu_fence> agpu_fence_ref;


#endif /* AGPU_HPP_ */
