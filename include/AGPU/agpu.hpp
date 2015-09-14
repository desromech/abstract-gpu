
#ifndef AGPU_HPP_
#define AGPU_HPP_

#include <stdexcept>
#include "AGPU/agpu.h"

/**
 * Abstract GPU exception.
 */
class agpu_exception : public std::runtime_error
{
public:
    explicit agpu_exception(agpu_error error)
        : std::runtime_error("AGPU Error"), errorCode(error)
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
    
    agpu_ref(const agpu_ref<T*> &other)
    {
        if(other.pointer)
            other.pointer->addReference();
        pointer = other.pointer();
    }
    
    agpu_ref(T* pointer)
        : pointer(pointer)
    {
    }

    agpu_ref<T> &operator=(const agpu_ref<T*> &other)
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
inline void AgpuThrowIfFailed(agpu_error error)
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

};

// Interface wrapper for agpu_device.
struct _agpu_device
{
private:
	_agpu_device() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddDeviceReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseDevice( this ));
	}

	inline agpu_command_queue* getDefaultCommandQueue (  )
	{
		return agpuGetDefaultCommandQueue( this );
	}

	inline void swapBuffers (  )
	{
		AgpuThrowIfFailed(agpuSwapBuffers( this ));
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

	inline agpu_shader_resource_binding* createShaderResourceBinding ( agpu_int bindingBank )
	{
		return agpuCreateShaderResourceBinding( this, bindingBank );
	}

	inline agpu_pipeline_builder* createPipelineBuilder (  )
	{
		return agpuCreatePipelineBuilder( this );
	}

	inline agpu_command_allocator* createCommandAllocator (  )
	{
		return agpuCreateCommandAllocator( this );
	}

	inline agpu_command_list* createCommandListBundle ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
	{
		return agpuCreateCommandListBundle( this, allocator, initial_pipeline_state );
	}

	inline agpu_command_list* createCommandList ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
	{
		return agpuCreateCommandList( this, allocator, initial_pipeline_state );
	}

	inline agpu_shader_language getPreferredShaderLanguage (  )
	{
		return agpuGetPreferredShaderLanguage( this );
	}

	inline agpu_shader_language getPreferredHighLevelShaderLanguage (  )
	{
		return agpuGetPreferredHighLevelShaderLanguage( this );
	}

	inline agpu_framebuffer* getCurrentBackBuffer (  )
	{
		return agpuGetCurrentBackBuffer( this );
	}

	inline agpu_framebuffer* createFrameBuffer ( agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil )
	{
		return agpuCreateFrameBuffer( this, width, height, renderTargetCount, hasDepth, hasStencil );
	}

};

// Interface wrapper for agpu_pipeline_builder.
struct _agpu_pipeline_builder
{
private:
	_agpu_pipeline_builder() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddPipelineBuilderReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleasePipelineBuilder( this ));
	}

	inline agpu_pipeline_state* build (  )
	{
		return agpuBuildPipelineState( this );
	}

	inline void attachShader ( agpu_shader* shader )
	{
		AgpuThrowIfFailed(agpuAttachShader( this, shader ));
	}

	inline agpu_size getBuildingLogLength (  )
	{
		return agpuGetPipelineBuildingLogLength( this );
	}

	inline void getBuildingLog ( agpu_size buffer_size, agpu_string_buffer buffer )
	{
		AgpuThrowIfFailed(agpuGetPipelineBuildingLog( this, buffer_size, buffer ));
	}

	inline void setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function )
	{
		AgpuThrowIfFailed(agpuSetDepthState( this, enabled, writeMask, function ));
	}

	inline void setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask )
	{
		AgpuThrowIfFailed(agpuSetStencilState( this, enabled, writeMask, readMask ));
	}

	inline void setRenderTargetCount ( agpu_int count )
	{
		AgpuThrowIfFailed(agpuSetRenderTargetCount( this, count ));
	}

	inline void setPrimitiveType ( agpu_primitive_type type )
	{
		AgpuThrowIfFailed(agpuSetPrimitiveType( this, type ));
	}

	inline void setVertexLayout ( agpu_vertex_layout* layout )
	{
		AgpuThrowIfFailed(agpuSetVertexLayout( this, layout ));
	}

};

