
#ifndef _AGPU_H_
#define _AGPU_H_

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32
#   ifdef AGPU_BUILD
#       define AGPU_EXPORT __declspec(dllexport)
#   else
#       define AGPU_EXPORT __declspec(dllimport)
#   endif
#else
#   define AGPU_EXPORT
#endif

typedef unsigned char agpu_byte;
typedef signed char agpu_sbyte;
typedef signed short agpu_short;
typedef unsigned short agpu_ushort;
typedef signed int agpu_int;
typedef unsigned int agpu_uint;
typedef void* agpu_pointer;
typedef size_t agpu_size;
typedef int agpu_enum;
typedef bool agpu_bool;
typedef float agpu_float;
typedef double agpu_double;
typedef unsigned int agpu_bitfield;
typedef const char* agpu_cstring;
typedef const char* agpu_string;
typedef int agpu_string_length;
typedef char* agpu_cstring_buffer;
typedef char* agpu_string_buffer;

typedef struct _agpu_platform agpu_platform;
typedef struct _agpu_device agpu_device;
typedef struct _agpu_pipeline_builder agpu_pipeline_builder;
typedef struct _agpu_pipeline_state agpu_pipeline_state;
typedef struct _agpu_command_queue agpu_command_queue;
typedef struct _agpu_command_allocator agpu_command_allocator;
typedef struct _agpu_command_list agpu_command_list;
typedef struct _agpu_texture agpu_texture;
typedef struct _agpu_buffer agpu_buffer;
typedef struct _agpu_vertex_binding agpu_vertex_binding;
typedef struct _agpu_vertex_layout agpu_vertex_layout;
typedef struct _agpu_shader agpu_shader;
typedef struct _agpu_framebuffer agpu_framebuffer;
typedef struct _agpu_shader_resource_binding agpu_shader_resource_binding;

typedef enum {
	AGPU_OK = 0,
	AGPU_ERROR = -1,
	AGPU_NULL_POINTER = -2,
	AGPU_INVALID_OPERATION = -3,
	AGPU_INVALID_PARAMETER = -4,
	AGPU_OUT_OF_BOUNDS = -5,
	AGPU_UNSUPPORTED = -6,
	AGPU_UNIMPLEMENTED = -7,
	AGPU_NOT_CURRENT_CONTEXT = -8,
	AGPU_COMPILATION_ERROR = -9,
	AGPU_LINKING_ERROR = -9,
	AGPU_COMMAND_LIST_CLOSED = -10,
} agpu_error;

typedef enum {
	AGPU_PRIMITIVE_TYPE_POINT = 0,
	AGPU_PRIMITIVE_TYPE_LINE = 1,
	AGPU_PRIMITIVE_TYPE_TRIANGLE = 2,
	AGPU_PRIMITIVE_TYPE_PATCH = 3,
} agpu_primitive_type;

typedef enum {
	AGPU_POINTS = 0,
	AGPU_LINES = 1,
	AGPU_LINES_ADJACENCY = 2,
	AGPU_LINE_STRIP = 3,
	AGPU_LINE_STRIP_ADJACENCY = 4,
	AGPU_TRIANGLES = 5,
	AGPU_TRIANGLES_ADJACENCY = 6,
	AGPU_TRIANGLE_STRIP = 7,
	AGPU_TRIANGLE_STRIP_ADJACENCY = 8,
	AGPU_PATCHES = 9,
} agpu_primitive_topology;

typedef enum {
	AGPU_KEEP = 0,
	AGPU_ZERO = 1,
	AGPU_REPLACE = 2,
	AGPU_INVERT = 3,
	AGPU_INCREASE = 4,
	AGPU_INCREASE_WRAP = 5,
	AGPU_DECREASE = 6,
	AGPU_DECREASE_WRAP = 7,
} agpu_stencil_operation;

typedef enum {
	AGPU_ALWAYS = 0,
	AGPU_NEVER = 1,
	AGPU_LESS = 2,
	AGPU_LESS_EQUAL = 3,
	AGPU_EQUAL = 4,
	AGPU_NOT_EQUAL = 5,
	AGPU_GREATER = 6,
	AGPU_GREATER_EQUAL = 7,
} agpu_compare_function;

