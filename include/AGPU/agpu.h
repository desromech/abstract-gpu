
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
typedef struct _agpu_context agpu_context;
typedef struct _agpu_texture agpu_texture;
typedef struct _agpu_buffer agpu_buffer;
typedef struct _agpu_shader agpu_shader;
typedef struct _agpu_program agpu_program;
typedef struct _agpu_framebuffer agpu_framebuffer;

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
} agpu_error;

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
	AGPU_SHADER_LANGUAGE_GLSL = 0,
	AGPU_SHADER_LANGUAGE_EGLSL = 1,
	AGPU_SHADER_LANGUAGE_SPIR_V = 2,
	AGPU_SHADER_LANGUAGE_HLSL = 3,
	AGPU_SHADER_LANGUAGE_BINARY = 4,
} agpu_shader_language;


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
	agpu_uint count;
	agpu_uint instance_count;
	agpu_uint first_index;
	agpu_uint base_vertex;
	agpu_uint base_instance;
} agpu_draw_elements_command;

/* Global functions. */
typedef agpu_error (*agpuGetPlatforms_FUN) ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms );

AGPU_EXPORT agpu_error agpuGetPlatforms ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms );

/* Methods for interface agpu_platform. */
typedef agpu_device* (*agpuOpenDevice_FUN) ( agpu_platform* platform, agpu_device_open_info* openInfo );

AGPU_EXPORT agpu_device* agpuOpenDevice ( agpu_platform* platform, agpu_device_open_info* openInfo );

/* Methods for interface agpu_device. */
typedef agpu_error (*agpuAddDeviceReference_FUN) ( agpu_device* device );
typedef agpu_error (*agpuReleaseDevice_FUN) ( agpu_device* device );
typedef agpu_context* (*agpuGetImmediateContext_FUN) ( agpu_device* device );
typedef agpu_context* (*agpuCreateDeferredContext_FUN) ( agpu_device* device );
typedef agpu_error (*agpuSwapBuffers_FUN) ( agpu_device* device );
typedef agpu_buffer* (*agpuCreateBuffer_FUN) ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data );
typedef agpu_shader* (*agpuCreateShader_FUN) ( agpu_device* device, agpu_shader_type type );
typedef agpu_program* (*agpuCreateProgram_FUN) ( agpu_device* device );

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device* device );
AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device* device );
AGPU_EXPORT agpu_context* agpuGetImmediateContext ( agpu_device* device );
AGPU_EXPORT agpu_context* agpuCreateDeferredContext ( agpu_device* device );
AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_device* device );
AGPU_EXPORT agpu_buffer* agpuCreateBuffer ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data );
AGPU_EXPORT agpu_shader* agpuCreateShader ( agpu_device* device, agpu_shader_type type );
AGPU_EXPORT agpu_program* agpuCreateProgram ( agpu_device* device );