// Interface wrapper for agpu_pipeline_state.
struct _agpu_pipeline_state
{
private:
	_agpu_pipeline_state() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddPipelineStateReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleasePipelineState( this ));
	}

	inline agpu_int getUniformLocation ( agpu_cstring name )
	{
		return agpuGetUniformLocation( this, name );
	}

};

// Interface wrapper for agpu_command_queue.
struct _agpu_command_queue
{
private:
	_agpu_command_queue() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddCommandQueueReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseCommandQueue( this ));
	}

	inline void addCommandList ( agpu_command_list* command_list )
	{
		AgpuThrowIfFailed(agpuAddCommandList( this, command_list ));
	}

};

// Interface wrapper for agpu_command_allocator.
struct _agpu_command_allocator
{
private:
	_agpu_command_allocator() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddCommandAllocatorReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseCommandAllocator( this ));
	}

	inline void reset (  )
	{
		AgpuThrowIfFailed(agpuResetCommandAllocator( this ));
	}

};

// Interface wrapper for agpu_command_list.
struct _agpu_command_list
{
private:
	_agpu_command_list() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddCommandListReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseCommandList( this ));
	}

	inline void setViewport ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
	{
		AgpuThrowIfFailed(agpuSetViewport( this, x, y, w, h ));
	}

	inline void setScissor ( agpu_int x, agpu_int y, agpu_int w, agpu_int h )
	{
		AgpuThrowIfFailed(agpuSetScissor( this, x, y, w, h ));
	}

	inline void setClearColor ( agpu_float r, agpu_float g, agpu_float b, agpu_float a )
	{
		AgpuThrowIfFailed(agpuSetClearColor( this, r, g, b, a ));
	}

	inline void setClearDepth ( agpu_float depth )
	{
		AgpuThrowIfFailed(agpuSetClearDepth( this, depth ));
	}

	inline void setClearStencil ( agpu_int value )
	{
		AgpuThrowIfFailed(agpuSetClearStencil( this, value ));
	}

	inline void clear ( agpu_bitfield buffers )
	{
		AgpuThrowIfFailed(agpuClear( this, buffers ));
	}

	inline void usePipelineState ( agpu_pipeline_state* pipeline )
	{
		AgpuThrowIfFailed(agpuUsePipelineState( this, pipeline ));
	}

	inline void useVertexBinding ( agpu_vertex_binding* vertex_binding )
	{
		AgpuThrowIfFailed(agpuUseVertexBinding( this, vertex_binding ));
	}

	inline void useIndexBuffer ( agpu_buffer* index_buffer )
	{
		AgpuThrowIfFailed(agpuUseIndexBuffer( this, index_buffer ));
	}

	inline void setPrimitiveTopology ( agpu_primitive_topology topology )
	{
		AgpuThrowIfFailed(agpuSetPrimitiveTopology( this, topology ));
	}

	inline void useDrawIndirectBuffer ( agpu_buffer* draw_buffer )
	{
		AgpuThrowIfFailed(agpuUseDrawIndirectBuffer( this, draw_buffer ));
	}

	inline void useShaderResources ( agpu_shader_resource_binding* binding )
	{
		AgpuThrowIfFailed(agpuUseShaderResources( this, binding ));
	}

	inline void drawArrays ( agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance )
	{
		AgpuThrowIfFailed(agpuDrawArrays( this, vertex_count, instance_count, first_vertex, base_instance ));
	}

	inline void drawElements ( agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance )
	{
		AgpuThrowIfFailed(agpuDrawElements( this, index_count, instance_count, first_index, base_vertex, base_instance ));
	}

	inline void drawElementsIndirect ( agpu_size offset )
	{
		AgpuThrowIfFailed(agpuDrawElementsIndirect( this, offset ));
	}

	inline void multiDrawElementsIndirect ( agpu_size offset, agpu_size drawcount )
	{
		AgpuThrowIfFailed(agpuMultiDrawElementsIndirect( this, offset, drawcount ));
	}

	inline void setStencilReference ( agpu_float reference )
	{
		AgpuThrowIfFailed(agpuSetStencilReference( this, reference ));
	}

	inline void setAlphaReference ( agpu_float reference )
	{
		AgpuThrowIfFailed(agpuSetAlphaReference( this, reference ));
	}

	inline void executeBundle ( agpu_command_list* bundle )
	{
		AgpuThrowIfFailed(agpuExecuteBundle( this, bundle ));
	}

	inline void close (  )
	{
		AgpuThrowIfFailed(agpuCloseCommandList( this ));
	}

	inline void reset ( agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state )
	{
		AgpuThrowIfFailed(agpuResetCommandList( this, allocator, initial_pipeline_state ));
	}

	inline void beginFrame ( agpu_framebuffer* framebuffer )
	{
		AgpuThrowIfFailed(agpuBeginFrame( this, framebuffer ));
	}

	inline void endFrame (  )
	{
		AgpuThrowIfFailed(agpuEndFrame( this ));
	}

};

