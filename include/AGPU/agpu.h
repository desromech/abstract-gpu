
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
#   if __GNUC__ >= 4
#       define AGPU_EXPORT __attribute__ ((visibility ("default")))
#   endif
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
typedef struct _agpu_swap_chain agpu_swap_chain;
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
typedef struct _agpu_shader_signature_builder agpu_shader_signature_builder;
typedef struct _agpu_shader_signature agpu_shader_signature;
typedef struct _agpu_shader_resource_binding agpu_shader_resource_binding;
typedef struct _agpu_fence agpu_fence;

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
	AGPU_COMMAND_QUEUE_TYPE_GRAPHICS = 0,
	AGPU_COMMAND_QUEUE_TYPE_COMPUTE = 1,
	AGPU_COMMAND_QUEUE_TYPE_TRANSFER = 2,
} agpu_command_queue_type;

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
	AGPU_TEXTURE_UNKNOWN = 0,
	AGPU_TEXTURE_BUFFER = 1,
	AGPU_TEXTURE_1D = 2,
	AGPU_TEXTURE_2D = 3,
	AGPU_TEXTURE_CUBE = 4,
	AGPU_TEXTURE_3D = 5,
} agpu_texture_type;

typedef enum {
	AGPU_TEXTURE_FLAG_NONE = 0,
	AGPU_TEXTURE_FLAG_RENDER_TARGET = 1,
	AGPU_TEXTURE_FLAG_DEPTH = 2,
	AGPU_TEXTURE_FLAG_STENCIL = 4,
	AGPU_TEXTURE_FLAG_UNORDERED_ACCESS = 8,
	AGPU_TEXTURE_FLAG_RENDERBUFFER_ONLY = 16,
	AGPU_TEXTURE_FLAG_READED_BACK = 32,
	AGPU_TEXTURE_FLAG_UPLOADED = 64,
} agpu_texture_flags;

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
	AGPU_GENERIC_DATA_BUFFER = 0,
	AGPU_ARRAY_BUFFER = 1,
	AGPU_ELEMENT_ARRAY_BUFFER = 2,
	AGPU_UNIFORM_BUFFER = 3,
	AGPU_DRAW_INDIRECT_BUFFER = 4,
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
	AGPU_SHADER_BINDING_TYPE_SRV = 0,
	AGPU_SHADER_BINDING_TYPE_UAV = 1,
	AGPU_SHADER_BINDING_TYPE_CBV = 2,
	AGPU_SHADER_BINDING_TYPE_SAMPLER = 3,
	AGPU_SHADER_BINDING_TYPE_COUNT = 4,
} agpu_shader_binding_type;

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