/* Methods for interface agpu_context. */
typedef agpu_error (*agpuAddContextReference_FUN) ( agpu_context* context );
typedef agpu_error (*agpuReleaseContext_FUN) ( agpu_context* context );
typedef agpu_error (*agpuFinish_FUN) ( agpu_context* context );
typedef agpu_error (*agpuFlush_FUN) ( agpu_context* context );
typedef agpu_error (*agpuMakeCurrent_FUN) ( agpu_context* context );
typedef agpu_error (*agpuSetClearColor_FUN) ( agpu_context* context, agpu_float r, agpu_float g, agpu_float b, agpu_float a );
typedef agpu_error (*agpuSetClearDepth_FUN) ( agpu_context* context, agpu_float depth );
typedef agpu_error (*agpuSetClearStencil_FUN) ( agpu_context* context, agpu_int value );
typedef agpu_error (*agpuClear_FUN) ( agpu_context* context, agpu_bitfield buffers );
typedef agpu_error (*agpuSetDepthFunction_FUN) ( agpu_context* context, agpu_compare_function function );
typedef agpu_error (*agpuSetAlphaFunction_FUN) ( agpu_context* context, agpu_compare_function function, agpu_float reference );
typedef agpu_error (*agpuUseProgram_FUN) ( agpu_context* context, agpu_program* program );
typedef agpu_error (*agpuUploadBufferData_FUN) ( agpu_context* context, agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

AGPU_EXPORT agpu_error agpuAddContextReference ( agpu_context* context );
AGPU_EXPORT agpu_error agpuReleaseContext ( agpu_context* context );
AGPU_EXPORT agpu_error agpuFinish ( agpu_context* context );
AGPU_EXPORT agpu_error agpuFlush ( agpu_context* context );
AGPU_EXPORT agpu_error agpuMakeCurrent ( agpu_context* context );
AGPU_EXPORT agpu_error agpuSetClearColor ( agpu_context* context, agpu_float r, agpu_float g, agpu_float b, agpu_float a );
AGPU_EXPORT agpu_error agpuSetClearDepth ( agpu_context* context, agpu_float depth );
AGPU_EXPORT agpu_error agpuSetClearStencil ( agpu_context* context, agpu_int value );
AGPU_EXPORT agpu_error agpuClear ( agpu_context* context, agpu_bitfield buffers );
AGPU_EXPORT agpu_error agpuSetDepthFunction ( agpu_context* context, agpu_compare_function function );
AGPU_EXPORT agpu_error agpuSetAlphaFunction ( agpu_context* context, agpu_compare_function function, agpu_float reference );
AGPU_EXPORT agpu_error agpuUseProgram ( agpu_context* context, agpu_program* program );
AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_context* context, agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

/* Methods for interface agpu_texture. */


/* Methods for interface agpu_buffer. */
typedef agpu_error (*agpuAddBufferReference_FUN) ( agpu_buffer* buffer );
typedef agpu_error (*agpuReleaseBuffer_FUN) ( agpu_buffer* buffer );
typedef agpu_pointer (*agpuMapBuffer_FUN) ( agpu_buffer* buffer, agpu_mapping_access flags );
typedef agpu_error (*agpuUnmapBuffer_FUN) ( agpu_buffer* buffer );

AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer );
AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer );
AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags );
AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer );

/* Methods for interface agpu_shader. */
typedef agpu_error (*agpuAddShaderReference_FUN) ( agpu_shader* shader );
typedef agpu_error (*agpuReleaseShader_FUN) ( agpu_shader* shader );
typedef agpu_error (*agpuSetShaderSource_FUN) ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength );
typedef agpu_error (*agpuCompileShader_FUN) ( agpu_shader* shader, agpu_cstring options );
typedef agpu_size (*agpuGetShaderCompilationLogLength_FUN) ( agpu_shader* shader );
typedef agpu_error (*agpuGetShaderCompilationLog_FUN) ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer );

AGPU_EXPORT agpu_error agpuAddShaderReference ( agpu_shader* shader );
AGPU_EXPORT agpu_error agpuReleaseShader ( agpu_shader* shader );
AGPU_EXPORT agpu_error agpuSetShaderSource ( agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength );
AGPU_EXPORT agpu_error agpuCompileShader ( agpu_shader* shader, agpu_cstring options );
AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength ( agpu_shader* shader );
AGPU_EXPORT agpu_error agpuGetShaderCompilationLog ( agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer );

/* Methods for interface agpu_program. */
typedef agpu_error (*agpuAddProgramReference_FUN) ( agpu_program* program );
typedef agpu_error (*agpuReleaseProgram_FUN) ( agpu_program* program );
typedef agpu_error (*agpuAttachShader_FUN) ( agpu_program* program, agpu_shader* shader );
typedef agpu_error (*agpuLinkProgram_FUN) ( agpu_program* program );
typedef agpu_size (*agpuGetProgramLinkingLogLength_FUN) ( agpu_program* program );
typedef agpu_error (*agpuGetProgramLinkingLog_FUN) ( agpu_program* program, agpu_size buffer_size, agpu_string_buffer buffer );
typedef agpu_error (*agpuBindAttributeLocation_FUN) ( agpu_program* program, agpu_cstring name, agpu_int location );