typedef enum {
	AGPU_TEXTURE_BUFFER = 0,
	AGPU_TEXTURE_1D = 1,
	AGPU_TEXTURE_2D = 2,
	AGPU_TEXTURE_CUBE = 3,
	AGPU_TEXTURE_3D = 4,
	AGPU_TEXTURE_ARRAY_1D = 5,
	AGPU_TEXTURE_ARRAY_2D = 6,
	AGPU_TEXTURE_ARRAY_CUBE = 7,
	AGPU_TEXTURE_ARRAY_3D = 8,
} agpu_texture_type;

typedef enum {
	AGPU_VERTEX_SHADER = 0,
	AGPU_FRAGMENT_SHADER = 1,
	AGPU_GEOMETRY_SHADER = 2,
	AGPU_COMPUTE_SHADER = 3,
	AGPU_TESSELLATION_CONTROL_SHADER = 4,
	AGPU_TESSELLATION_EVALUATION_SHADER = 5,
} agpu_shader_type;

typedef enum {
	AGPU_STATIC = 0,
	AGPU_DYNAMIC = 1,
	AGPU_STREAM = 2,
} agpu_buffer_usage_type;

typedef enum {
	AGPU_ARRAY_BUFFER = 0,
	AGPU_ELEMENT_ARRAY_BUFFER = 1,
	AGPU_UNIFORM_BUFFER = 2,
	AGPU_DRAW_INDIRECT_BUFFER = 3,
} agpu_buffer_binding_type;

typedef enum {
	AGPU_MAP_READ_BIT = 1,
	AGPU_MAP_WRITE_BIT = 2,
	AGPU_MAP_PERSISTENT_BIT = 4,
	AGPU_MAP_COHERENT_BIT = 8,
	AGPU_MAP_DYNAMIC_STORAGE_BIT = 16,
} agpu_buffer_mapping_flags;

typedef enum {
	AGPU_READ_ONLY = 1,
	AGPU_WRITE_ONLY = 2,
	AGPU_READ_WRITE = 3,
} agpu_mapping_access;

typedef enum {
	AGPU_DEPTH_BUFFER_BIT = 1,
	AGPU_STENCIL_BUFFER_BIT = 2,
	AGPU_COLOR_BUFFER_BIT = 4,
} agpu_render_buffer_bit;

typedef enum {
	AGPU_SHADER_LANGUAGE_NONE = 0,
	AGPU_SHADER_LANGUAGE_GLSL = 1,
	AGPU_SHADER_LANGUAGE_EGLSL = 2,
	AGPU_SHADER_LANGUAGE_SPIR_V = 3,
	AGPU_SHADER_LANGUAGE_HLSL = 4,
	AGPU_SHADER_LANGUAGE_BINARY = 5,
} agpu_shader_language;

typedef enum {
	AGPU_FLOAT = 0,
	AGPU_HALF_FLOAT = 1,
	AGPU_DOUBLE = 2,
	AGPU_FIXED = 3,
	AGPU_BYTE = 4,
	AGPU_UNSIGNED_BYTE = 5,
	AGPU_SHORT = 6,
	AGPU_UNSIGNED_SHORT = 7,
	AGPU_INT = 8,
	AGPU_UNSIGNED_INT = 9,
} agpu_field_type;


/* Structure agpu_device_open_info. */
typedef struct agpu_device_open_info {
	agpu_pointer display;
	agpu_pointer window;
	agpu_pointer surface;
	agpu_int red_size;
	agpu_int green_size;
	agpu_int blue_size;
	agpu_int alpha_size;
	agpu_int depth_size;
	agpu_int stencil_size;
	agpu_bool doublebuffer;
	agpu_bool sample_buffers;
	agpu_int samples;
	agpu_bool debugLayer;
} agpu_device_open_info;

/* Structure agpu_buffer_description. */
typedef struct agpu_buffer_description {
	agpu_uint size;
	agpu_buffer_usage_type usage;
	agpu_buffer_binding_type binding;
	agpu_bitfield mapping_flags;
	agpu_uint stride;
} agpu_buffer_description;

/* Structure agpu_draw_elements_command. */
typedef struct agpu_draw_elements_command {
	agpu_uint index_count;
	agpu_uint instance_count;
	agpu_uint first_index;
	agpu_int base_vertex;
	agpu_uint base_instance;
} agpu_draw_elements_command;

/* Structure agpu_vertex_attrib_description. */
typedef struct agpu_vertex_attrib_description {
	agpu_uint buffer;
	agpu_uint binding;
	agpu_field_type type;
	agpu_uint components;
	agpu_uint rows;
	agpu_bool normalized;
	agpu_size offset;
	agpu_uint divisor;
} agpu_vertex_attrib_description;