typedef enum {
	AGPU_TEXTURE_FORMAT_UNKNOWN = 0,
	AGPU_TEXTURE_FORMAT_R32G32B32A32_TYPELESS = 1,
	AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT = 2,
	AGPU_TEXTURE_FORMAT_R32G32B32A32_UINT = 3,
	AGPU_TEXTURE_FORMAT_R32G32B32A32_SINT = 4,
	AGPU_TEXTURE_FORMAT_R32G32B32_TYPELESS = 5,
	AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT = 6,
	AGPU_TEXTURE_FORMAT_R32G32B32_UINT = 7,
	AGPU_TEXTURE_FORMAT_R32G32B32_SINT = 8,
	AGPU_TEXTURE_FORMAT_R16G16B16A16_TYPELESS = 9,
	AGPU_TEXTURE_FORMAT_R16G16B16A16_FLOAT = 10,
	AGPU_TEXTURE_FORMAT_R16G16B16A16_UNORM = 11,
	AGPU_TEXTURE_FORMAT_R16G16B16A16_UINT = 12,
	AGPU_TEXTURE_FORMAT_R16G16B16A16_SNORM = 13,
	AGPU_TEXTURE_FORMAT_R16G16B16A16_SINT = 14,
	AGPU_TEXTURE_FORMAT_R32G32_TYPELESS = 15,
	AGPU_TEXTURE_FORMAT_R32G32_FLOAT = 16,
	AGPU_TEXTURE_FORMAT_R32G32_UINT = 17,
	AGPU_TEXTURE_FORMAT_R32G32_SINT = 18,
	AGPU_TEXTURE_FORMAT_R32G8X24_TYPELESS = 19,
	AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	AGPU_TEXTURE_FORMAT_R32_FLOAT_S8X24_TYPELESS = 21,
	AGPU_TEXTURE_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	AGPU_TEXTURE_FORMAT_R10G10B10A2_TYPELESS = 23,
	AGPU_TEXTURE_FORMAT_R10G10B10A2_UNORM = 24,
	AGPU_TEXTURE_FORMAT_R10G10B10A2_UINT = 25,
	AGPU_TEXTURE_FORMAT_R11G11B10A2_FLOAT = 26,
	AGPU_TEXTURE_FORMAT_R8G8B8A8_TYPELESS = 27,
	AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM = 28,
	AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	AGPU_TEXTURE_FORMAT_R8G8B8A8_UINT = 30,
	AGPU_TEXTURE_FORMAT_R8G8B8A8_SNORM = 31,
	AGPU_TEXTURE_FORMAT_R8G8B8A8_SINT = 32,
	AGPU_TEXTURE_FORMAT_R16G16_TYPELESS = 33,
	AGPU_TEXTURE_FORMAT_R16G16_FLOAT = 34,
	AGPU_TEXTURE_FORMAT_R16G16_UNORM = 35,
	AGPU_TEXTURE_FORMAT_R16G16_UINT = 36,
	AGPU_TEXTURE_FORMAT_R16G16_SNORM = 37,
	AGPU_TEXTURE_FORMAT_R16G16_SINT = 38,
	AGPU_TEXTURE_FORMAT_R32_TYPELESS = 39,
	AGPU_TEXTURE_FORMAT_D32_FLOAT = 40,
	AGPU_TEXTURE_FORMAT_R32_FLOAT = 41,
	AGPU_TEXTURE_FORMAT_R32_UINT = 42,
	AGPU_TEXTURE_FORMAT_R32_SINT = 43,
	AGPU_TEXTURE_FORMAT_R24G8_TYPELESS = 44,
	AGPU_TEXTURE_FORMAT_D24_UNORM_S8_UINT = 45,
	AGPU_TEXTURE_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	AGPU_TEXTURE_FORMAT_X24TG8_UINT = 47,
	AGPU_TEXTURE_FORMAT_R8G8_TYPELESS = 48,
	AGPU_TEXTURE_FORMAT_R8G8_UNORM = 49,
	AGPU_TEXTURE_FORMAT_R8G8_UINT = 50,
	AGPU_TEXTURE_FORMAT_R8G8_SNORM = 51,
	AGPU_TEXTURE_FORMAT_R8G8_SINT = 52,
	AGPU_TEXTURE_FORMAT_R16_TYPELESS = 53,
	AGPU_TEXTURE_FORMAT_R16_FLOAT = 54,
	AGPU_TEXTURE_FORMAT_D16_UNORM = 55,
	AGPU_TEXTURE_FORMAT_R16_UNORM = 56,
	AGPU_TEXTURE_FORMAT_R16_UINT = 57,
	AGPU_TEXTURE_FORMAT_R16_SNORM = 58,
	AGPU_TEXTURE_FORMAT_R16_SINT = 59,
	AGPU_TEXTURE_FORMAT_R8_TYPELESS = 60,
	AGPU_TEXTURE_FORMAT_R8_UNORM = 61,
	AGPU_TEXTURE_FORMAT_R8_UINT = 62,
	AGPU_TEXTURE_FORMAT_R8_SNORM = 63,
	AGPU_TEXTURE_FORMAT_R8_SINT = 64,
	AGPU_TEXTURE_FORMAT_A8_UNORM = 65,
	AGPU_TEXTURE_FORMAT_R1_UNORM = 66,
	AGPU_TEXTURE_FORMAT_BC1_TYPELESS = 70,
	AGPU_TEXTURE_FORMAT_BC1_UNORM = 71,
	AGPU_TEXTURE_FORMAT_BC1_UNORM_SRGB = 72,
	AGPU_TEXTURE_FORMAT_BC2_TYPELESS = 73,
	AGPU_TEXTURE_FORMAT_BC2_UNORM = 74,
	AGPU_TEXTURE_FORMAT_BC2_UNORM_SRGB = 75,
	AGPU_TEXTURE_FORMAT_BC3_TYPELESS = 76,
	AGPU_TEXTURE_FORMAT_BC3_UNORM = 77,
	AGPU_TEXTURE_FORMAT_BC3_UNORM_SRGB = 78,
	AGPU_TEXTURE_FORMAT_BC4_TYPELESS = 79,
	AGPU_TEXTURE_FORMAT_BC4_UNORM = 80,
	AGPU_TEXTURE_FORMAT_BC4_SNORM = 81,
	AGPU_TEXTURE_FORMAT_BC5_TYPELESS = 82,
	AGPU_TEXTURE_FORMAT_BC5_UNORM = 83,
	AGPU_TEXTURE_FORMAT_BC5_SNORM = 84,
	AGPU_TEXTURE_FORMAT_B5G6R5_UNORM = 85,
	AGPU_TEXTURE_FORMAT_B5G5R5A1_UNORM = 86,
	AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM = 87,
	AGPU_TEXTURE_FORMAT_B8G8R8X8_UNORM = 88,
	AGPU_TEXTURE_FORMAT_B8G8R8A8_TYPELESS = 90,
	AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	AGPU_TEXTURE_FORMAT_B8G8R8X8_TYPELESS = 92,
	AGPU_TEXTURE_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
} agpu_texture_format;

typedef enum {
	AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST = 0x0,
	AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR = 0x1,
	AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST = 0x4,
	AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR = 0x5,
	AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST = 0x10,
	AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR = 0x11,
	AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST = 0x14,
	AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR = 0x15,
	AGPU_FILTER_ANISOTROPIC = 0x55,
} agpu_filter;

typedef enum {
	AGPU_TEXTURE_ADDRESS_MODE_WRAP = 1,
	AGPU_TEXTURE_ADDRESS_MODE_MIRROR = 2,
	AGPU_TEXTURE_ADDRESS_MODE_CLAMP = 3,
	AGPU_TEXTURE_ADDRESS_MODE_BORDER = 4,
	AGPU_TEXTURE_ADDRESS_MODE_MIRROR_ONCE = 5,
} agpu_texture_address_mode;

typedef enum {
	AGPU_COMMAND_LIST_TYPE_DIRECT = 1,
	AGPU_COMMAND_LIST_TYPE_BUNDLE = 2,
	AGPU_COMMAND_LIST_TYPE_COMPUTE = 3,
	AGPU_COMMAND_LIST_TYPE_COPY = 4,
} agpu_command_list_type;

