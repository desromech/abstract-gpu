
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
	AGPU_INVALID_OPERATION = -1,
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
	AGPU_ARRAY_BUFFER = 0,
	AGPU_ELEMENT_ARRAY_BUFFER = 1,
	AGPU_UNIFORM_BUFFER = 2,
	AGPU_DRAW_INDIRECT_BUFFER = 3,
} agpu_buffer_binding_type;

typedef enum {
	AGPU_DEPTH_BUFFER_BIT = 1,
	AGPU_STENCIL_BUFFER_BIT = 2,
	AGPU_COLOR_BUFFER_BIT = 4,
} agpu_render_buffer_bit;


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

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device* device );
AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device* device );
AGPU_EXPORT agpu_context* agpuGetImmediateContext ( agpu_device* device );
AGPU_EXPORT agpu_context* agpuCreateDeferredContext ( agpu_device* device );
AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_device* device );

/* Methods for interface agpu_context. */
typedef agpu_error (*agpuAddContextReference_FUN) ( agpu_context* context );
typedef agpu_error (*agpuReleaseContext_FUN) ( agpu_context* context );
typedef agpu_error (*agpuSetClearColor_FUN) ( agpu_context* context, agpu_float r, agpu_float g, agpu_float b, agpu_float a );
typedef agpu_error (*agpuSetClearDepth_FUN) ( agpu_context* context, agpu_float depth );
typedef agpu_error (*agpuSetClearStencil_FUN) ( agpu_context* context, agpu_int value );
typedef agpu_error (*agpuClear_FUN) ( agpu_context* context, agpu_render_buffer_bit buffers );
typedef agpu_error (*agpuSetDepthFunction_FUN) ( agpu_context* context, agpu_compare_function function );
typedef agpu_error (*agpuSetAlphaFunction_FUN) ( agpu_context* context, agpu_compare_function function, agpu_float reference );

AGPU_EXPORT agpu_error agpuAddContextReference ( agpu_context* context );
AGPU_EXPORT agpu_error agpuReleaseContext ( agpu_context* context );
AGPU_EXPORT agpu_error agpuSetClearColor ( agpu_context* context, agpu_float r, agpu_float g, agpu_float b, agpu_float a );
AGPU_EXPORT agpu_error agpuSetClearDepth ( agpu_context* context, agpu_float depth );
AGPU_EXPORT agpu_error agpuSetClearStencil ( agpu_context* context, agpu_int value );
AGPU_EXPORT agpu_error agpuClear ( agpu_context* context, agpu_render_buffer_bit buffers );
AGPU_EXPORT agpu_error agpuSetDepthFunction ( agpu_context* context, agpu_compare_function function );
AGPU_EXPORT agpu_error agpuSetAlphaFunction ( agpu_context* context, agpu_compare_function function, agpu_float reference );

/* Methods for interface agpu_texture. */


/* Methods for interface agpu_buffer. */


/* Methods for interface agpu_shader. */


/* Methods for interface agpu_program. */


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
	agpuAddContextReference_FUN agpuAddContextReference;
	agpuReleaseContext_FUN agpuReleaseContext;
	agpuSetClearColor_FUN agpuSetClearColor;
	agpuSetClearDepth_FUN agpuSetClearDepth;
	agpuSetClearStencil_FUN agpuSetClearStencil;
	agpuClear_FUN agpuClear;
	agpuSetDepthFunction_FUN agpuSetDepthFunction;
	agpuSetAlphaFunction_FUN agpuSetAlphaFunction;
} agpu_icd_dispatch;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _AGPU_H_ */