/* Global functions. */
typedef agpu_error (*agpuGetPlatforms_FUN) ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms );

AGPU_EXPORT agpu_error agpuGetPlatforms ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms );

/* Methods for interface agpu_platform. */
typedef agpu_device* (*agpuOpenDevice_FUN) ( agpu_platform* platform, agpu_device_open_info* openInfo );

AGPU_EXPORT agpu_device* agpuOpenDevice ( agpu_platform* platform, agpu_device_open_info* openInfo );

/* Methods for interface agpu_device. */
typedef agpu_error (*agpuAddDeviceReference_FUN) ( agpu_device* device );
typedef agpu_error (*agpuReleaseDevice_FUN) ( agpu_device* device );
typedef agpu_command_queue* (*agpuGetDefaultCommandQueue_FUN) ( agpu_device* device );
typedef agpu_error (*agpuSwapBuffers_FUN) ( agpu_device* device );
typedef agpu_buffer* (*agpuCreateBuffer_FUN) ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data );
typedef agpu_vertex_layout* (*agpuCreateVertexLayout_FUN) ( agpu_device* device );
typedef agpu_vertex_binding* (*agpuCreateVertexBinding_FUN) ( agpu_device* device, agpu_vertex_layout* layout );
typedef agpu_shader* (*agpuCreateShader_FUN) ( agpu_device* device, agpu_shader_type type );
typedef agpu_shader_resource_binding* (*agpuCreateShaderResourceBinding_FUN) ( agpu_device* device, agpu_int bindingBank );
typedef agpu_pipeline_builder* (*agpuCreatePipelineBuilder_FUN) ( agpu_device* device );
typedef agpu_command_allocator* (*agpuCreateCommandAllocator_FUN) ( agpu_device* device );
typedef agpu_command_list* (*agpuCreateCommandList_FUN) ( agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
typedef agpu_shader_language (*agpuGetPreferredShaderLanguage_FUN) ( agpu_device* device );
typedef agpu_shader_language (*agpuGetPreferredHighLevelShaderLanguage_FUN) ( agpu_device* device );

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device* device );
AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device* device );
AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue ( agpu_device* device );
AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_device* device );
AGPU_EXPORT agpu_buffer* agpuCreateBuffer ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data );
AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout ( agpu_device* device );
AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding ( agpu_device* device, agpu_vertex_layout* layout );
AGPU_EXPORT agpu_shader* agpuCreateShader ( agpu_device* device, agpu_shader_type type );
AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding ( agpu_device* device, agpu_int bindingBank );
AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device );
AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device );
AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage ( agpu_device* device );
AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage ( agpu_device* device );

/* Methods for interface agpu_pipeline_builder. */
typedef agpu_error (*agpuAddPipelineBuilderReference_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_error (*agpuReleasePipelineBuilder_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_pipeline_state* (*agpuBuildPipelineState_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_error (*agpuAttachShader_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader );
typedef agpu_size (*agpuGetPipelineBuildingLogLength_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_error (*agpuGetPipelineBuildingLog_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer );
typedef agpu_error (*agpuSetDepthState_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function );
typedef agpu_error (*agpuSetStencilState_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask );
typedef agpu_error (*agpuSetRenderTargetCount_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_int count );
typedef agpu_error (*agpuSetPrimitiveType_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_type type );
typedef agpu_error (*agpuSetVertexLayout_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout );

AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_error agpuReleasePipelineBuilder ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_error agpuAttachShader ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader );
AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer );
AGPU_EXPORT agpu_error agpuSetDepthState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function );
AGPU_EXPORT agpu_error agpuSetStencilState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask );
AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count );
AGPU_EXPORT agpu_error agpuSetPrimitiveType ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_type type );
AGPU_EXPORT agpu_error agpuSetVertexLayout ( agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout );

/* Methods for interface agpu_pipeline_state. */
typedef agpu_error (*agpuAddPipelineStateReference_FUN) ( agpu_pipeline_state* pipeline_state );
typedef agpu_error (*agpuReleasePipelineState_FUN) ( agpu_pipeline_state* pipeline_state );
typedef agpu_int (*agpuGetUniformLocation_FUN) ( agpu_pipeline_state* pipeline_state, agpu_cstring name );