typedef enum {
	AGPU_BLENDING_ZERO = 1,
	AGPU_BLENDING_ONE = 2,
	AGPU_BLENDING_SRC_COLOR = 3,
	AGPU_BLENDING_INVERTED_SRC_COLOR = 4,
	AGPU_BLENDING_SRC_ALPHA = 5,
	AGPU_BLENDING_INVERTED_SRC_ALPHA = 6,
	AGPU_BLENDING_DEST_ALPHA = 7,
	AGPU_BLENDING_INVERTED_DEST_ALPHA = 8,
	AGPU_BLENDING_DEST_COLOR = 9,
	AGPU_BLENDING_INVERTED_DEST_COLOR = 10,
	AGPU_BLENDING_SRC_ALPHA_SAT = 11,
	AGPU_BLENDING_CONSTANT_FACTOR = 14,
	AGPU_BLENDING_INVERTED_CONSTANT_FACTOR = 15,
	AGPU_BLENDING_SRC_1COLOR = 16,
	AGPU_BLENDING_INVERTED_SRC_1COLOR = 17,
	AGPU_BLENDING_SRC_1ALPHA = 18,
	AGPU_BLENDING_INVERTED_SRC_1ALPHA = 19,
} agpu_blending_factor;

typedef enum {
	AGPU_BLENDING_OPERATION_ADD = 1,
	AGPU_BLENDING_OPERATION_SUBTRACT = 2,
	AGPU_BLENDING_OPERATION_REVERSE_SUBTRACT = 3,
	AGPU_BLENDING_OPERATION_MIN = 4,
	AGPU_BLENDING_OPERATION_MAX = 5,
} agpu_blending_operation;

typedef enum {
	AGPU_COMPONENT_SWIZZLE_IDENTITY = 0,
	AGPU_COMPONENT_SWIZZLE_ONE = 1,
	AGPU_COMPONENT_SWIZZLE_ZERO = 2,
	AGPU_COMPONENT_SWIZZLE_R = 3,
	AGPU_COMPONENT_SWIZZLE_G = 4,
	AGPU_COMPONENT_SWIZZLE_B = 5,
	AGPU_COMPONENT_SWIZZLE_A = 6,
} agpu_component_swizzle;


/* Structure agpu_device_open_info. */
typedef struct agpu_device_open_info {
	agpu_pointer display;
	agpu_bool debug_layer;
	agpu_cstring application_name;
	agpu_uint application_version;
	agpu_cstring engine_name;
	agpu_uint engine_version;
	agpu_int gpu_index;
} agpu_device_open_info;

/* Structure agpu_swap_chain_create_info. */
typedef struct agpu_swap_chain_create_info {
	agpu_pointer window;
	agpu_pointer surface;
	agpu_texture_format colorbuffer_format;
	agpu_texture_format depth_stencil_format;
	agpu_uint width;
	agpu_uint height;
	agpu_uint buffer_count;
	agpu_bool sample_buffers;
	agpu_int samples;
} agpu_swap_chain_create_info;

/* Structure agpu_buffer_description. */
typedef struct agpu_buffer_description {
	agpu_uint size;
	agpu_buffer_usage_type usage;
	agpu_buffer_binding_type binding;
	agpu_bitfield mapping_flags;
	agpu_uint stride;
} agpu_buffer_description;

/* Structure agpu_texture_description. */
typedef struct agpu_texture_description {
	agpu_texture_type type;
	agpu_uint width;
	agpu_uint height;
	agpu_ushort depthOrArraySize;
	agpu_ushort miplevels;
	agpu_texture_format format;
	agpu_texture_flags flags;
	agpu_uint sample_count;
	agpu_uint sample_quality;
} agpu_texture_description;

/* Structure agpu_components_swizzle. */
typedef struct agpu_components_swizzle {
	agpu_component_swizzle r;
	agpu_component_swizzle g;
	agpu_component_swizzle b;
	agpu_component_swizzle a;
} agpu_components_swizzle;

/* Structure agpu_subresource_range. */
typedef struct agpu_subresource_range {
	agpu_texture_flags usage_flags;
	agpu_uint base_miplevel;
	agpu_uint level_count;
	agpu_uint base_arraylayer;
	agpu_uint layer_count;
} agpu_subresource_range;

/* Structure agpu_texture_view_description. */
typedef struct agpu_texture_view_description {
	agpu_texture_type type;
	agpu_texture* texture;
	agpu_texture_format format;
	agpu_components_swizzle components;
	agpu_subresource_range subresource_range;
} agpu_texture_view_description;

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
	agpu_texture_format internal_format;
} agpu_vertex_attrib_description;

/* Structure agpu_color4f. */
typedef struct agpu_color4f {
	agpu_float r;
	agpu_float g;
	agpu_float b;
	agpu_float a;
} agpu_color4f;

/* Structure agpu_sampler_description. */
typedef struct agpu_sampler_description {
	agpu_filter filter;
	agpu_texture_address_mode address_u;
	agpu_texture_address_mode address_v;
	agpu_texture_address_mode address_w;
	agpu_float mip_lod_bias;
	agpu_uint maxanisotropy;
	agpu_compare_function comparison_function;
	agpu_color4f border_color;
	agpu_float min_lod;
	agpu_float max_lod;
} agpu_sampler_description;

/* Global functions. */
typedef agpu_error (*agpuGetPlatforms_FUN) ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms );

AGPU_EXPORT agpu_error agpuGetPlatforms ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms );

/* Methods for interface agpu_platform. */
typedef agpu_device* (*agpuOpenDevice_FUN) ( agpu_platform* platform, agpu_device_open_info* openInfo );
typedef agpu_cstring (*agpuGetPlatformName_FUN) ( agpu_platform* platform );
typedef agpu_int (*agpuGetPlatformVersion_FUN) ( agpu_platform* platform );
typedef agpu_int (*agpuGetPlatformImplementationVersion_FUN) ( agpu_platform* platform );
typedef agpu_bool (*agpuPlatformHasRealMultithreading_FUN) ( agpu_platform* platform );
typedef agpu_bool (*agpuIsNativePlatform_FUN) ( agpu_platform* platform );
typedef agpu_bool (*agpuIsCrossPlatform_FUN) ( agpu_platform* platform );

AGPU_EXPORT agpu_device* agpuOpenDevice ( agpu_platform* platform, agpu_device_open_info* openInfo );
AGPU_EXPORT agpu_cstring agpuGetPlatformName ( agpu_platform* platform );
AGPU_EXPORT agpu_int agpuGetPlatformVersion ( agpu_platform* platform );
AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion ( agpu_platform* platform );
AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading ( agpu_platform* platform );
AGPU_EXPORT agpu_bool agpuIsNativePlatform ( agpu_platform* platform );
AGPU_EXPORT agpu_bool agpuIsCrossPlatform ( agpu_platform* platform );

/* Methods for interface agpu_device. */
typedef agpu_error (*agpuAddDeviceReference_FUN) ( agpu_device* device );
typedef agpu_error (*agpuReleaseDevice_FUN) ( agpu_device* device );
typedef agpu_command_queue* (*agpuGetDefaultCommandQueue_FUN) ( agpu_device* device );
typedef agpu_swap_chain* (*agpuCreateSwapChain_FUN) ( agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo );
typedef agpu_buffer* (*agpuCreateBuffer_FUN) ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data );
typedef agpu_vertex_layout* (*agpuCreateVertexLayout_FUN) ( agpu_device* device );
typedef agpu_vertex_binding* (*agpuCreateVertexBinding_FUN) ( agpu_device* device, agpu_vertex_layout* layout );
typedef agpu_shader* (*agpuCreateShader_FUN) ( agpu_device* device, agpu_shader_type type );
typedef agpu_shader_signature_builder* (*agpuCreateShaderSignatureBuilder_FUN) ( agpu_device* device );
typedef agpu_pipeline_builder* (*agpuCreatePipelineBuilder_FUN) ( agpu_device* device );
typedef agpu_command_allocator* (*agpuCreateCommandAllocator_FUN) ( agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue );
typedef agpu_command_list* (*agpuCreateCommandList_FUN) ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
typedef agpu_shader_language (*agpuGetPreferredShaderLanguage_FUN) ( agpu_device* device );
typedef agpu_shader_language (*agpuGetPreferredHighLevelShaderLanguage_FUN) ( agpu_device* device );
typedef agpu_framebuffer* (*agpuCreateFrameBuffer_FUN) ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorView, agpu_texture_view_description* depthStencilViews );
typedef agpu_texture* (*agpuCreateTexture_FUN) ( agpu_device* device, agpu_texture_description* description );
typedef agpu_fence* (*agpuCreateFence_FUN) ( agpu_device* device );
typedef agpu_int (*agpuGetMultiSampleQualityLevels_FUN) ( agpu_device* device, agpu_uint sample_count );

AGPU_EXPORT agpu_error agpuAddDeviceReference ( agpu_device* device );
AGPU_EXPORT agpu_error agpuReleaseDevice ( agpu_device* device );
AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue ( agpu_device* device );
AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain ( agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo );
AGPU_EXPORT agpu_buffer* agpuCreateBuffer ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data );
AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout ( agpu_device* device );
AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding ( agpu_device* device, agpu_vertex_layout* layout );
AGPU_EXPORT agpu_shader* agpuCreateShader ( agpu_device* device, agpu_shader_type type );
AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder ( agpu_device* device );
AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder ( agpu_device* device );
AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator ( agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue );
AGPU_EXPORT agpu_command_list* agpuCreateCommandList ( agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage ( agpu_device* device );
AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage ( agpu_device* device );
AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer ( agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorView, agpu_texture_view_description* depthStencilViews );
AGPU_EXPORT agpu_texture* agpuCreateTexture ( agpu_device* device, agpu_texture_description* description );
AGPU_EXPORT agpu_fence* agpuCreateFence ( agpu_device* device );
AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels ( agpu_device* device, agpu_uint sample_count );

/* Methods for interface agpu_swap_chain. */
typedef agpu_error (*agpuAddSwapChainReference_FUN) ( agpu_swap_chain* swap_chain );
typedef agpu_error (*agpuReleaseSwapChain_FUN) ( agpu_swap_chain* swap_chain );
typedef agpu_error (*agpuSwapBuffers_FUN) ( agpu_swap_chain* swap_chain );
typedef agpu_framebuffer* (*agpuGetCurrentBackBuffer_FUN) ( agpu_swap_chain* swap_chain );

AGPU_EXPORT agpu_error agpuAddSwapChainReference ( agpu_swap_chain* swap_chain );
AGPU_EXPORT agpu_error agpuReleaseSwapChain ( agpu_swap_chain* swap_chain );
AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_swap_chain* swap_chain );
AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_swap_chain* swap_chain );