AGPU_EXPORT agpu_error agpuAddProgramReference ( agpu_program* program );
AGPU_EXPORT agpu_error agpuReleaseProgram ( agpu_program* program );
AGPU_EXPORT agpu_error agpuAttachShader ( agpu_program* program, agpu_shader* shader );
AGPU_EXPORT agpu_error agpuLinkProgram ( agpu_program* program );
AGPU_EXPORT agpu_size agpuGetProgramLinkingLogLength ( agpu_program* program );
AGPU_EXPORT agpu_error agpuGetProgramLinkingLog ( agpu_program* program, agpu_size buffer_size, agpu_string_buffer buffer );
AGPU_EXPORT agpu_error agpuBindAttributeLocation ( agpu_program* program, agpu_cstring name, agpu_int location );

/* Methods for interface agpu_framebuffer. */


/* Installable client driver interface. */
typedef struct _agpu_icd_dispatch {
	int icd_interface_version;
	agpuGetPlatforms_FUN agpuGetPlatforms;
	agpuOpenDevice_FUN agpuOpenDevice;
	agpuAddDeviceReference_FUN agpuAddDeviceReference;
	agpuReleaseDevice_FUN agpuReleaseDevice;
	agpuGetImmediateContext_FUN agpuGetImmediateContext;
	agpuCreateDeferredContext_FUN agpuCreateDeferredContext;
	agpuSwapBuffers_FUN agpuSwapBuffers;
	agpuCreateBuffer_FUN agpuCreateBuffer;
	agpuCreateShader_FUN agpuCreateShader;
	agpuCreateProgram_FUN agpuCreateProgram;
	agpuAddContextReference_FUN agpuAddContextReference;
	agpuReleaseContext_FUN agpuReleaseContext;
	agpuFinish_FUN agpuFinish;
	agpuFlush_FUN agpuFlush;
	agpuMakeCurrent_FUN agpuMakeCurrent;
	agpuSetClearColor_FUN agpuSetClearColor;
	agpuSetClearDepth_FUN agpuSetClearDepth;
	agpuSetClearStencil_FUN agpuSetClearStencil;
	agpuClear_FUN agpuClear;
	agpuSetDepthFunction_FUN agpuSetDepthFunction;
	agpuSetAlphaFunction_FUN agpuSetAlphaFunction;
	agpuUseProgram_FUN agpuUseProgram;
	agpuUploadBufferData_FUN agpuUploadBufferData;
	agpuAddBufferReference_FUN agpuAddBufferReference;
	agpuReleaseBuffer_FUN agpuReleaseBuffer;
	agpuMapBuffer_FUN agpuMapBuffer;
	agpuUnmapBuffer_FUN agpuUnmapBuffer;
	agpuAddShaderReference_FUN agpuAddShaderReference;
	agpuReleaseShader_FUN agpuReleaseShader;
	agpuSetShaderSource_FUN agpuSetShaderSource;
	agpuCompileShader_FUN agpuCompileShader;
	agpuGetShaderCompilationLogLength_FUN agpuGetShaderCompilationLogLength;
	agpuGetShaderCompilationLog_FUN agpuGetShaderCompilationLog;
	agpuAddProgramReference_FUN agpuAddProgramReference;
	agpuReleaseProgram_FUN agpuReleaseProgram;
	agpuAttachShader_FUN agpuAttachShader;
	agpuLinkProgram_FUN agpuLinkProgram;
	agpuGetProgramLinkingLogLength_FUN agpuGetProgramLinkingLogLength;
	agpuGetProgramLinkingLog_FUN agpuGetProgramLinkingLog;
	agpuBindAttributeLocation_FUN agpuBindAttributeLocation;
} agpu_icd_dispatch;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _AGPU_H_ */