AGPU_EXPORT agpu_error agpuAddPipelineStateReference ( agpu_pipeline_state* pipeline_state );
AGPU_EXPORT agpu_error agpuReleasePipelineState ( agpu_pipeline_state* pipeline_state );
AGPU_EXPORT agpu_int agpuGetUniformLocation ( agpu_pipeline_state* pipeline_state, agpu_cstring name );

/* Methods for interface agpu_command_queue. */
typedef agpu_error (*agpuAddCommandQueueReference_FUN) ( agpu_command_queue* command_queue );
typedef agpu_error (*agpuReleaseCommandQueue_FUN) ( agpu_command_queue* command_queue );
typedef agpu_error (*agpuAddCommandList_FUN) ( agpu_command_queue* command_queue, agpu_command_list* command_list );

AGPU_EXPORT agpu_error agpuAddCommandQueueReference ( agpu_command_queue* command_queue );
AGPU_EXPORT agpu_error agpuReleaseCommandQueue ( agpu_command_queue* command_queue );
AGPU_EXPORT agpu_error agpuAddCommandList ( agpu_command_queue* command_queue, agpu_command_list* command_list );

/* Methods for interface agpu_command_allocator. */
typedef agpu_error (*agpuAddCommandAllocatorReference_FUN) ( agpu_command_allocator* command_allocator );
typedef agpu_error (*agpuReleaseCommandAllocator_FUN) ( agpu_command_allocator* command_allocator );
typedef agpu_error (*agpuResetCommandAllocator_FUN) ( agpu_command_allocator* command_allocator );

AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference ( agpu_command_allocator* command_allocator );
AGPU_EXPORT agpu_error agpuReleaseCommandAllocator ( agpu_command_allocator* command_allocator );
AGPU_EXPORT agpu_error agpuResetCommandAllocator ( agpu_command_allocator* command_allocator );

/* Methods for interface agpu_command_list. */
typedef agpu_error (*agpuAddCommandListReference_FUN) ( agpu_command_list* command_list );
typedef agpu_error (*agpuReleaseCommandList_FUN) ( agpu_command_list* command_list );
typedef agpu_error (*agpuSetViewport_FUN) ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h );
typedef agpu_error (*agpuSetScissor_FUN) ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h );
typedef agpu_error (*agpuSetClearColor_FUN) ( agpu_command_list* command_list, agpu_float r, agpu_float g, agpu_float b, agpu_float a );
typedef agpu_error (*agpuSetClearDepth_FUN) ( agpu_command_list* command_list, agpu_float depth );
typedef agpu_error (*agpuSetClearStencil_FUN) ( agpu_command_list* command_list, agpu_int value );
typedef agpu_error (*agpuClear_FUN) ( agpu_command_list* command_list, agpu_bitfield buffers );
typedef agpu_error (*agpuUsePipelineState_FUN) ( agpu_command_list* command_list, agpu_pipeline_state* pipeline );
typedef agpu_error (*agpuUseVertexBinding_FUN) ( agpu_command_list* command_list, agpu_vertex_binding* vertex_binding );
typedef agpu_error (*agpuUseIndexBuffer_FUN) ( agpu_command_list* command_list, agpu_buffer* index_buffer );
typedef agpu_error (*agpuSetPrimitiveTopology_FUN) ( agpu_command_list* command_list, agpu_primitive_topology topology );
typedef agpu_error (*agpuUseDrawIndirectBuffer_FUN) ( agpu_command_list* command_list, agpu_buffer* draw_buffer );
typedef agpu_error (*agpuUseShaderResources_FUN) ( agpu_command_list* command_list, agpu_shader_resource_binding* binding );
typedef agpu_error (*agpuDrawArrays_FUN) ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance );
typedef agpu_error (*agpuDrawElements_FUN) ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance );
typedef agpu_error (*agpuDrawElementsIndirect_FUN) ( agpu_command_list* command_list, agpu_size offset );
typedef agpu_error (*agpuMultiDrawElementsIndirect_FUN) ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount );
typedef agpu_error (*agpuSetStencilReference_FUN) ( agpu_command_list* command_list, agpu_float reference );
typedef agpu_error (*agpuSetAlphaReference_FUN) ( agpu_command_list* command_list, agpu_float reference );
typedef agpu_error (*agpuCloseCommandList_FUN) ( agpu_command_list* command_list );
typedef agpu_error (*agpuResetCommandList_FUN) ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
typedef agpu_error (*agpuBeginFrame_FUN) ( agpu_command_list* command_list );
typedef agpu_error (*agpuEndFrame_FUN) ( agpu_command_list* command_list );