// Interface wrapper for agpu_texture.
struct _agpu_texture
{
private:
	_agpu_texture() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddTextureReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseTexture( this ));
	}

};

// Interface wrapper for agpu_buffer.
struct _agpu_buffer
{
private:
	_agpu_buffer() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddBufferReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseBuffer( this ));
	}

	inline agpu_pointer mapBuffer ( agpu_mapping_access flags )
	{
		return agpuMapBuffer( this, flags );
	}

	inline void unmapBuffer (  )
	{
		AgpuThrowIfFailed(agpuUnmapBuffer( this ));
	}

	inline void uploadBufferData ( agpu_size offset, agpu_size size, agpu_pointer data )
	{
		AgpuThrowIfFailed(agpuUploadBufferData( this, offset, size, data ));
	}

};

// Interface wrapper for agpu_vertex_binding.
struct _agpu_vertex_binding
{
private:
	_agpu_vertex_binding() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddVertexBindingReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseVertexBinding( this ));
	}

	inline void bindVertexBuffers ( agpu_uint count, agpu_buffer** vertex_buffers )
	{
		AgpuThrowIfFailed(agpuBindVertexBuffers( this, count, vertex_buffers ));
	}

};

// Interface wrapper for agpu_vertex_layout.
struct _agpu_vertex_layout
{
private:
	_agpu_vertex_layout() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddVertexLayoutReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseVertexLayout( this ));
	}

	inline void addVertexAttributeBindings ( agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes )
	{
		AgpuThrowIfFailed(agpuAddVertexAttributeBindings( this, vertex_buffer_count, attribute_count, attributes ));
	}

};

// Interface wrapper for agpu_shader.
struct _agpu_shader
{
private:
	_agpu_shader() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddShaderReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseShader( this ));
	}

	inline void setShaderSource ( agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength )
	{
		AgpuThrowIfFailed(agpuSetShaderSource( this, language, sourceText, sourceTextLength ));
	}

	inline void compileShader ( agpu_cstring options )
	{
		AgpuThrowIfFailed(agpuCompileShader( this, options ));
	}

	inline agpu_size getCompilationLogLength (  )
	{
		return agpuGetShaderCompilationLogLength( this );
	}

	inline void getCompilationLog ( agpu_size buffer_size, agpu_string_buffer buffer )
	{
		AgpuThrowIfFailed(agpuGetShaderCompilationLog( this, buffer_size, buffer ));
	}

	inline void bindAttributeLocation ( agpu_cstring name, agpu_int location )
	{
		AgpuThrowIfFailed(agpuBindAttributeLocation( this, name, location ));
	}

};

// Interface wrapper for agpu_framebuffer.
struct _agpu_framebuffer
{
private:
	_agpu_framebuffer() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddFramebufferReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseFramebuffer( this ));
	}

	inline agpu_bool isMainFrameBuffer (  )
	{
		return agpuIsMainFrameBuffer( this );
	}

};

// Interface wrapper for agpu_shader_resource_binding.
struct _agpu_shader_resource_binding
{
private:
	_agpu_shader_resource_binding() {}

public:
	inline void addReference (  )
	{
		AgpuThrowIfFailed(agpuAddShaderResourceBindingReference( this ));
	}

	inline void release (  )
	{
		AgpuThrowIfFailed(agpuReleaseShaderResourceBinding( this ));
	}

	inline void bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer )
	{
		AgpuThrowIfFailed(agpuBindUniformBuffer( this, location, uniform_buffer ));
	}

	inline void bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
	{
		AgpuThrowIfFailed(agpuBindUniformBufferRange( this, location, uniform_buffer, offset, size ));
	}

};


#endif /* AGPU_HPP_ */