/* Methods for interface agpu_pipeline_builder. */
typedef agpu_error (*agpuAddPipelineBuilderReference_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_error (*agpuReleasePipelineBuilder_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_pipeline_state* (*agpuBuildPipelineState_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_error (*agpuAttachShader_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader );
typedef agpu_size (*agpuGetPipelineBuildingLogLength_FUN) ( agpu_pipeline_builder* pipeline_builder );
typedef agpu_error (*agpuGetPipelineBuildingLog_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer );
typedef agpu_error (*agpuSetBlendState_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled );
typedef agpu_error (*agpuSetBlendFunction_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation );
typedef agpu_error (*agpuSetColorMask_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled );
typedef agpu_error (*agpuSetDepthState_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function );
typedef agpu_error (*agpuSetStencilState_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask );
typedef agpu_error (*agpuSetStencilFrontFace_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction );
typedef agpu_error (*agpuSetStencilBackFace_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction );
typedef agpu_error (*agpuSetRenderTargetCount_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_int count );
typedef agpu_error (*agpuSetRenderTargetFormat_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format );
typedef agpu_error (*agpuSetDepthStencilFormat_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_texture_format format );
typedef agpu_error (*agpuSetPrimitiveType_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type );
typedef agpu_error (*agpuSetVertexLayout_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout );
typedef agpu_error (*agpuSetPipelineShaderSignature_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature );
typedef agpu_error (*agpuSetSampleDescription_FUN) ( agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality );

AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_error agpuReleasePipelineBuilder ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_error agpuAttachShader ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader );
AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength ( agpu_pipeline_builder* pipeline_builder );
AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog ( agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer );
AGPU_EXPORT agpu_error agpuSetBlendState ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled );
AGPU_EXPORT agpu_error agpuSetBlendFunction ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation );
AGPU_EXPORT agpu_error agpuSetColorMask ( agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled );
AGPU_EXPORT agpu_error agpuSetDepthState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function );
AGPU_EXPORT agpu_error agpuSetStencilState ( agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask );
AGPU_EXPORT agpu_error agpuSetStencilFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction );
AGPU_EXPORT agpu_error agpuSetStencilBackFace ( agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction );
AGPU_EXPORT agpu_error agpuSetRenderTargetCount ( agpu_pipeline_builder* pipeline_builder, agpu_int count );
AGPU_EXPORT agpu_error agpuSetRenderTargetFormat ( agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format );
AGPU_EXPORT agpu_error agpuSetDepthStencilFormat ( agpu_pipeline_builder* pipeline_builder, agpu_texture_format format );
AGPU_EXPORT agpu_error agpuSetPrimitiveType ( agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type );
AGPU_EXPORT agpu_error agpuSetVertexLayout ( agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout );
AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature ( agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature );
AGPU_EXPORT agpu_error agpuSetSampleDescription ( agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality );

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
typedef agpu_error (*agpuFinishQueueExecution_FUN) ( agpu_command_queue* command_queue );
typedef agpu_error (*agpuSignalFence_FUN) ( agpu_command_queue* command_queue, agpu_fence* fence );
typedef agpu_error (*agpuWaitFence_FUN) ( agpu_command_queue* command_queue, agpu_fence* fence );

AGPU_EXPORT agpu_error agpuAddCommandQueueReference ( agpu_command_queue* command_queue );
AGPU_EXPORT agpu_error agpuReleaseCommandQueue ( agpu_command_queue* command_queue );
AGPU_EXPORT agpu_error agpuAddCommandList ( agpu_command_queue* command_queue, agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuFinishQueueExecution ( agpu_command_queue* command_queue );
AGPU_EXPORT agpu_error agpuSignalFence ( agpu_command_queue* command_queue, agpu_fence* fence );
AGPU_EXPORT agpu_error agpuWaitFence ( agpu_command_queue* command_queue, agpu_fence* fence );

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
typedef agpu_error (*agpuSetShaderSignature_FUN) ( agpu_command_list* command_list, agpu_shader_signature* signature );
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
typedef agpu_error (*agpuSetStencilReference_FUN) ( agpu_command_list* command_list, agpu_uint reference );
typedef agpu_error (*agpuExecuteBundle_FUN) ( agpu_command_list* command_list, agpu_command_list* bundle );
typedef agpu_error (*agpuCloseCommandList_FUN) ( agpu_command_list* command_list );
typedef agpu_error (*agpuResetCommandList_FUN) ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
typedef agpu_error (*agpuBeginFrame_FUN) ( agpu_command_list* command_list, agpu_framebuffer* framebuffer, agpu_bool bundle_content );
typedef agpu_error (*agpuEndFrame_FUN) ( agpu_command_list* command_list );
typedef agpu_error (*agpuResolveFramebuffer_FUN) ( agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer );

AGPU_EXPORT agpu_error agpuAddCommandListReference ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuReleaseCommandList ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuSetShaderSignature ( agpu_command_list* command_list, agpu_shader_signature* signature );
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
AGPU_EXPORT agpu_error agpuSetStencilReference ( agpu_command_list* command_list, agpu_uint reference );
AGPU_EXPORT agpu_error agpuExecuteBundle ( agpu_command_list* command_list, agpu_command_list* bundle );
AGPU_EXPORT agpu_error agpuCloseCommandList ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuResetCommandList ( agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state );
AGPU_EXPORT agpu_error agpuBeginFrame ( agpu_command_list* command_list, agpu_framebuffer* framebuffer, agpu_bool bundle_content );
AGPU_EXPORT agpu_error agpuEndFrame ( agpu_command_list* command_list );
AGPU_EXPORT agpu_error agpuResolveFramebuffer ( agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer );

/* Methods for interface agpu_texture. */
typedef agpu_error (*agpuAddTextureReference_FUN) ( agpu_texture* texture );
typedef agpu_error (*agpuReleaseTexture_FUN) ( agpu_texture* texture );
typedef agpu_error (*agpuGetTextureDescription_FUN) ( agpu_texture* texture, agpu_texture_description* description );
typedef agpu_pointer (*agpuMapTextureLevel_FUN) ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags );
typedef agpu_error (*agpuUnmapTextureLevel_FUN) ( agpu_texture* texture );
typedef agpu_error (*agpuReadTextureData_FUN) ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer );
typedef agpu_error (*agpuUploadTextureData_FUN) ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data );
typedef agpu_error (*agpuDiscardTextureUploadBuffer_FUN) ( agpu_texture* texture );
typedef agpu_error (*agpuDiscardTextureReadbackBuffer_FUN) ( agpu_texture* texture );
typedef agpu_error (*agpuGetTextureFullViewDescription_FUN) ( agpu_texture* texture, agpu_texture_view_description* result );