AGPU_EXPORT agpu_error agpuAddCommandListReference ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuReleaseCommandList ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuSetViewport ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h );
AGPU_EXPORT agpu_error agpuSetScissor ( agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h );
AGPU_EXPORT agpu_error agpuSetClearColor ( agpu_command_list* command_list, agpu_float r, agpu_float g, agpu_float b, agpu_float a );
AGPU_EXPORT agpu_error agpuSetClearDepth ( agpu_command_list* command_list, agpu_float depth );
AGPU_EXPORT agpu_error agpuSetClearStencil ( agpu_command_list* command_list, agpu_int value );
AGPU_EXPORT agpu_error agpuClear ( agpu_command_list* command_list, agpu_bitfield buffers );
AGPU_EXPORT agpu_error agpuUsePipelineState ( agpu_command_list* command_list, agpu_pipeline_state* pipeline );
AGPU_EXPORT agpu_error agpuUseVertexBinding ( agpu_command_list* command_list, agpu_vertex_binding* vertex_binding );
AGPU_EXPORT agpu_error agpuUseIndexBuffer ( agpu_command_list* command_list, agpu_buffer* index_buffer );
AGPU_EXPORT agpu_error agpuSetPrimitiveTopology ( agpu_command_list* command_list, agpu_primitive_topology topology );
AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer ( agpu_command_list* command_list, agpu_buffer* draw_buffer );
AGPU_EXPORT agpu_error agpuUseShaderResources ( agpu_command_list* command_list, agpu_shader_resource_binding* binding );
AGPU_EXPORT agpu_error agpuDrawArrays ( agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance );
AGPU_EXPORT agpu_error agpuDrawElements ( agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance );
AGPU_EXPORT agpu_error agpuDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset );
AGPU_EXPORT agpu_error agpuMultiDrawElementsIndirect ( agpu_command_list* command_list, agpu_size offset, agpu_size drawcount );
AGPU_EXPORT agpu_error agpuSetStencilReference ( agpu_command_list* command_list, agpu_float reference );
AGPU_EXPORT agpu_error agpuSetAlphaReference ( agpu_command_list* command_list, agpu_float reference );
AGPU_EXPORT agpu_error agpuCloseCommandList ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuResetCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
AGPU_EXPORT agpu_error agpuBeginFrame ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuEndFrame ( agpu_command_list* command_list );

/* Methods for interface agpu_texture. */


/* Methods for interface agpu_buffer. */
typedef agpu_error (*agpuAddBufferReference_FUN) ( agpu_buffer* buffer );
typedef agpu_error (*agpuReleaseBuffer_FUN) ( agpu_buffer* buffer );
typedef agpu_pointer (*agpuMapBuffer_FUN) ( agpu_buffer* buffer, agpu_mapping_access flags );
typedef agpu_error (*agpuUnmapBuffer_FUN) ( agpu_buffer* buffer );
typedef agpu_error (*agpuUploadBufferData_FUN) ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer );
AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer );
AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags );
AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer );
AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

/* Methods for interface agpu_vertex_binding. */
typedef agpu_error (*agpuAddVertexBindingReference_FUN) ( agpu_vertex_binding* vertex_binding );
typedef agpu_error (*agpuReleaseVertexBinding_FUN) ( agpu_vertex_binding* vertex_binding );
typedef agpu_error (*agpuBindVertexBuffers_FUN) ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers );

AGPU_EXPORT agpu_error agpuAddVertexBindingReference ( agpu_vertex_binding* vertex_binding );
AGPU_EXPORT agpu_error agpuReleaseVertexBinding ( agpu_vertex_binding* vertex_binding );
AGPU_EXPORT agpu_error agpuBindVertexBuffers ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers );

/* Methods for interface agpu_vertex_layout. */
typedef agpu_error (*agpuAddVertexLayoutReference_FUN) ( agpu_vertex_layout* vertex_layout );
typedef agpu_error (*agpuReleaseVertexLayout_FUN) ( agpu_vertex_layout* vertex_layout );
typedef agpu_error (*agpuAddVertexAttributeBindings_FUN) ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes );

AGPU_EXPORT agpu_error agpuAddVertexLayoutReference ( agpu_vertex_layout* vertex_layout );
AGPU_EXPORT agpu_error agpuReleaseVertexLayout ( agpu_vertex_layout* vertex_layout );
AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size attribute_count, agpu_vertex_attrib_description* attributes );

/* Methods for interface agpu_shader. */
typedef agpu_error (*agpuAddShaderReference_FUN) ( agpu_shader* shader );
typedef agpu_error (*agpuReleaseShader_FUN) ( agpu_shader* shader );
typedef agpu_error (*agpuSetShaderSource_FUN) ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength );
typedef agpu_error (*agpuCompileShader_FUN) ( agpu_shader* shader, agpu_cstring options );
typedef agpu_size (*agpuGetShaderCompilationLogLength_FUN) ( agpu_shader* shader );
typedef agpu_error (*agpuGetShaderCompilationLog_FUN) ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer );
typedef agpu_error (*agpuBindAttributeLocation_FUN) ( agpu_shader* shader, agpu_cstring name, agpu_int location );

AGPU_EXPORT agpu_error agpuAddShaderReference ( agpu_shader* shader );
AGPU_EXPORT agpu_error agpuReleaseShader ( agpu_shader* shader );
AGPU_EXPORT agpu_error agpuSetShaderSource ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength );
AGPU_EXPORT agpu_error agpuCompileShader ( agpu_shader* shader, agpu_cstring options );
AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength ( agpu_shader* shader );
AGPU_EXPORT agpu_error agpuGetShaderCompilationLog ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer );
AGPU_EXPORT agpu_error agpuBindAttributeLocation ( agpu_shader* shader, agpu_cstring name, agpu_int location );

/* Methods for interface agpu_framebuffer. */


/* Methods for interface agpu_shader_resource_binding. */
typedef agpu_error (*agpuAddShaderResourceBindingReference_FUN) ( agpu_shader_resource_binding* shader_resource_binding );
typedef agpu_error (*agpuReleaseShaderResourceBinding_FUN) ( agpu_shader_resource_binding* shader_resource_binding );
typedef agpu_error (*agpuBindUniformBuffer_FUN) ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer );

AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference ( agpu_shader_resource_binding* shader_resource_binding );
AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding ( agpu_shader_resource_binding* shader_resource_binding );
AGPU_EXPORT agpu_error agpuBindUniformBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer );

/* Installable client driver interface. */
typedef struct _agpu_icd_dispatch {
	int icd_interface_version;
	agpuGetPlatforms_FUN agpuGetPlatforms;
	agpuOpenDevice_FUN agpuOpenDevice;
	agpuAddDeviceReference_FUN agpuAddDeviceReference;
	agpuReleaseDevice_FUN agpuReleaseDevice;
	agpuGetDefaultCommandQueue_FUN agpuGetDefaultCommandQueue;
	agpuSwapBuffers_FUN agpuSwapBuffers;
	agpuCreateBuffer_FUN agpuCreateBuffer;
	agpuCreateVertexLayout_FUN agpuCreateVertexLayout;
	agpuCreateVertexBinding_FUN agpuCreateVertexBinding;
	agpuCreateShader_FUN agpuCreateShader;
	agpuCreateShaderResourceBinding_FUN agpuCreateShaderResourceBinding;
	agpuCreatePipelineBuilder_FUN agpuCreatePipelineBuilder;
	agpuCreateCommandAllocator_FUN agpuCreateCommandAllocator;
	agpuCreateCommandList_FUN agpuCreateCommandList;
	agpuGetPreferredShaderLanguage_FUN agpuGetPreferredShaderLanguage;
	agpuGetPreferredHighLevelShaderLanguage_FUN agpuGetPreferredHighLevelShaderLanguage;
	agpuAddPipelineBuilderReference_FUN agpuAddPipelineBuilderReference;
	agpuReleasePipelineBuilder_FUN agpuReleasePipelineBuilder;
	agpuBuildPipelineState_FUN agpuBuildPipelineState;
	agpuAttachShader_FUN agpuAttachShader;
	agpuGetPipelineBuildingLogLength_FUN agpuGetPipelineBuildingLogLength;
	agpuGetPipelineBuildingLog_FUN agpuGetPipelineBuildingLog;
	agpuSetDepthState_FUN agpuSetDepthState;
	agpuSetStencilState_FUN agpuSetStencilState;
	agpuSetRenderTargetCount_FUN agpuSetRenderTargetCount;
	agpuSetPrimitiveType_FUN agpuSetPrimitiveType;
	agpuSetVertexLayout_FUN agpuSetVertexLayout;
	agpuAddPipelineStateReference_FUN agpuAddPipelineStateReference;
	agpuReleasePipelineState_FUN agpuReleasePipelineState;
	agpuGetUniformLocation_FUN agpuGetUniformLocation;
	agpuAddCommandQueueReference_FUN agpuAddCommandQueueReference;
	agpuReleaseCommandQueue_FUN agpuReleaseCommandQueue;
	agpuAddCommandList_FUN agpuAddCommandList;
	agpuAddCommandAllocatorReference_FUN agpuAddCommandAllocatorReference;
	agpuReleaseCommandAllocator_FUN agpuReleaseCommandAllocator;
	agpuResetCommandAllocator_FUN agpuResetCommandAllocator;
	agpuAddCommandListReference_FUN agpuAddCommandListReference;
	agpuReleaseCommandList_FUN agpuReleaseCommandList;
	agpuSetViewport_FUN agpuSetViewport;
	agpuSetScissor_FUN agpuSetScissor;
	agpuSetClearColor_FUN agpuSetClearColor;
	agpuSetClearDepth_FUN agpuSetClearDepth;
	agpuSetClearStencil_FUN agpuSetClearStencil;
	agpuClear_FUN agpuClear;
	agpuUsePipelineState_FUN agpuUsePipelineState;
	agpuUseVertexBinding_FUN agpuUseVertexBinding;
	agpuUseIndexBuffer_FUN agpuUseIndexBuffer;
	agpuSetPrimitiveTopology_FUN agpuSetPrimitiveTopology;
	agpuUseDrawIndirectBuffer_FUN agpuUseDrawIndirectBuffer;
	agpuUseShaderResources_FUN agpuUseShaderResources;
	agpuDrawArrays_FUN agpuDrawArrays;
	agpuDrawElements_FUN agpuDrawElements;
	agpuDrawElementsIndirect_FUN agpuDrawElementsIndirect;
	agpuMultiDrawElementsIndirect_FUN agpuMultiDrawElementsIndirect;
	agpuSetStencilReference_FUN agpuSetStencilReference;
	agpuSetAlphaReference_FUN agpuSetAlphaReference;
	agpuCloseCommandList_FUN agpuCloseCommandList;
	agpuResetCommandList_FUN agpuResetCommandList;
	agpuBeginFrame_FUN agpuBeginFrame;
	agpuEndFrame_FUN agpuEndFrame;
	agpuAddBufferReference_FUN agpuAddBufferReference;
	agpuReleaseBuffer_FUN agpuReleaseBuffer;
	agpuMapBuffer_FUN agpuMapBuffer;
	agpuUnmapBuffer_FUN agpuUnmapBuffer;
	agpuUploadBufferData_FUN agpuUploadBufferData;
	agpuAddVertexBindingReference_FUN agpuAddVertexBindingReference;
	agpuReleaseVertexBinding_FUN agpuReleaseVertexBinding;
	agpuBindVertexBuffers_FUN agpuBindVertexBuffers;
	agpuAddVertexLayoutReference_FUN agpuAddVertexLayoutReference;
	agpuReleaseVertexLayout_FUN agpuReleaseVertexLayout;
	agpuAddVertexAttributeBindings_FUN agpuAddVertexAttributeBindings;
	agpuAddShaderReference_FUN agpuAddShaderReference;
	agpuReleaseShader_FUN agpuReleaseShader;
	agpuSetShaderSource_FUN agpuSetShaderSource;
	agpuCompileShader_FUN agpuCompileShader;
	agpuGetShaderCompilationLogLength_FUN agpuGetShaderCompilationLogLength;
	agpuGetShaderCompilationLog_FUN agpuGetShaderCompilationLog;
	agpuBindAttributeLocation_FUN agpuBindAttributeLocation;
	agpuAddShaderResourceBindingReference_FUN agpuAddShaderResourceBindingReference;
	agpuReleaseShaderResourceBinding_FUN agpuReleaseShaderResourceBinding;
	agpuBindUniformBuffer_FUN agpuBindUniformBuffer;
} agpu_icd_dispatch;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _AGPU_H_ */