AGPU_EXPORT agpu_error agpuAddTextureReference ( agpu_texture* texture );
AGPU_EXPORT agpu_error agpuReleaseTexture ( agpu_texture* texture );
AGPU_EXPORT agpu_error agpuGetTextureDescription ( agpu_texture* texture, agpu_texture_description* description );
AGPU_EXPORT agpu_pointer agpuMapTextureLevel ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags );
AGPU_EXPORT agpu_error agpuUnmapTextureLevel ( agpu_texture* texture );
AGPU_EXPORT agpu_error agpuReadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer );
AGPU_EXPORT agpu_error agpuUploadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data );
AGPU_EXPORT agpu_error agpuDiscardTextureUploadBuffer ( agpu_texture* texture );
AGPU_EXPORT agpu_error agpuDiscardTextureReadbackBuffer ( agpu_texture* texture );
AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription ( agpu_texture* texture, agpu_texture_view_description* result );

/* Methods for interface agpu_buffer. */
typedef agpu_error (*agpuAddBufferReference_FUN) ( agpu_buffer* buffer );
typedef agpu_error (*agpuReleaseBuffer_FUN) ( agpu_buffer* buffer );
typedef agpu_pointer (*agpuMapBuffer_FUN) ( agpu_buffer* buffer, agpu_mapping_access flags );
typedef agpu_error (*agpuUnmapBuffer_FUN) ( agpu_buffer* buffer );
typedef agpu_error (*agpuGetBufferDescription_FUN) ( agpu_buffer* buffer, agpu_buffer_description* description );
typedef agpu_error (*agpuUploadBufferData_FUN) ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );
typedef agpu_error (*agpuReadBufferData_FUN) ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer );
AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer );
AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags );
AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer );
AGPU_EXPORT agpu_error agpuGetBufferDescription ( agpu_buffer* buffer, agpu_buffer_description* description );
AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );
AGPU_EXPORT agpu_error agpuReadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data );

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
typedef agpu_error (*agpuAddVertexAttributeBindings_FUN) ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes );

AGPU_EXPORT agpu_error agpuAddVertexLayoutReference ( agpu_vertex_layout* vertex_layout );
AGPU_EXPORT agpu_error agpuReleaseVertexLayout ( agpu_vertex_layout* vertex_layout );
AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings ( agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes );

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

/* Methods for interface agpu_framebuffer. */
typedef agpu_error (*agpuAddFramebufferReference_FUN) ( agpu_framebuffer* framebuffer );
typedef agpu_error (*agpuReleaseFramebuffer_FUN) ( agpu_framebuffer* framebuffer );

AGPU_EXPORT agpu_error agpuAddFramebufferReference ( agpu_framebuffer* framebuffer );
AGPU_EXPORT agpu_error agpuReleaseFramebuffer ( agpu_framebuffer* framebuffer );

/* Methods for interface agpu_shader_signature_builder. */
typedef agpu_error (*agpuAddShaderSignatureBuilderReference_FUN) ( agpu_shader_signature_builder* shader_signature_builder );
typedef agpu_error (*agpuReleaseShaderSignatureBuilder_FUN) ( agpu_shader_signature_builder* shader_signature_builder );
typedef agpu_shader_signature* (*agpuBuildShaderSignature_FUN) ( agpu_shader_signature_builder* shader_signature_builder );
typedef agpu_error (*agpuAddShaderSignatureBindingConstant_FUN) ( agpu_shader_signature_builder* shader_signature_builder );
typedef agpu_error (*agpuAddShaderSignatureBindingElement_FUN) ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings );
typedef agpu_error (*agpuAddShaderSignatureBindingBank_FUN) ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings );

AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference ( agpu_shader_signature_builder* shader_signature_builder );
AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder ( agpu_shader_signature_builder* shader_signature_builder );
AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature ( agpu_shader_signature_builder* shader_signature_builder );
AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant ( agpu_shader_signature_builder* shader_signature_builder );
AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings );
AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBank ( agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings );

/* Methods for interface agpu_shader_signature. */
typedef agpu_error (*agpuAddShaderSignature_FUN) ( agpu_shader_signature* shader_signature );
typedef agpu_error (*agpuReleaseShaderSignature_FUN) ( agpu_shader_signature* shader_signature );
typedef agpu_shader_resource_binding* (*agpuCreateShaderResourceBinding_FUN) ( agpu_shader_signature* shader_signature, agpu_uint element );

AGPU_EXPORT agpu_error agpuAddShaderSignature ( agpu_shader_signature* shader_signature );
AGPU_EXPORT agpu_error agpuReleaseShaderSignature ( agpu_shader_signature* shader_signature );
AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding ( agpu_shader_signature* shader_signature, agpu_uint element );

/* Methods for interface agpu_shader_resource_binding. */
typedef agpu_error (*agpuAddShaderResourceBindingReference_FUN) ( agpu_shader_resource_binding* shader_resource_binding );
typedef agpu_error (*agpuReleaseShaderResourceBinding_FUN) ( agpu_shader_resource_binding* shader_resource_binding );
typedef agpu_error (*agpuBindUniformBuffer_FUN) ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer );
typedef agpu_error (*agpuBindUniformBufferRange_FUN) ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
typedef agpu_error (*agpuBindTexture_FUN) ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp );
typedef agpu_error (*agpuBindTextureArrayRange_FUN) ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp );
typedef agpu_error (*agpuCreateSampler_FUN) ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler_description* description );

AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference ( agpu_shader_resource_binding* shader_resource_binding );
AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding ( agpu_shader_resource_binding* shader_resource_binding );
AGPU_EXPORT agpu_error agpuBindUniformBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer );
AGPU_EXPORT agpu_error agpuBindUniformBufferRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
AGPU_EXPORT agpu_error agpuBindTexture ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp );
AGPU_EXPORT agpu_error agpuBindTextureArrayRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp );
AGPU_EXPORT agpu_error agpuCreateSampler ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler_description* description );

/* Methods for interface agpu_fence. */
typedef agpu_error (*agpuAddFenceReference_FUN) ( agpu_fence* fence );
typedef agpu_error (*agpuReleaseFenceReference_FUN) ( agpu_fence* fence );
typedef agpu_error (*agpuWaitOnClient_FUN) ( agpu_fence* fence );

AGPU_EXPORT agpu_error agpuAddFenceReference ( agpu_fence* fence );
AGPU_EXPORT agpu_error agpuReleaseFenceReference ( agpu_fence* fence );
AGPU_EXPORT agpu_error agpuWaitOnClient ( agpu_fence* fence );

/* Installable client driver interface. */
typedef struct _agpu_icd_dispatch {
	int icd_interface_version;
	agpuGetPlatforms_FUN agpuGetPlatforms;
	agpuOpenDevice_FUN agpuOpenDevice;
	agpuGetPlatformName_FUN agpuGetPlatformName;
	agpuGetPlatformVersion_FUN agpuGetPlatformVersion;
	agpuGetPlatformImplementationVersion_FUN agpuGetPlatformImplementationVersion;
	agpuPlatformHasRealMultithreading_FUN agpuPlatformHasRealMultithreading;
	agpuIsNativePlatform_FUN agpuIsNativePlatform;
	agpuIsCrossPlatform_FUN agpuIsCrossPlatform;
	agpuAddDeviceReference_FUN agpuAddDeviceReference;
	agpuReleaseDevice_FUN agpuReleaseDevice;
	agpuGetDefaultCommandQueue_FUN agpuGetDefaultCommandQueue;
	agpuCreateSwapChain_FUN agpuCreateSwapChain;
	agpuCreateBuffer_FUN agpuCreateBuffer;
	agpuCreateVertexLayout_FUN agpuCreateVertexLayout;
	agpuCreateVertexBinding_FUN agpuCreateVertexBinding;
	agpuCreateShader_FUN agpuCreateShader;
	agpuCreateShaderSignatureBuilder_FUN agpuCreateShaderSignatureBuilder;
	agpuCreatePipelineBuilder_FUN agpuCreatePipelineBuilder;
	agpuCreateCommandAllocator_FUN agpuCreateCommandAllocator;
	agpuCreateCommandList_FUN agpuCreateCommandList;
	agpuGetPreferredShaderLanguage_FUN agpuGetPreferredShaderLanguage;
	agpuGetPreferredHighLevelShaderLanguage_FUN agpuGetPreferredHighLevelShaderLanguage;
	agpuCreateFrameBuffer_FUN agpuCreateFrameBuffer;
	agpuCreateTexture_FUN agpuCreateTexture;
	agpuCreateFence_FUN agpuCreateFence;
	agpuGetMultiSampleQualityLevels_FUN agpuGetMultiSampleQualityLevels;
	agpuAddSwapChainReference_FUN agpuAddSwapChainReference;
	agpuReleaseSwapChain_FUN agpuReleaseSwapChain;
	agpuSwapBuffers_FUN agpuSwapBuffers;
	agpuGetCurrentBackBuffer_FUN agpuGetCurrentBackBuffer;
	agpuAddPipelineBuilderReference_FUN agpuAddPipelineBuilderReference;
	agpuReleasePipelineBuilder_FUN agpuReleasePipelineBuilder;
	agpuBuildPipelineState_FUN agpuBuildPipelineState;
	agpuAttachShader_FUN agpuAttachShader;
	agpuGetPipelineBuildingLogLength_FUN agpuGetPipelineBuildingLogLength;
	agpuGetPipelineBuildingLog_FUN agpuGetPipelineBuildingLog;
	agpuSetBlendState_FUN agpuSetBlendState;
	agpuSetBlendFunction_FUN agpuSetBlendFunction;
	agpuSetColorMask_FUN agpuSetColorMask;
	agpuSetDepthState_FUN agpuSetDepthState;
	agpuSetStencilState_FUN agpuSetStencilState;
	agpuSetStencilFrontFace_FUN agpuSetStencilFrontFace;
	agpuSetStencilBackFace_FUN agpuSetStencilBackFace;
	agpuSetRenderTargetCount_FUN agpuSetRenderTargetCount;
	agpuSetRenderTargetFormat_FUN agpuSetRenderTargetFormat;
	agpuSetDepthStencilFormat_FUN agpuSetDepthStencilFormat;
	agpuSetPrimitiveType_FUN agpuSetPrimitiveType;
	agpuSetVertexLayout_FUN agpuSetVertexLayout;
	agpuSetPipelineShaderSignature_FUN agpuSetPipelineShaderSignature;
	agpuSetSampleDescription_FUN agpuSetSampleDescription;
	agpuAddPipelineStateReference_FUN agpuAddPipelineStateReference;
	agpuReleasePipelineState_FUN agpuReleasePipelineState;
	agpuGetUniformLocation_FUN agpuGetUniformLocation;
	agpuAddCommandQueueReference_FUN agpuAddCommandQueueReference;
	agpuReleaseCommandQueue_FUN agpuReleaseCommandQueue;
	agpuAddCommandList_FUN agpuAddCommandList;
	agpuFinishQueueExecution_FUN agpuFinishQueueExecution;
	agpuSignalFence_FUN agpuSignalFence;
	agpuWaitFence_FUN agpuWaitFence;
	agpuAddCommandAllocatorReference_FUN agpuAddCommandAllocatorReference;
	agpuReleaseCommandAllocator_FUN agpuReleaseCommandAllocator;
	agpuResetCommandAllocator_FUN agpuResetCommandAllocator;
	agpuAddCommandListReference_FUN agpuAddCommandListReference;
	agpuReleaseCommandList_FUN agpuReleaseCommandList;
	agpuSetShaderSignature_FUN agpuSetShaderSignature;
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
	agpuExecuteBundle_FUN agpuExecuteBundle;
	agpuCloseCommandList_FUN agpuCloseCommandList;
	agpuResetCommandList_FUN agpuResetCommandList;
	agpuBeginFrame_FUN agpuBeginFrame;
	agpuEndFrame_FUN agpuEndFrame;
	agpuResolveFramebuffer_FUN agpuResolveFramebuffer;
	agpuAddTextureReference_FUN agpuAddTextureReference;
	agpuReleaseTexture_FUN agpuReleaseTexture;
	agpuGetTextureDescription_FUN agpuGetTextureDescription;
	agpuMapTextureLevel_FUN agpuMapTextureLevel;
	agpuUnmapTextureLevel_FUN agpuUnmapTextureLevel;
	agpuReadTextureData_FUN agpuReadTextureData;
	agpuUploadTextureData_FUN agpuUploadTextureData;
	agpuDiscardTextureUploadBuffer_FUN agpuDiscardTextureUploadBuffer;
	agpuDiscardTextureReadbackBuffer_FUN agpuDiscardTextureReadbackBuffer;
	agpuGetTextureFullViewDescription_FUN agpuGetTextureFullViewDescription;
	agpuAddBufferReference_FUN agpuAddBufferReference;
	agpuReleaseBuffer_FUN agpuReleaseBuffer;
	agpuMapBuffer_FUN agpuMapBuffer;
	agpuUnmapBuffer_FUN agpuUnmapBuffer;
	agpuGetBufferDescription_FUN agpuGetBufferDescription;
	agpuUploadBufferData_FUN agpuUploadBufferData;
	agpuReadBufferData_FUN agpuReadBufferData;
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
	agpuAddFramebufferReference_FUN agpuAddFramebufferReference;
	agpuReleaseFramebuffer_FUN agpuReleaseFramebuffer;
	agpuAddShaderSignatureBuilderReference_FUN agpuAddShaderSignatureBuilderReference;
	agpuReleaseShaderSignatureBuilder_FUN agpuReleaseShaderSignatureBuilder;
	agpuBuildShaderSignature_FUN agpuBuildShaderSignature;
	agpuAddShaderSignatureBindingConstant_FUN agpuAddShaderSignatureBindingConstant;
	agpuAddShaderSignatureBindingElement_FUN agpuAddShaderSignatureBindingElement;
	agpuAddShaderSignatureBindingBank_FUN agpuAddShaderSignatureBindingBank;
	agpuAddShaderSignature_FUN agpuAddShaderSignature;
	agpuReleaseShaderSignature_FUN agpuReleaseShaderSignature;
	agpuCreateShaderResourceBinding_FUN agpuCreateShaderResourceBinding;
	agpuAddShaderResourceBindingReference_FUN agpuAddShaderResourceBindingReference;
	agpuReleaseShaderResourceBinding_FUN agpuReleaseShaderResourceBinding;
	agpuBindUniformBuffer_FUN agpuBindUniformBuffer;
	agpuBindUniformBufferRange_FUN agpuBindUniformBufferRange;
	agpuBindTexture_FUN agpuBindTexture;
	agpuBindTextureArrayRange_FUN agpuBindTextureArrayRange;
	agpuCreateSampler_FUN agpuCreateSampler;
	agpuAddFenceReference_FUN agpuAddFenceReference;
	agpuReleaseFenceReference_FUN agpuReleaseFenceReference;
	agpuWaitOnClient_FUN agpuWaitOnClient;
} agpu_icd_dispatch;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _AGPU_H_ */
