
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
typedef unsigned int agpu_size;
typedef int agpu_enum;
typedef int agpu_bool;
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
typedef struct _agpu_vr_system agpu_vr_system;
typedef struct _agpu_swap_chain agpu_swap_chain;
typedef struct _agpu_compute_pipeline_builder agpu_compute_pipeline_builder;
typedef struct _agpu_pipeline_builder agpu_pipeline_builder;
typedef struct _agpu_pipeline_state agpu_pipeline_state;
typedef struct _agpu_command_queue agpu_command_queue;
typedef struct _agpu_command_allocator agpu_command_allocator;
typedef struct _agpu_command_list agpu_command_list;
typedef struct _agpu_texture agpu_texture;
typedef struct _agpu_texture_view agpu_texture_view;
typedef struct _agpu_sampler agpu_sampler;
typedef struct _agpu_buffer agpu_buffer;
typedef struct _agpu_vertex_binding agpu_vertex_binding;
typedef struct _agpu_vertex_layout agpu_vertex_layout;
typedef struct _agpu_shader agpu_shader;
typedef struct _agpu_framebuffer agpu_framebuffer;
typedef struct _agpu_renderpass agpu_renderpass;
typedef struct _agpu_shader_signature_builder agpu_shader_signature_builder;
typedef struct _agpu_shader_signature agpu_shader_signature;
typedef struct _agpu_shader_resource_binding agpu_shader_resource_binding;
typedef struct _agpu_fence agpu_fence;
typedef struct _agpu_offline_shader_compiler agpu_offline_shader_compiler;
typedef struct _agpu_state_tracker_cache agpu_state_tracker_cache;
typedef struct _agpu_state_tracker agpu_state_tracker;
typedef struct _agpu_immediate_renderer agpu_immediate_renderer;

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
	AGPU_LINKING_ERROR = -10,
	AGPU_COMMAND_LIST_CLOSED = -11,
	AGPU_OUT_OF_MEMORY = -12,
} agpu_error;

typedef enum {
	AGPU_DEVICE_OPEN_FLAG_NONE = 0,
	AGPU_DEVICE_OPEN_FLAG_ALLOW_VR = 1,
} agpu_device_open_flags;

typedef enum {
	AGPU_SWAP_CHAIN_FLAG_NONE = 0,
	AGPU_SWAP_CHAIN_FLAG_OVERLAY_WINDOW = 1,
} agpu_swap_chain_flags;

typedef enum {
	AGPU_COMMAND_QUEUE_TYPE_GRAPHICS = 0,
	AGPU_COMMAND_QUEUE_TYPE_COMPUTE = 1,
	AGPU_COMMAND_QUEUE_TYPE_TRANSFER = 2,
} agpu_command_queue_type;

typedef enum {
	AGPU_ACCESS_INDIRECT_COMMAND_READ = 1,
	AGPU_ACCESS_INDEX_READ = 2,
	AGPU_ACCESS_VERTEX_ATTRIBUTE_READ = 4,
	AGPU_ACCESS_UNIFORM_READ = 8,
	AGPU_ACCESS_INPUT_ATTACHMENT_READ = 16,
	AGPU_ACCESS_SHADER_READ = 32,
	AGPU_ACCESS_SHADER_WRITE = 64,
	AGPU_ACCESS_COLOR_ATTACHMENT_READ = 128,
	AGPU_ACCESS_COLOR_ATTACHMENT_WRITE = 256,
	AGPU_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ = 512,
	AGPU_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = 1024,
	AGPU_ACCESS_TRANSFER_READ = 2048,
	AGPU_ACCESS_TRANSFER_WRITE = 4096,
	AGPU_ACCESS_HOST_READ = 8192,
	AGPU_ACCESS_HOST_WRITE = 16384,
	AGPU_ACCESS_MEMORY_READ = 32768,
	AGPU_ACCESS_MEMORY_WRITE = 65536,
	AGPU_ACCESS_TRANSFORM_FEEDBACK_WRITE = 33554432,
	AGPU_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ = 67108864,
	AGPU_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE = 134217728,
} agpu_access_flags;

typedef enum {
	AGPU_PIPELINE_STAGE_TOP_OF_PIPE = 1,
	AGPU_PIPELINE_STAGE_DRAW_INDIRECT = 2,
	AGPU_PIPELINE_STAGE_VERTEX_INPUT = 4,
	AGPU_PIPELINE_STAGE_VERTEX_SHADER = 8,
	AGPU_PIPELINE_STAGE_TESSELLATION_CONTROL = 16,
	AGPU_PIPELINE_STAGE_TESSELLATION_EVALUATION = 32,
	AGPU_PIPELINE_STAGE_GEOMETRY_SHADER = 64,
	AGPU_PIPELINE_STAGE_FRAGMENT_SHADER = 128,
	AGPU_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 256,
	AGPU_PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 512,
	AGPU_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 1024,
	AGPU_PIPELINE_STAGE_COMPUTE_SHADER = 2048,
	AGPU_PIPELINE_STAGE_TRANSFER = 4096,
	AGPU_PIPELINE_STAGE_BOTTOM_OF_PIPE = 8192,
	AGPU_PIPELINE_STAGE_HOST = 16384,
	AGPU_PIPELINE_STAGE_ALL_GRAPHICS = 32768,
	AGPU_PIPELINE_STAGE_ALL_COMMANDS = 65536,
	AGPU_PIPELINE_STAGE_TRANSFORM_FEEDBACK = 16777216,
	AGPU_PIPELINE_STAGE_CONDITIONAL_RENDERING = 262144,
} agpu_pipeline_stage_flags;

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
	AGPU_IMMEDIATE_TRIANGLE_FAN = 100,
	AGPU_IMMEDIATE_QUADS = 101,
	AGPU_IMMEDIATE_POLYGON = 102,
} agpu_primitive_topology;

typedef enum {
	AGPU_POLYGON_MODE_FILL = 0,
	AGPU_POLYGON_MODE_LINE = 1,
	AGPU_POLYGON_MODE_POINT = 2,
} agpu_polygon_mode;

typedef enum {
	AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING = 1,
	AGPU_FEATURE_COHERENT_MEMORY_MAPPING = 2,
	AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING = 3,
	AGPU_FEATURE_COMMAND_LIST_REUSE = 4,
	AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE = 5,
	AGPU_FEATURE_VRDISPLAY = 6,
	AGPU_FEATURE_VRINPUT_DEVICES = 7,
} agpu_feature;

typedef enum {
	AGPU_ATTACHMENT_KEEP = 0,
	AGPU_ATTACHMENT_CLEAR = 1,
	AGPU_ATTACHMENT_DISCARD = 2,
} agpu_renderpass_attachment_action;

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
	AGPU_TEXTURE_USAGE_NONE = 0,
	AGPU_TEXTURE_USAGE_SAMPLED = 1,
	AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT = 2,
	AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT = 4,
	AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT = 8,
	AGPU_TEXTURE_USAGE_STORAGE = 16,
	AGPU_TEXTURE_USAGE_COPY_SOURCE = 32,
	AGPU_TEXTURE_USAGE_COPY_DESTINATION = 64,
	AGPU_TEXTURE_USAGE_READED_BACK = 32,
	AGPU_TEXTURE_USAGE_UPLOADED = 64,
	AGPU_TEXTURE_USAGE_PRESENT = 128,
} agpu_texture_usage_mode_mask;

typedef enum {
	AGPU_VERTEX_SHADER = 0,
	AGPU_FRAGMENT_SHADER = 1,
	AGPU_GEOMETRY_SHADER = 2,
	AGPU_COMPUTE_SHADER = 3,
	AGPU_TESSELLATION_CONTROL_SHADER = 4,
	AGPU_TESSELLATION_EVALUATION_SHADER = 5,
	AGPU_LIBRARY_SHADER = 6,
} agpu_shader_type;

typedef enum {
	AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL = 0,
	AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE = 1,
	AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST = 2,
	AGPU_MEMORY_HEAP_TYPE_HOST = 3,
	AGPU_MEMORY_HEAP_TYPE_CUSTOM = 4,
	AGPU_MEMORY_HEAP_TYPE_STAGING_UPLOAD = 3,
	AGPU_STATIC = 0,
	AGPU_DYNAMIC = 1,
	AGPU_STREAM = 1,
} agpu_memory_heap_type;

typedef enum {
	AGPU_COPY_DESTINATION_BUFFER = 1,
	AGPU_COPY_SOURCE_BUFFER = 2,
	AGPU_GENERIC_DATA_BUFFER = 3,
	AGPU_ARRAY_BUFFER = 4,
	AGPU_ELEMENT_ARRAY_BUFFER = 8,
	AGPU_UNIFORM_BUFFER = 16,
	AGPU_DRAW_INDIRECT_BUFFER = 32,
	AGPU_STORAGE_BUFFER = 64,
	AGPU_UNIFORM_TEXEL_BUFFER = 128,
	AGPU_STORAGE_TEXEL_BUFFER = 256,
	AGPU_COMPUTE_DISPATCH_INDIRECT_BUFFER = 512,
} agpu_buffer_usage_mask;

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
	AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE = 0,
	AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE = 1,
	AGPU_SHADER_BINDING_TYPE_UNIFORM_TEXEL_BUFFER = 2,
	AGPU_SHADER_BINDING_TYPE_STORAGE_TEXEL_BUFFER = 3,
	AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER = 4,
	AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER = 5,
	AGPU_SHADER_BINDING_TYPE_SAMPLER = 6,
	AGPU_SHADER_BINDING_TYPE_COUNT = 7,
} agpu_shader_binding_type;

typedef enum {
	AGPU_SHADER_LANGUAGE_NONE = 0,
	AGPU_SHADER_LANGUAGE_GLSL = 1,
	AGPU_SHADER_LANGUAGE_EGLSL = 2,
	AGPU_SHADER_LANGUAGE_VGLSL = 3,
	AGPU_SHADER_LANGUAGE_SPIR_V = 4,
	AGPU_SHADER_LANGUAGE_HLSL = 5,
	AGPU_SHADER_LANGUAGE_METAL = 6,
	AGPU_SHADER_LANGUAGE_METAL_AIR = 7,
	AGPU_SHADER_LANGUAGE_BINARY = 8,
	AGPU_SHADER_LANGUAGE_SPIR_VASSEMBLY = 9,
	AGPU_SHADER_LANGUAGE_DEVICE_SHADER = 10,
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
	AGPU_TEXTURE_FORMAT_R11G11B10_FLOAT = 26,
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
	AGPU_TEXTURE_FORMAT_ETC2_R8G8B8_UNORM = 200,
	AGPU_TEXTURE_FORMAT_ETC2_R8G8B8_UNORM_SRGB = 201,
	AGPU_TEXTURE_FORMAT_ETC2_R8G8B8A1_UNORM = 202,
	AGPU_TEXTURE_FORMAT_ETC2_R8G8B8A1_UNORM_SRGB = 203,
	AGPU_TEXTURE_FORMAT_ETC2_R8G8B8A8_UNORM = 204,
	AGPU_TEXTURE_FORMAT_ETC2_R8G8B8A8_UNORM_SRGB = 205,
	AGPU_TEXTURE_FORMAT_EAC_R11_UNORM = 206,
	AGPU_TEXTURE_FORMAT_EAC_R11_SNORM = 207,
	AGPU_TEXTURE_FORMAT_EAC_R11G11_UNORM = 208,
	AGPU_TEXTURE_FORMAT_EAC_R11G11_SNORM = 209,
	AGPU_TEXTURE_FORMAT_ASTC4X4_UNORM = 230,
	AGPU_TEXTURE_FORMAT_ASTC4X4_UNORM_SRGB = 231,
	AGPU_TEXTURE_FORMAT_ASTC5X4_UNORM = 232,
	AGPU_TEXTURE_FORMAT_ASTC5X4_UNORM_SRGB = 233,
	AGPU_TEXTURE_FORMAT_ASTC5X5_UNORM = 234,
	AGPU_TEXTURE_FORMAT_ASTC5X5_UNORM_SRGB = 235,
	AGPU_TEXTURE_FORMAT_ASTC6X5_UNORM = 236,
	AGPU_TEXTURE_FORMAT_ASTC6X5_UNORM_SRGB = 237,
	AGPU_TEXTURE_FORMAT_ASTC6X6_UNORM = 238,
	AGPU_TEXTURE_FORMAT_ASTC6X6_UNORM_SRGB = 239,
	AGPU_TEXTURE_FORMAT_ASTC8X5_UNORM = 240,
	AGPU_TEXTURE_FORMAT_ASTC8X5_UNORM_SRGB = 241,
	AGPU_TEXTURE_FORMAT_ASTC8X6_UNORM = 242,
	AGPU_TEXTURE_FORMAT_ASTC8X6_UNORM_SRGB = 243,
	AGPU_TEXTURE_FORMAT_ASTC8X8_UNORM = 244,
	AGPU_TEXTURE_FORMAT_ASTC8X8_UNORM_SRGB = 245,
	AGPU_TEXTURE_FORMAT_ASTC10X5_UNORM = 246,
	AGPU_TEXTURE_FORMAT_ASTC10X5_UNORM_SRGB = 247,
	AGPU_TEXTURE_FORMAT_ASTC10X6_UNORM = 248,
	AGPU_TEXTURE_FORMAT_ASTC10X6_UNORM_SRGB = 249,
	AGPU_TEXTURE_FORMAT_ASTC10X8_UNORM = 250,
	AGPU_TEXTURE_FORMAT_ASTC10X8_UNORM_SRGB = 251,
	AGPU_TEXTURE_FORMAT_ASTC10X10_UNORM = 252,
	AGPU_TEXTURE_FORMAT_ASTC10X10_UNORM_SRGB = 253,
	AGPU_TEXTURE_FORMAT_ASTC12X10_UNORM = 254,
	AGPU_TEXTURE_FORMAT_ASTC12X10_UNORM_SRGB = 255,
	AGPU_TEXTURE_FORMAT_ASTC12X12_UNORM = 256,
	AGPU_TEXTURE_FORMAT_ASTC12X12_UNORM_SRGB = 257,
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
	AGPU_TEXTURE_ASPECT_COLOR = 1,
	AGPU_TEXTURE_ASPECT_DEPTH = 2,
	AGPU_TEXTURE_ASPECT_STENCIL = 4,
	AGPU_TEXTURE_ASPECT_ALL = -1,
} agpu_texture_aspect;

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

typedef enum {
	AGPU_COUNTER_CLOCKWISE = 0,
	AGPU_CLOCKWISE = 1,
} agpu_face_winding;

typedef enum {
	AGPU_CULL_MODE_NONE = 0,
	AGPU_CULL_MODE_FRONT = 1,
	AGPU_CULL_MODE_BACK = 2,
	AGPU_CULL_MODE_FRONT_AND_BACK = 3,
} agpu_cull_mode;

typedef enum {
	AGPU_VR_EYE_LEFT = 0,
	AGPU_VR_EYE_RIGHT = 1,
} agpu_vr_eye;

typedef enum {
	AGPU_VR_TRACKED_DEVICE_CLASS_INVALID = 0,
	AGPU_VR_TRACKED_DEVICE_CLASS_HMD = 1,
	AGPU_VR_TRACKED_DEVICE_CLASS_CONTROLLER = 2,
	AGPU_VR_TRACKED_DEVICE_CLASS_GENERIC_TRACKER = 3,
	AGPU_VR_TRACKED_DEVICE_CLASS_TRACKING_REFERENCE = 4,
	AGPU_VR_TRACKED_DEVICE_CLASS_DISPLAY_REDIRECT = 5,
} agpu_vr_tracked_device_class;

typedef enum {
	AGPU_VR_TRACKED_DEVICE_ROLE_INVALID = 0,
	AGPU_VR_TRACKED_DEVICE_ROLE_LEFT_HAND = 1,
	AGPU_VR_TRACKED_DEVICE_ROLE_RIGHT_HAND = 2,
	AGPU_VR_TRACKED_DEVICE_ROLE_OPT_OUT = 3,
	AGPU_VR_TRACKED_DEVICE_ROLE_THREADMILL = 4,
} agpu_vr_tracked_device_role;

typedef enum {
	AGPU_VR_BUTTON_SYSTEM = 0,
	AGPU_VR_BUTTON_APPLICATION_MENU = 1,
	AGPU_VR_BUTTON_GRIP = 2,
	AGPU_VR_BUTTON_DPAD_LEFT = 3,
	AGPU_VR_BUTTON_DPAD_UP = 4,
	AGPU_VR_BUTTON_DPAD_RIGHT = 5,
	AGPU_VR_BUTTON_DPAD_DOWN = 6,
	AGPU_VR_BUTTON_A = 7,
	AGPU_VR_BUTTON_PROXIMITY_SENSOR = 31,
	AGPU_VR_BUTTON_AXIS_0 = 32,
	AGPU_VR_BUTTON_AXIS_1 = 33,
	AGPU_VR_BUTTON_AXIS_2 = 34,
	AGPU_VR_BUTTON_AXIS_3 = 35,
	AGPU_VR_BUTTON_AXIS_4 = 36,
	AGPU_VR_BUTTON_STEAM_VR_TOUCHPAD = 32,
	AGPU_VR_BUTTON_STEAM_VR_TRIGGER = 33,
	AGPU_VR_BUTTON_DASHBOARD_BACK = 2,
	AGPU_VR_BUTTON_KNUCKLES_A = 2,
	AGPU_VR_BUTTON_KNUCKLES_B = 1,
	AGPU_VR_BUTTON_KNUCKLES_JOY_STICK = 35,
} agpu_vr_button;

typedef enum {
	AGPU_VR_DUAL_ANALOG_LEFT = 0,
	AGPU_VR_DUAL_ANALOG_RIGHT = 1,
} agpu_vr_dual_analog_which;

typedef enum {
	AGPU_VR_EVENT_TYPE_INVALID = 0,
	AGPU_VR_EVENT_TYPE_TRACKED_DEVICE_ACTIVATED = 100,
	AGPU_VR_EVENT_TYPE_TRACKED_DEVICE_DEACTIVATED = 101,
	AGPU_VR_EVENT_TYPE_TRACKED_DEVICE_UPDATED = 102,
	AGPU_VR_EVENT_TYPE_TRACKED_DEVICE_USER_INTERACTION_STARTED = 103,
	AGPU_VR_EVENT_TYPE_TRACKED_DEVICE_USER_INTERACTION_ENDED = 104,
	AGPU_VR_EVENT_TYPE_IPD_CHANGED = 105,
	AGPU_VR_EVENT_TYPE_ENTER_STANDBY_MODE = 106,
	AGPU_VR_EVENT_TYPE_LEAVE_STANDBY_MODE = 107,
	AGPU_VR_EVENT_TYPE_TRACKED_DEVICE_ROLE_CHANGED = 108,
	AGPU_VR_EVENT_TYPE_WIRELESS_DISCONNECT = 112,
	AGPU_VR_EVENT_TYPE_WIRELESS_RECONNECT = 113,
	AGPU_VR_EVENT_TYPE_BUTTON_PRESSED = 200,
	AGPU_VR_EVENT_TYPE_BUTTON_RELEASED = 201,
	AGPU_VR_EVENT_TYPE_BUTTON_TOUCH = 202,
	AGPU_VR_EVENT_TYPE_BUTTON_UNTOUCH = 203,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_PRESSED = 250,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_RELEASED = 251,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_TOUCH = 252,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_UNTOUCH = 253,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_MOVE = 254,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_MODE_SWITCH_1 = 255,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_MODE_SWITCH_2 = 256,
	AGPU_VR_EVENT_TYPE_DUAL_ANALOG_CANCEL = 257,
} agpu_vr_event_type;

typedef enum {
	AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_POSITION = 0,
	AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_COLOR = 1,
	AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_NORMAL = 2,
	AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_TEXCOORD = 3,
} agpu_immediate_renderer_vertex_attribute;

typedef enum {
	AGPU_IMMEDIATE_RENDERER_FOG_MODE_NONE = 0,
	AGPU_IMMEDIATE_RENDERER_FOG_MODE_LINEAR = 1,
	AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL = 2,
	AGPU_IMMEDIATE_RENDERER_FOG_MODE_EXPONENTIAL_SQUARED = 3,
} agpu_immediate_renderer_fog_mode;


/* Structure agpu_device_open_info. */
typedef struct agpu_device_open_info {
	agpu_pointer display;
	agpu_cstring window_system_name;
	agpu_bool debug_layer;
	agpu_cstring application_name;
	agpu_uint application_version;
	agpu_cstring engine_name;
	agpu_uint engine_version;
	agpu_int gpu_index;
	agpu_device_open_flags open_flags;
} agpu_device_open_info;

/* Structure agpu_swap_chain_create_info. */
typedef struct agpu_swap_chain_create_info {
	agpu_pointer display;
	agpu_pointer window;
	agpu_cstring window_system_name;
	agpu_pointer surface;
	agpu_texture_format colorbuffer_format;
	agpu_texture_format depth_stencil_format;
	agpu_uint width;
	agpu_uint height;
	agpu_uint buffer_count;
	agpu_bool sample_buffers;
	agpu_int samples;
	agpu_swap_chain_flags flags;
	agpu_int x;
	agpu_int y;
	agpu_swap_chain* old_swap_chain;
} agpu_swap_chain_create_info;

/* Structure agpu_buffer_description. */
typedef struct agpu_buffer_description {
	agpu_uint size;
	agpu_memory_heap_type heap_type;
	agpu_buffer_usage_mask usage_modes;
	agpu_buffer_usage_mask main_usage_mode;
	agpu_bitfield mapping_flags;
	agpu_uint stride;
} agpu_buffer_description;

/* Structure agpu_texture_description. */
typedef struct agpu_texture_description {
	agpu_texture_type type;
	agpu_uint width;
	agpu_uint height;
	agpu_uint depth;
	agpu_uint layers;
	agpu_ushort miplevels;
	agpu_texture_format format;
	agpu_texture_usage_mode_mask usage_modes;
	agpu_texture_usage_mode_mask main_usage_mode;
	agpu_memory_heap_type heap_type;
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
	agpu_texture_usage_mode_mask usage_mode;
	agpu_uint base_miplevel;
	agpu_uint level_count;
	agpu_uint base_arraylayer;
	agpu_uint layer_count;
} agpu_subresource_range;

/* Structure agpu_texture_view_description. */
typedef struct agpu_texture_view_description {
	agpu_texture_type type;
	agpu_texture_format format;
	agpu_uint sample_count;
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
	agpu_texture_format format;
	agpu_size offset;
	agpu_uint divisor;
} agpu_vertex_attrib_description;

/* Structure agpu_color4f. */
typedef struct agpu_color4f {
	agpu_float r;
	agpu_float g;
	agpu_float b;
	agpu_float a;
} agpu_color4f;

/* Structure agpu_depth_stencil_value. */
typedef struct agpu_depth_stencil_value {
	agpu_float depth;
	agpu_byte stencil;
} agpu_depth_stencil_value;

/* Structure agpu_sampler_description. */
typedef struct agpu_sampler_description {
	agpu_filter filter;
	agpu_texture_address_mode address_u;
	agpu_texture_address_mode address_v;
	agpu_texture_address_mode address_w;
	agpu_float mip_lod_bias;
	agpu_float maxanisotropy;
	agpu_bool comparison_enabled;
	agpu_compare_function comparison_function;
	agpu_color4f border_color;
	agpu_float min_lod;
	agpu_float max_lod;
} agpu_sampler_description;

/* Structure agpu_renderpass_color_attachment_description. */
typedef struct agpu_renderpass_color_attachment_description {
	agpu_texture_format format;
	agpu_uint sample_count;
	agpu_uint sample_quality;
	agpu_renderpass_attachment_action begin_action;
	agpu_renderpass_attachment_action end_action;
	agpu_color4f clear_value;
} agpu_renderpass_color_attachment_description;

/* Structure agpu_renderpass_depth_stencil_description. */
typedef struct agpu_renderpass_depth_stencil_description {
	agpu_texture_format format;
	agpu_uint sample_count;
	agpu_uint sample_quality;
	agpu_renderpass_attachment_action begin_action;
	agpu_renderpass_attachment_action end_action;
	agpu_renderpass_attachment_action stencil_begin_action;
	agpu_renderpass_attachment_action stencil_end_action;
	agpu_depth_stencil_value clear_value;
} agpu_renderpass_depth_stencil_description;

/* Structure agpu_renderpass_description. */
typedef struct agpu_renderpass_description {
	agpu_size color_attachment_count;
	agpu_renderpass_color_attachment_description* color_attachments;
	agpu_renderpass_depth_stencil_description* depth_stencil_attachment;
} agpu_renderpass_description;

/* Structure agpu_inheritance_info. */
typedef struct agpu_inheritance_info {
	agpu_int flat;
	agpu_renderpass* renderpass;
} agpu_inheritance_info;

/* Structure agpu_vector3f. */
typedef struct agpu_vector3f {
	agpu_float x;
	agpu_float y;
	agpu_float z;
} agpu_vector3f;

/* Structure agpu_vector4f. */
typedef struct agpu_vector4f {
	agpu_float x;
	agpu_float y;
	agpu_float z;
	agpu_float w;
} agpu_vector4f;

/* Structure agpu_quaternionf. */
typedef struct agpu_quaternionf {
	agpu_float w;
	agpu_float x;
	agpu_float y;
	agpu_float z;
} agpu_quaternionf;

/* Structure agpu_matrix3x3f. */
typedef struct agpu_matrix3x3f {
	agpu_vector3f c1;
	agpu_vector3f c2;
	agpu_vector3f c3;
} agpu_matrix3x3f;

/* Structure agpu_matrix4x4f. */
typedef struct agpu_matrix4x4f {
	agpu_vector4f c1;
	agpu_vector4f c2;
	agpu_vector4f c3;
	agpu_vector4f c4;
} agpu_matrix4x4f;

/* Structure agpu_size2d. */
typedef struct agpu_size2d {
	agpu_uint width;
	agpu_uint height;
} agpu_size2d;

/* Structure agpu_size3d. */
typedef struct agpu_size3d {
	agpu_uint width;
	agpu_uint height;
	agpu_uint depth;
} agpu_size3d;

/* Structure agpu_frustum_tangents. */
typedef struct agpu_frustum_tangents {
	agpu_float left;
	agpu_float right;
	agpu_float top;
	agpu_float bottom;
} agpu_frustum_tangents;

/* Structure agpu_region3d. */
typedef struct agpu_region3d {
	agpu_uint x;
	agpu_uint y;
	agpu_uint z;
	agpu_uint width;
	agpu_uint height;
	agpu_uint depth;
} agpu_region3d;

/* Structure agpu_buffer_image_copy_region. */
typedef struct agpu_buffer_image_copy_region {
	agpu_size buffer_offset;
	agpu_size buffer_row_length;
	agpu_size buffer_image_height;
	agpu_subresource_range texture_subresource_range;
	agpu_region3d texture_region;
} agpu_buffer_image_copy_region;

/* Structure agpu_vr_tracked_device_pose. */
typedef struct agpu_vr_tracked_device_pose {
	agpu_uint device_id;
	agpu_vr_tracked_device_class device_class;
	agpu_vr_tracked_device_role device_role;
	agpu_matrix4x4f device_to_absolute_tracking;
	agpu_vector3f velocity;
	agpu_vector3f angular_velocity;
	agpu_bool is_valid;
} agpu_vr_tracked_device_pose;

/* Structure agpu_vr_generic_event. */
typedef struct agpu_vr_generic_event {
	agpu_uint word1;
	agpu_uint word2;
	agpu_uint word3;
	agpu_uint word4;
	agpu_uint word5;
	agpu_uint word6;
	agpu_uint word7;
	agpu_uint word8;
} agpu_vr_generic_event;

/* Structure agpu_vr_controller_event. */
typedef struct agpu_vr_controller_event {
	agpu_uint button;
} agpu_vr_controller_event;

/* Structure agpu_vr_dual_analog_event. */
typedef struct agpu_vr_dual_analog_event {
	agpu_float x;
	agpu_float y;
	agpu_float transformed_x;
	agpu_float transformed_y;
	agpu_uint which;
} agpu_vr_dual_analog_event;

/* Union agpu_vr_event_data. */
typedef union agpu_vr_event_data {
	agpu_uint type;
	agpu_vr_generic_event generic;
	agpu_vr_controller_event controller;
	agpu_vr_dual_analog_event dual_analog;
} agpu_vr_event_data;

/* Structure agpu_vr_event. */
typedef struct agpu_vr_event {
	agpu_uint type;
	agpu_uint tracked_device_index;
	agpu_float event_age_seconds;
	agpu_vr_event_data data;
} agpu_vr_event;

/* Structure agpu_immediate_renderer_light. */
typedef struct agpu_immediate_renderer_light {
	agpu_vector4f ambient;
	agpu_vector4f diffuse;
	agpu_vector4f specular;
	agpu_vector4f position;
	agpu_vector3f spot_direction;
	agpu_float spot_exponent;
	agpu_float spot_cutoff;
	agpu_float constant_attenuation;
	agpu_float linear_attenuation;
	agpu_float quadratic_attenuation;
} agpu_immediate_renderer_light;

/* Structure agpu_immediate_renderer_material. */
typedef struct agpu_immediate_renderer_material {
	agpu_vector4f emission;
	agpu_vector4f ambient;
	agpu_vector4f diffuse;
	agpu_vector4f specular;
	agpu_float shininess;
} agpu_immediate_renderer_material;

/* Global functions. */
typedef agpu_error (*agpuGetPlatforms_FUN) (agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms);

AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms);

/* Methods for interface agpu_platform. */
typedef agpu_device* (*agpuOpenDevice_FUN) (agpu_platform* platform, agpu_device_open_info* openInfo);
typedef agpu_cstring (*agpuGetPlatformName_FUN) (agpu_platform* platform);
typedef agpu_size (*agpuGetPlatformGpuCount_FUN) (agpu_platform* platform);
typedef agpu_cstring (*agpuGetPlatformGpuName_FUN) (agpu_platform* platform, agpu_size gpu_index);
typedef agpu_int (*agpuGetPlatformVersion_FUN) (agpu_platform* platform);
typedef agpu_int (*agpuGetPlatformImplementationVersion_FUN) (agpu_platform* platform);
typedef agpu_bool (*agpuPlatformHasRealMultithreading_FUN) (agpu_platform* platform);
typedef agpu_bool (*agpuIsNativePlatform_FUN) (agpu_platform* platform);
typedef agpu_bool (*agpuIsCrossPlatform_FUN) (agpu_platform* platform);
typedef agpu_offline_shader_compiler* (*agpuCreateOfflineShaderCompiler_FUN) (agpu_platform* platform);

AGPU_EXPORT agpu_device* agpuOpenDevice(agpu_platform* platform, agpu_device_open_info* openInfo);
AGPU_EXPORT agpu_cstring agpuGetPlatformName(agpu_platform* platform);
AGPU_EXPORT agpu_size agpuGetPlatformGpuCount(agpu_platform* platform);
AGPU_EXPORT agpu_cstring agpuGetPlatformGpuName(agpu_platform* platform, agpu_size gpu_index);
AGPU_EXPORT agpu_int agpuGetPlatformVersion(agpu_platform* platform);
AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion(agpu_platform* platform);
AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading(agpu_platform* platform);
AGPU_EXPORT agpu_bool agpuIsNativePlatform(agpu_platform* platform);
AGPU_EXPORT agpu_bool agpuIsCrossPlatform(agpu_platform* platform);
AGPU_EXPORT agpu_offline_shader_compiler* agpuCreateOfflineShaderCompiler(agpu_platform* platform);

/* Methods for interface agpu_device. */
typedef agpu_error (*agpuAddDeviceReference_FUN) (agpu_device* device);
typedef agpu_error (*agpuReleaseDevice_FUN) (agpu_device* device);
typedef agpu_command_queue* (*agpuGetDefaultCommandQueue_FUN) (agpu_device* device);
typedef agpu_swap_chain* (*agpuCreateSwapChain_FUN) (agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo);
typedef agpu_buffer* (*agpuCreateBuffer_FUN) (agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data);
typedef agpu_vertex_layout* (*agpuCreateVertexLayout_FUN) (agpu_device* device);
typedef agpu_vertex_binding* (*agpuCreateVertexBinding_FUN) (agpu_device* device, agpu_vertex_layout* layout);
typedef agpu_shader* (*agpuCreateShader_FUN) (agpu_device* device, agpu_shader_type type);
typedef agpu_shader_signature_builder* (*agpuCreateShaderSignatureBuilder_FUN) (agpu_device* device);
typedef agpu_pipeline_builder* (*agpuCreatePipelineBuilder_FUN) (agpu_device* device);
typedef agpu_compute_pipeline_builder* (*agpuCreateComputePipelineBuilder_FUN) (agpu_device* device);
typedef agpu_command_allocator* (*agpuCreateCommandAllocator_FUN) (agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue);
typedef agpu_command_list* (*agpuCreateCommandList_FUN) (agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);
typedef agpu_shader_language (*agpuGetPreferredShaderLanguage_FUN) (agpu_device* device);
typedef agpu_shader_language (*agpuGetPreferredIntermediateShaderLanguage_FUN) (agpu_device* device);
typedef agpu_shader_language (*agpuGetPreferredHighLevelShaderLanguage_FUN) (agpu_device* device);
typedef agpu_framebuffer* (*agpuCreateFrameBuffer_FUN) (agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view** colorViews, agpu_texture_view* depthStencilView);
typedef agpu_renderpass* (*agpuCreateRenderPass_FUN) (agpu_device* device, agpu_renderpass_description* description);
typedef agpu_texture* (*agpuCreateTexture_FUN) (agpu_device* device, agpu_texture_description* description);
typedef agpu_sampler* (*agpuCreateSampler_FUN) (agpu_device* device, agpu_sampler_description* description);
typedef agpu_fence* (*agpuCreateFence_FUN) (agpu_device* device);
typedef agpu_int (*agpuGetMultiSampleQualityLevels_FUN) (agpu_device* device, agpu_texture_format format, agpu_uint sample_count);
typedef agpu_bool (*agpuHasTopLeftNdcOrigin_FUN) (agpu_device* device);
typedef agpu_bool (*agpuHasBottomLeftTextureCoordinates_FUN) (agpu_device* device);
typedef agpu_bool (*agpuIsFeatureSupportedOnDevice_FUN) (agpu_device* device, agpu_feature feature);
typedef agpu_vr_system* (*agpuGetVRSystem_FUN) (agpu_device* device);
typedef agpu_offline_shader_compiler* (*agpuCreateOfflineShaderCompilerForDevice_FUN) (agpu_device* device);
typedef agpu_state_tracker_cache* (*agpuCreateStateTrackerCache_FUN) (agpu_device* device, agpu_command_queue* command_queue_family);
typedef agpu_error (*agpuFinishDeviceExecution_FUN) (agpu_device* device);

AGPU_EXPORT agpu_error agpuAddDeviceReference(agpu_device* device);
AGPU_EXPORT agpu_error agpuReleaseDevice(agpu_device* device);
AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue(agpu_device* device);
AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain(agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo);
AGPU_EXPORT agpu_buffer* agpuCreateBuffer(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data);
AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout(agpu_device* device);
AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* device, agpu_vertex_layout* layout);
AGPU_EXPORT agpu_shader* agpuCreateShader(agpu_device* device, agpu_shader_type type);
AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder(agpu_device* device);
AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder(agpu_device* device);
AGPU_EXPORT agpu_compute_pipeline_builder* agpuCreateComputePipelineBuilder(agpu_device* device);
AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator(agpu_device* device, agpu_command_list_type type, agpu_command_queue* queue);
AGPU_EXPORT agpu_command_list* agpuCreateCommandList(agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);
AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* device);
AGPU_EXPORT agpu_shader_language agpuGetPreferredIntermediateShaderLanguage(agpu_device* device);
AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* device);
AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view** colorViews, agpu_texture_view* depthStencilView);
AGPU_EXPORT agpu_renderpass* agpuCreateRenderPass(agpu_device* device, agpu_renderpass_description* description);
AGPU_EXPORT agpu_texture* agpuCreateTexture(agpu_device* device, agpu_texture_description* description);
AGPU_EXPORT agpu_sampler* agpuCreateSampler(agpu_device* device, agpu_sampler_description* description);
AGPU_EXPORT agpu_fence* agpuCreateFence(agpu_device* device);
AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels(agpu_device* device, agpu_texture_format format, agpu_uint sample_count);
AGPU_EXPORT agpu_bool agpuHasTopLeftNdcOrigin(agpu_device* device);
AGPU_EXPORT agpu_bool agpuHasBottomLeftTextureCoordinates(agpu_device* device);
AGPU_EXPORT agpu_bool agpuIsFeatureSupportedOnDevice(agpu_device* device, agpu_feature feature);
AGPU_EXPORT agpu_vr_system* agpuGetVRSystem(agpu_device* device);
AGPU_EXPORT agpu_offline_shader_compiler* agpuCreateOfflineShaderCompilerForDevice(agpu_device* device);
AGPU_EXPORT agpu_state_tracker_cache* agpuCreateStateTrackerCache(agpu_device* device, agpu_command_queue* command_queue_family);
AGPU_EXPORT agpu_error agpuFinishDeviceExecution(agpu_device* device);

/* Methods for interface agpu_vr_system. */
typedef agpu_error (*agpuAddVRSystemReference_FUN) (agpu_vr_system* vr_system);
typedef agpu_error (*agpuReleaseVRSystem_FUN) (agpu_vr_system* vr_system);
typedef agpu_cstring (*agpuGetVRSystemName_FUN) (agpu_vr_system* vr_system);
typedef agpu_pointer (*agpuGetVRSystemNativeHandle_FUN) (agpu_vr_system* vr_system);
typedef agpu_error (*agpuGetVRRecommendedRenderTargetSize_FUN) (agpu_vr_system* vr_system, agpu_size2d* size);
typedef agpu_error (*agpuGetVREyeToHeadTransformInto_FUN) (agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_matrix4x4f* transform);
typedef agpu_error (*agpuGetVRProjectionMatrix_FUN) (agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix);
typedef agpu_error (*agpuGetVRProjectionFrustumTangents_FUN) (agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_frustum_tangents* frustum);
typedef agpu_error (*agpuSubmitVREyeRenderTargets_FUN) (agpu_vr_system* vr_system, agpu_texture* left_eye, agpu_texture* right_eye);
typedef agpu_error (*agpuWaitAndFetchVRPoses_FUN) (agpu_vr_system* vr_system);
typedef agpu_size (*agpuGetMaxVRTrackedDevicePoseCount_FUN) (agpu_vr_system* vr_system);
typedef agpu_size (*agpuGetCurrentVRTrackedDevicePoseCount_FUN) (agpu_vr_system* vr_system);
typedef agpu_error (*agpuGetCurrentVRTrackedDevicePoseInto_FUN) (agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest);
typedef agpu_size (*agpuGetMaxVRRenderTrackedDevicePoseCount_FUN) (agpu_vr_system* vr_system);
typedef agpu_size (*agpuGetCurrentVRRenderTrackedDevicePoseCount_FUN) (agpu_vr_system* vr_system);
typedef agpu_error (*agpuGetCurrentVRRenderTrackedDevicePoseInto_FUN) (agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest);
typedef agpu_bool (*agpuPollVREvent_FUN) (agpu_vr_system* vr_system, agpu_vr_event* event);

AGPU_EXPORT agpu_error agpuAddVRSystemReference(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_error agpuReleaseVRSystem(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_cstring agpuGetVRSystemName(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_pointer agpuGetVRSystemNativeHandle(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_error agpuGetVRRecommendedRenderTargetSize(agpu_vr_system* vr_system, agpu_size2d* size);
AGPU_EXPORT agpu_error agpuGetVREyeToHeadTransformInto(agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_matrix4x4f* transform);
AGPU_EXPORT agpu_error agpuGetVRProjectionMatrix(agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_float near_distance, agpu_float far_distance, agpu_matrix4x4f* projection_matrix);
AGPU_EXPORT agpu_error agpuGetVRProjectionFrustumTangents(agpu_vr_system* vr_system, agpu_vr_eye eye, agpu_frustum_tangents* frustum);
AGPU_EXPORT agpu_error agpuSubmitVREyeRenderTargets(agpu_vr_system* vr_system, agpu_texture* left_eye, agpu_texture* right_eye);
AGPU_EXPORT agpu_error agpuWaitAndFetchVRPoses(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_size agpuGetMaxVRTrackedDevicePoseCount(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_size agpuGetCurrentVRTrackedDevicePoseCount(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_error agpuGetCurrentVRTrackedDevicePoseInto(agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest);
AGPU_EXPORT agpu_size agpuGetMaxVRRenderTrackedDevicePoseCount(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_size agpuGetCurrentVRRenderTrackedDevicePoseCount(agpu_vr_system* vr_system);
AGPU_EXPORT agpu_error agpuGetCurrentVRRenderTrackedDevicePoseInto(agpu_vr_system* vr_system, agpu_size index, agpu_vr_tracked_device_pose* dest);
AGPU_EXPORT agpu_bool agpuPollVREvent(agpu_vr_system* vr_system, agpu_vr_event* event);

/* Methods for interface agpu_swap_chain. */
typedef agpu_error (*agpuAddSwapChainReference_FUN) (agpu_swap_chain* swap_chain);
typedef agpu_error (*agpuReleaseSwapChain_FUN) (agpu_swap_chain* swap_chain);
typedef agpu_error (*agpuSwapBuffers_FUN) (agpu_swap_chain* swap_chain);
typedef agpu_framebuffer* (*agpuGetCurrentBackBuffer_FUN) (agpu_swap_chain* swap_chain);
typedef agpu_size (*agpuGetCurrentBackBufferIndex_FUN) (agpu_swap_chain* swap_chain);
typedef agpu_size (*agpuGetFramebufferCount_FUN) (agpu_swap_chain* swap_chain);
typedef agpu_error (*agpuSetSwapChainOverlayPosition_FUN) (agpu_swap_chain* swap_chain, agpu_int x, agpu_int y);

AGPU_EXPORT agpu_error agpuAddSwapChainReference(agpu_swap_chain* swap_chain);
AGPU_EXPORT agpu_error agpuReleaseSwapChain(agpu_swap_chain* swap_chain);
AGPU_EXPORT agpu_error agpuSwapBuffers(agpu_swap_chain* swap_chain);
AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer(agpu_swap_chain* swap_chain);
AGPU_EXPORT agpu_size agpuGetCurrentBackBufferIndex(agpu_swap_chain* swap_chain);
AGPU_EXPORT agpu_size agpuGetFramebufferCount(agpu_swap_chain* swap_chain);
AGPU_EXPORT agpu_error agpuSetSwapChainOverlayPosition(agpu_swap_chain* swap_chain, agpu_int x, agpu_int y);

/* Methods for interface agpu_compute_pipeline_builder. */
typedef agpu_error (*agpuAddComputePipelineBuilderReference_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder);
typedef agpu_error (*agpuReleaseComputePipelineBuilder_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder);
typedef agpu_pipeline_state* (*agpuBuildComputePipelineState_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder);
typedef agpu_error (*agpuAttachComputeShader_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader);
typedef agpu_error (*agpuAttachComputeShaderWithEntryPoint_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point);
typedef agpu_size (*agpuGetComputePipelineBuildingLogLength_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder);
typedef agpu_error (*agpuGetComputePipelineBuildingLog_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer);
typedef agpu_error (*agpuSetComputePipelineShaderSignature_FUN) (agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader_signature* signature);

AGPU_EXPORT agpu_error agpuAddComputePipelineBuilderReference(agpu_compute_pipeline_builder* compute_pipeline_builder);
AGPU_EXPORT agpu_error agpuReleaseComputePipelineBuilder(agpu_compute_pipeline_builder* compute_pipeline_builder);
AGPU_EXPORT agpu_pipeline_state* agpuBuildComputePipelineState(agpu_compute_pipeline_builder* compute_pipeline_builder);
AGPU_EXPORT agpu_error agpuAttachComputeShader(agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader);
AGPU_EXPORT agpu_error agpuAttachComputeShaderWithEntryPoint(agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point);
AGPU_EXPORT agpu_size agpuGetComputePipelineBuildingLogLength(agpu_compute_pipeline_builder* compute_pipeline_builder);
AGPU_EXPORT agpu_error agpuGetComputePipelineBuildingLog(agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer);
AGPU_EXPORT agpu_error agpuSetComputePipelineShaderSignature(agpu_compute_pipeline_builder* compute_pipeline_builder, agpu_shader_signature* signature);

/* Methods for interface agpu_pipeline_builder. */
typedef agpu_error (*agpuAddPipelineBuilderReference_FUN) (agpu_pipeline_builder* pipeline_builder);
typedef agpu_error (*agpuReleasePipelineBuilder_FUN) (agpu_pipeline_builder* pipeline_builder);
typedef agpu_pipeline_state* (*agpuBuildPipelineState_FUN) (agpu_pipeline_builder* pipeline_builder);
typedef agpu_error (*agpuAttachShader_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_shader* shader);
typedef agpu_error (*agpuAttachShaderWithEntryPoint_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point);
typedef agpu_size (*agpuGetPipelineBuildingLogLength_FUN) (agpu_pipeline_builder* pipeline_builder);
typedef agpu_error (*agpuGetPipelineBuildingLog_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer);
typedef agpu_error (*agpuSetBlendState_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled);
typedef agpu_error (*agpuSetBlendFunction_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation);
typedef agpu_error (*agpuSetColorMask_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled);
typedef agpu_error (*agpuSetFrontFace_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_face_winding winding);
typedef agpu_error (*agpuSetCullMode_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_cull_mode mode);
typedef agpu_error (*agpuSetDepthBias_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor);
typedef agpu_error (*agpuSetDepthState_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function);
typedef agpu_error (*agpuSetPolygonMode_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_polygon_mode mode);
typedef agpu_error (*agpuSetStencilState_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask);
typedef agpu_error (*agpuSetStencilFrontFace_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
typedef agpu_error (*agpuSetStencilBackFace_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
typedef agpu_error (*agpuSetRenderTargetCount_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_int count);
typedef agpu_error (*agpuSetRenderTargetFormat_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format);
typedef agpu_error (*agpuSetDepthStencilFormat_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_texture_format format);
typedef agpu_error (*agpuSetPrimitiveType_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type);
typedef agpu_error (*agpuSetVertexLayout_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout);
typedef agpu_error (*agpuSetPipelineShaderSignature_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature);
typedef agpu_error (*agpuSetSampleDescription_FUN) (agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality);

AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference(agpu_pipeline_builder* pipeline_builder);
AGPU_EXPORT agpu_error agpuReleasePipelineBuilder(agpu_pipeline_builder* pipeline_builder);
AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState(agpu_pipeline_builder* pipeline_builder);
AGPU_EXPORT agpu_error agpuAttachShader(agpu_pipeline_builder* pipeline_builder, agpu_shader* shader);
AGPU_EXPORT agpu_error agpuAttachShaderWithEntryPoint(agpu_pipeline_builder* pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point);
AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength(agpu_pipeline_builder* pipeline_builder);
AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog(agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer);
AGPU_EXPORT agpu_error agpuSetBlendState(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled);
AGPU_EXPORT agpu_error agpuSetBlendFunction(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation);
AGPU_EXPORT agpu_error agpuSetColorMask(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled);
AGPU_EXPORT agpu_error agpuSetFrontFace(agpu_pipeline_builder* pipeline_builder, agpu_face_winding winding);
AGPU_EXPORT agpu_error agpuSetCullMode(agpu_pipeline_builder* pipeline_builder, agpu_cull_mode mode);
AGPU_EXPORT agpu_error agpuSetDepthBias(agpu_pipeline_builder* pipeline_builder, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor);
AGPU_EXPORT agpu_error agpuSetDepthState(agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function);
AGPU_EXPORT agpu_error agpuSetPolygonMode(agpu_pipeline_builder* pipeline_builder, agpu_polygon_mode mode);
AGPU_EXPORT agpu_error agpuSetStencilState(agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask);
AGPU_EXPORT agpu_error agpuSetStencilFrontFace(agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
AGPU_EXPORT agpu_error agpuSetStencilBackFace(agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
AGPU_EXPORT agpu_error agpuSetRenderTargetCount(agpu_pipeline_builder* pipeline_builder, agpu_int count);
AGPU_EXPORT agpu_error agpuSetRenderTargetFormat(agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format);
AGPU_EXPORT agpu_error agpuSetDepthStencilFormat(agpu_pipeline_builder* pipeline_builder, agpu_texture_format format);
AGPU_EXPORT agpu_error agpuSetPrimitiveType(agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type);
AGPU_EXPORT agpu_error agpuSetVertexLayout(agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout);
AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature(agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature);
AGPU_EXPORT agpu_error agpuSetSampleDescription(agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality);

/* Methods for interface agpu_pipeline_state. */
typedef agpu_error (*agpuAddPipelineStateReference_FUN) (agpu_pipeline_state* pipeline_state);
typedef agpu_error (*agpuReleasePipelineState_FUN) (agpu_pipeline_state* pipeline_state);

AGPU_EXPORT agpu_error agpuAddPipelineStateReference(agpu_pipeline_state* pipeline_state);
AGPU_EXPORT agpu_error agpuReleasePipelineState(agpu_pipeline_state* pipeline_state);

/* Methods for interface agpu_command_queue. */
typedef agpu_error (*agpuAddCommandQueueReference_FUN) (agpu_command_queue* command_queue);
typedef agpu_error (*agpuReleaseCommandQueue_FUN) (agpu_command_queue* command_queue);
typedef agpu_error (*agpuAddCommandList_FUN) (agpu_command_queue* command_queue, agpu_command_list* command_list);
typedef agpu_error (*agpuFinishQueueExecution_FUN) (agpu_command_queue* command_queue);
typedef agpu_error (*agpuSignalFence_FUN) (agpu_command_queue* command_queue, agpu_fence* fence);
typedef agpu_error (*agpuWaitFence_FUN) (agpu_command_queue* command_queue, agpu_fence* fence);

AGPU_EXPORT agpu_error agpuAddCommandQueueReference(agpu_command_queue* command_queue);
AGPU_EXPORT agpu_error agpuReleaseCommandQueue(agpu_command_queue* command_queue);
AGPU_EXPORT agpu_error agpuAddCommandList(agpu_command_queue* command_queue, agpu_command_list* command_list);
AGPU_EXPORT agpu_error agpuFinishQueueExecution(agpu_command_queue* command_queue);
AGPU_EXPORT agpu_error agpuSignalFence(agpu_command_queue* command_queue, agpu_fence* fence);
AGPU_EXPORT agpu_error agpuWaitFence(agpu_command_queue* command_queue, agpu_fence* fence);

/* Methods for interface agpu_command_allocator. */
typedef agpu_error (*agpuAddCommandAllocatorReference_FUN) (agpu_command_allocator* command_allocator);
typedef agpu_error (*agpuReleaseCommandAllocator_FUN) (agpu_command_allocator* command_allocator);
typedef agpu_error (*agpuResetCommandAllocator_FUN) (agpu_command_allocator* command_allocator);

AGPU_EXPORT agpu_error agpuAddCommandAllocatorReference(agpu_command_allocator* command_allocator);
AGPU_EXPORT agpu_error agpuReleaseCommandAllocator(agpu_command_allocator* command_allocator);
AGPU_EXPORT agpu_error agpuResetCommandAllocator(agpu_command_allocator* command_allocator);

/* Methods for interface agpu_command_list. */
typedef agpu_error (*agpuAddCommandListReference_FUN) (agpu_command_list* command_list);
typedef agpu_error (*agpuReleaseCommandList_FUN) (agpu_command_list* command_list);
typedef agpu_error (*agpuSetShaderSignature_FUN) (agpu_command_list* command_list, agpu_shader_signature* signature);
typedef agpu_error (*agpuSetViewport_FUN) (agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
typedef agpu_error (*agpuSetScissor_FUN) (agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
typedef agpu_error (*agpuUsePipelineState_FUN) (agpu_command_list* command_list, agpu_pipeline_state* pipeline);
typedef agpu_error (*agpuUseVertexBinding_FUN) (agpu_command_list* command_list, agpu_vertex_binding* vertex_binding);
typedef agpu_error (*agpuUseIndexBuffer_FUN) (agpu_command_list* command_list, agpu_buffer* index_buffer);
typedef agpu_error (*agpuUseIndexBufferAt_FUN) (agpu_command_list* command_list, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size);
typedef agpu_error (*agpuUseDrawIndirectBuffer_FUN) (agpu_command_list* command_list, agpu_buffer* draw_buffer);
typedef agpu_error (*agpuUseComputeDispatchIndirectBuffer_FUN) (agpu_command_list* command_list, agpu_buffer* buffer);
typedef agpu_error (*agpuUseShaderResources_FUN) (agpu_command_list* command_list, agpu_shader_resource_binding* binding);
typedef agpu_error (*agpuUseComputeShaderResources_FUN) (agpu_command_list* command_list, agpu_shader_resource_binding* binding);
typedef agpu_error (*agpuDrawArrays_FUN) (agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance);
typedef agpu_error (*agpuDrawArraysIndirect_FUN) (agpu_command_list* command_list, agpu_size offset, agpu_size drawcount);
typedef agpu_error (*agpuDrawElements_FUN) (agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
typedef agpu_error (*agpuDrawElementsIndirect_FUN) (agpu_command_list* command_list, agpu_size offset, agpu_size drawcount);
typedef agpu_error (*agpuDispatchCompute_FUN) (agpu_command_list* command_list, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z);
typedef agpu_error (*agpuDispatchComputeIndirect_FUN) (agpu_command_list* command_list, agpu_size offset);
typedef agpu_error (*agpuSetStencilReference_FUN) (agpu_command_list* command_list, agpu_uint reference);
typedef agpu_error (*agpuExecuteBundle_FUN) (agpu_command_list* command_list, agpu_command_list* bundle);
typedef agpu_error (*agpuCloseCommandList_FUN) (agpu_command_list* command_list);
typedef agpu_error (*agpuResetCommandList_FUN) (agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);
typedef agpu_error (*agpuResetBundleCommandList_FUN) (agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info);
typedef agpu_error (*agpuBeginRenderPass_FUN) (agpu_command_list* command_list, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content);
typedef agpu_error (*agpuEndRenderPass_FUN) (agpu_command_list* command_list);
typedef agpu_error (*agpuResolveFramebuffer_FUN) (agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer);
typedef agpu_error (*agpuResolveTexture_FUN) (agpu_command_list* command_list, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect);
typedef agpu_error (*agpuPushConstants_FUN) (agpu_command_list* command_list, agpu_uint offset, agpu_uint size, agpu_pointer values);
typedef agpu_error (*agpuMemoryBarrier_FUN) (agpu_command_list* command_list, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses);
typedef agpu_error (*agpuBufferMemoryBarrier_FUN) (agpu_command_list* command_list, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size);
typedef agpu_error (*agpuTextureMemoryBarrier_FUN) (agpu_command_list* command_list, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range);
typedef agpu_error (*agpuPushBufferTransitionBarrier_FUN) (agpu_command_list* command_list, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage);
typedef agpu_error (*agpuPushTextureTransitionBarrier_FUN) (agpu_command_list* command_list, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range);
typedef agpu_error (*agpuPopBufferTransitionBarrier_FUN) (agpu_command_list* command_list);
typedef agpu_error (*agpuPopTextureTransitionBarrier_FUN) (agpu_command_list* command_list);
typedef agpu_error (*agpuCopyBuffer_FUN) (agpu_command_list* command_list, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size);
typedef agpu_error (*agpuCopyBufferToTexture_FUN) (agpu_command_list* command_list, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region);
typedef agpu_error (*agpuCopyTextureToBuffer_FUN) (agpu_command_list* command_list, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region);

AGPU_EXPORT agpu_error agpuAddCommandListReference(agpu_command_list* command_list);
AGPU_EXPORT agpu_error agpuReleaseCommandList(agpu_command_list* command_list);
AGPU_EXPORT agpu_error agpuSetShaderSignature(agpu_command_list* command_list, agpu_shader_signature* signature);
AGPU_EXPORT agpu_error agpuSetViewport(agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
AGPU_EXPORT agpu_error agpuSetScissor(agpu_command_list* command_list, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
AGPU_EXPORT agpu_error agpuUsePipelineState(agpu_command_list* command_list, agpu_pipeline_state* pipeline);
AGPU_EXPORT agpu_error agpuUseVertexBinding(agpu_command_list* command_list, agpu_vertex_binding* vertex_binding);
AGPU_EXPORT agpu_error agpuUseIndexBuffer(agpu_command_list* command_list, agpu_buffer* index_buffer);
AGPU_EXPORT agpu_error agpuUseIndexBufferAt(agpu_command_list* command_list, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size);
AGPU_EXPORT agpu_error agpuUseDrawIndirectBuffer(agpu_command_list* command_list, agpu_buffer* draw_buffer);
AGPU_EXPORT agpu_error agpuUseComputeDispatchIndirectBuffer(agpu_command_list* command_list, agpu_buffer* buffer);
AGPU_EXPORT agpu_error agpuUseShaderResources(agpu_command_list* command_list, agpu_shader_resource_binding* binding);
AGPU_EXPORT agpu_error agpuUseComputeShaderResources(agpu_command_list* command_list, agpu_shader_resource_binding* binding);
AGPU_EXPORT agpu_error agpuDrawArrays(agpu_command_list* command_list, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance);
AGPU_EXPORT agpu_error agpuDrawArraysIndirect(agpu_command_list* command_list, agpu_size offset, agpu_size drawcount);
AGPU_EXPORT agpu_error agpuDrawElements(agpu_command_list* command_list, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
AGPU_EXPORT agpu_error agpuDrawElementsIndirect(agpu_command_list* command_list, agpu_size offset, agpu_size drawcount);
AGPU_EXPORT agpu_error agpuDispatchCompute(agpu_command_list* command_list, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z);
AGPU_EXPORT agpu_error agpuDispatchComputeIndirect(agpu_command_list* command_list, agpu_size offset);
AGPU_EXPORT agpu_error agpuSetStencilReference(agpu_command_list* command_list, agpu_uint reference);
AGPU_EXPORT agpu_error agpuExecuteBundle(agpu_command_list* command_list, agpu_command_list* bundle);
AGPU_EXPORT agpu_error agpuCloseCommandList(agpu_command_list* command_list);
AGPU_EXPORT agpu_error agpuResetCommandList(agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state);
AGPU_EXPORT agpu_error agpuResetBundleCommandList(agpu_command_list* command_list, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state, agpu_inheritance_info* inheritance_info);
AGPU_EXPORT agpu_error agpuBeginRenderPass(agpu_command_list* command_list, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content);
AGPU_EXPORT agpu_error agpuEndRenderPass(agpu_command_list* command_list);
AGPU_EXPORT agpu_error agpuResolveFramebuffer(agpu_command_list* command_list, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer);
AGPU_EXPORT agpu_error agpuResolveTexture(agpu_command_list* command_list, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect);
AGPU_EXPORT agpu_error agpuPushConstants(agpu_command_list* command_list, agpu_uint offset, agpu_uint size, agpu_pointer values);
AGPU_EXPORT agpu_error agpuMemoryBarrier(agpu_command_list* command_list, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses);
AGPU_EXPORT agpu_error agpuBufferMemoryBarrier(agpu_command_list* command_list, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size);
AGPU_EXPORT agpu_error agpuTextureMemoryBarrier(agpu_command_list* command_list, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range);
AGPU_EXPORT agpu_error agpuPushBufferTransitionBarrier(agpu_command_list* command_list, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage);
AGPU_EXPORT agpu_error agpuPushTextureTransitionBarrier(agpu_command_list* command_list, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range);
AGPU_EXPORT agpu_error agpuPopBufferTransitionBarrier(agpu_command_list* command_list);
AGPU_EXPORT agpu_error agpuPopTextureTransitionBarrier(agpu_command_list* command_list);
AGPU_EXPORT agpu_error agpuCopyBuffer(agpu_command_list* command_list, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size);
AGPU_EXPORT agpu_error agpuCopyBufferToTexture(agpu_command_list* command_list, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region);
AGPU_EXPORT agpu_error agpuCopyTextureToBuffer(agpu_command_list* command_list, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region);

/* Methods for interface agpu_texture. */
typedef agpu_error (*agpuAddTextureReference_FUN) (agpu_texture* texture);
typedef agpu_error (*agpuReleaseTexture_FUN) (agpu_texture* texture);
typedef agpu_error (*agpuGetTextureDescription_FUN) (agpu_texture* texture, agpu_texture_description* description);
typedef agpu_pointer (*agpuMapTextureLevel_FUN) (agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region);
typedef agpu_error (*agpuUnmapTextureLevel_FUN) (agpu_texture* texture);
typedef agpu_error (*agpuReadTextureData_FUN) (agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer);
typedef agpu_error (*agpuReadTextureSubData_FUN) (agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer);
typedef agpu_error (*agpuUploadTextureData_FUN) (agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data);
typedef agpu_error (*agpuUploadTextureSubData_FUN) (agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data);
typedef agpu_error (*agpuGetTextureFullViewDescription_FUN) (agpu_texture* texture, agpu_texture_view_description* result);
typedef agpu_texture_view* (*agpuCreateTextureView_FUN) (agpu_texture* texture, agpu_texture_view_description* description);
typedef agpu_texture_view* (*agpuGetOrCreateFullTextureView_FUN) (agpu_texture* texture);

AGPU_EXPORT agpu_error agpuAddTextureReference(agpu_texture* texture);
AGPU_EXPORT agpu_error agpuReleaseTexture(agpu_texture* texture);
AGPU_EXPORT agpu_error agpuGetTextureDescription(agpu_texture* texture, agpu_texture_description* description);
AGPU_EXPORT agpu_pointer agpuMapTextureLevel(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region);
AGPU_EXPORT agpu_error agpuUnmapTextureLevel(agpu_texture* texture);
AGPU_EXPORT agpu_error agpuReadTextureData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer);
AGPU_EXPORT agpu_error agpuReadTextureSubData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer);
AGPU_EXPORT agpu_error agpuUploadTextureData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data);
AGPU_EXPORT agpu_error agpuUploadTextureSubData(agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data);
AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription(agpu_texture* texture, agpu_texture_view_description* result);
AGPU_EXPORT agpu_texture_view* agpuCreateTextureView(agpu_texture* texture, agpu_texture_view_description* description);
AGPU_EXPORT agpu_texture_view* agpuGetOrCreateFullTextureView(agpu_texture* texture);

/* Methods for interface agpu_texture_view. */
typedef agpu_error (*agpuAddTextureViewReference_FUN) (agpu_texture_view* texture_view);
typedef agpu_error (*agpuReleaseTextureView_FUN) (agpu_texture_view* texture_view);
typedef agpu_texture* (*agpuGetTextureFromView_FUN) (agpu_texture_view* texture_view);

AGPU_EXPORT agpu_error agpuAddTextureViewReference(agpu_texture_view* texture_view);
AGPU_EXPORT agpu_error agpuReleaseTextureView(agpu_texture_view* texture_view);
AGPU_EXPORT agpu_texture* agpuGetTextureFromView(agpu_texture_view* texture_view);

/* Methods for interface agpu_sampler. */
typedef agpu_error (*agpuAddSamplerReference_FUN) (agpu_sampler* sampler);
typedef agpu_error (*agpuReleaseSampler_FUN) (agpu_sampler* sampler);

AGPU_EXPORT agpu_error agpuAddSamplerReference(agpu_sampler* sampler);
AGPU_EXPORT agpu_error agpuReleaseSampler(agpu_sampler* sampler);

/* Methods for interface agpu_buffer. */
typedef agpu_error (*agpuAddBufferReference_FUN) (agpu_buffer* buffer);
typedef agpu_error (*agpuReleaseBuffer_FUN) (agpu_buffer* buffer);
typedef agpu_pointer (*agpuMapBuffer_FUN) (agpu_buffer* buffer, agpu_mapping_access flags);
typedef agpu_error (*agpuUnmapBuffer_FUN) (agpu_buffer* buffer);
typedef agpu_error (*agpuGetBufferDescription_FUN) (agpu_buffer* buffer, agpu_buffer_description* description);
typedef agpu_error (*agpuUploadBufferData_FUN) (agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data);
typedef agpu_error (*agpuReadBufferData_FUN) (agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data);
typedef agpu_error (*agpuFlushWholeBuffer_FUN) (agpu_buffer* buffer);
typedef agpu_error (*agpuInvalidateWholeBuffer_FUN) (agpu_buffer* buffer);

AGPU_EXPORT agpu_error agpuAddBufferReference(agpu_buffer* buffer);
AGPU_EXPORT agpu_error agpuReleaseBuffer(agpu_buffer* buffer);
AGPU_EXPORT agpu_pointer agpuMapBuffer(agpu_buffer* buffer, agpu_mapping_access flags);
AGPU_EXPORT agpu_error agpuUnmapBuffer(agpu_buffer* buffer);
AGPU_EXPORT agpu_error agpuGetBufferDescription(agpu_buffer* buffer, agpu_buffer_description* description);
AGPU_EXPORT agpu_error agpuUploadBufferData(agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data);
AGPU_EXPORT agpu_error agpuReadBufferData(agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data);
AGPU_EXPORT agpu_error agpuFlushWholeBuffer(agpu_buffer* buffer);
AGPU_EXPORT agpu_error agpuInvalidateWholeBuffer(agpu_buffer* buffer);

/* Methods for interface agpu_vertex_binding. */
typedef agpu_error (*agpuAddVertexBindingReference_FUN) (agpu_vertex_binding* vertex_binding);
typedef agpu_error (*agpuReleaseVertexBinding_FUN) (agpu_vertex_binding* vertex_binding);
typedef agpu_error (*agpuBindVertexBuffers_FUN) (agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers);
typedef agpu_error (*agpuBindVertexBuffersWithOffsets_FUN) (agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers, agpu_size* offsets);

AGPU_EXPORT agpu_error agpuAddVertexBindingReference(agpu_vertex_binding* vertex_binding);
AGPU_EXPORT agpu_error agpuReleaseVertexBinding(agpu_vertex_binding* vertex_binding);
AGPU_EXPORT agpu_error agpuBindVertexBuffers(agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers);
AGPU_EXPORT agpu_error agpuBindVertexBuffersWithOffsets(agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers, agpu_size* offsets);

/* Methods for interface agpu_vertex_layout. */
typedef agpu_error (*agpuAddVertexLayoutReference_FUN) (agpu_vertex_layout* vertex_layout);
typedef agpu_error (*agpuReleaseVertexLayout_FUN) (agpu_vertex_layout* vertex_layout);
typedef agpu_error (*agpuAddVertexAttributeBindings_FUN) (agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes);

AGPU_EXPORT agpu_error agpuAddVertexLayoutReference(agpu_vertex_layout* vertex_layout);
AGPU_EXPORT agpu_error agpuReleaseVertexLayout(agpu_vertex_layout* vertex_layout);
AGPU_EXPORT agpu_error agpuAddVertexAttributeBindings(agpu_vertex_layout* vertex_layout, agpu_uint vertex_buffer_count, agpu_size* vertex_strides, agpu_size attribute_count, agpu_vertex_attrib_description* attributes);

/* Methods for interface agpu_shader. */
typedef agpu_error (*agpuAddShaderReference_FUN) (agpu_shader* shader);
typedef agpu_error (*agpuReleaseShader_FUN) (agpu_shader* shader);
typedef agpu_error (*agpuSetShaderSource_FUN) (agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength);
typedef agpu_error (*agpuCompileShader_FUN) (agpu_shader* shader, agpu_cstring options);
typedef agpu_size (*agpuGetShaderCompilationLogLength_FUN) (agpu_shader* shader);
typedef agpu_error (*agpuGetShaderCompilationLog_FUN) (agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer);

AGPU_EXPORT agpu_error agpuAddShaderReference(agpu_shader* shader);
AGPU_EXPORT agpu_error agpuReleaseShader(agpu_shader* shader);
AGPU_EXPORT agpu_error agpuSetShaderSource(agpu_shader* shader, agpu_shader_language language, agpu_string sourceText, agpu_string_length sourceTextLength);
AGPU_EXPORT agpu_error agpuCompileShader(agpu_shader* shader, agpu_cstring options);
AGPU_EXPORT agpu_size agpuGetShaderCompilationLogLength(agpu_shader* shader);
AGPU_EXPORT agpu_error agpuGetShaderCompilationLog(agpu_shader* shader, agpu_size buffer_size, agpu_string_buffer buffer);

/* Methods for interface agpu_framebuffer. */
typedef agpu_error (*agpuAddFramebufferReference_FUN) (agpu_framebuffer* framebuffer);
typedef agpu_error (*agpuReleaseFramebuffer_FUN) (agpu_framebuffer* framebuffer);

AGPU_EXPORT agpu_error agpuAddFramebufferReference(agpu_framebuffer* framebuffer);
AGPU_EXPORT agpu_error agpuReleaseFramebuffer(agpu_framebuffer* framebuffer);

/* Methods for interface agpu_renderpass. */
typedef agpu_error (*agpuAddRenderPassReference_FUN) (agpu_renderpass* renderpass);
typedef agpu_error (*agpuReleaseRenderPass_FUN) (agpu_renderpass* renderpass);
typedef agpu_error (*agpuSetDepthStencilClearValue_FUN) (agpu_renderpass* renderpass, agpu_depth_stencil_value value);
typedef agpu_error (*agpuSetColorClearValue_FUN) (agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f value);
typedef agpu_error (*agpuSetColorClearValueFrom_FUN) (agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f* value);
typedef agpu_error (*agpuGetRenderPassColorAttachmentFormats_FUN) (agpu_renderpass* renderpass, agpu_uint* color_attachment_count, agpu_texture_format* formats);
typedef agpu_texture_format (*agpuGetRenderPassDepthStencilAttachmentFormat_FUN) (agpu_renderpass* renderpass);
typedef agpu_uint (*agpuGetRenderPassSampleCount_FUN) (agpu_renderpass* renderpass);
typedef agpu_uint (*agpuGetRenderPassSampleQuality_FUN) (agpu_renderpass* renderpass);

AGPU_EXPORT agpu_error agpuAddRenderPassReference(agpu_renderpass* renderpass);
AGPU_EXPORT agpu_error agpuReleaseRenderPass(agpu_renderpass* renderpass);
AGPU_EXPORT agpu_error agpuSetDepthStencilClearValue(agpu_renderpass* renderpass, agpu_depth_stencil_value value);
AGPU_EXPORT agpu_error agpuSetColorClearValue(agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f value);
AGPU_EXPORT agpu_error agpuSetColorClearValueFrom(agpu_renderpass* renderpass, agpu_uint attachment_index, agpu_color4f* value);
AGPU_EXPORT agpu_error agpuGetRenderPassColorAttachmentFormats(agpu_renderpass* renderpass, agpu_uint* color_attachment_count, agpu_texture_format* formats);
AGPU_EXPORT agpu_texture_format agpuGetRenderPassDepthStencilAttachmentFormat(agpu_renderpass* renderpass);
AGPU_EXPORT agpu_uint agpuGetRenderPassSampleCount(agpu_renderpass* renderpass);
AGPU_EXPORT agpu_uint agpuGetRenderPassSampleQuality(agpu_renderpass* renderpass);

/* Methods for interface agpu_shader_signature_builder. */
typedef agpu_error (*agpuAddShaderSignatureBuilderReference_FUN) (agpu_shader_signature_builder* shader_signature_builder);
typedef agpu_error (*agpuReleaseShaderSignatureBuilder_FUN) (agpu_shader_signature_builder* shader_signature_builder);
typedef agpu_shader_signature* (*agpuBuildShaderSignature_FUN) (agpu_shader_signature_builder* shader_signature_builder);
typedef agpu_error (*agpuAddShaderSignatureBindingConstant_FUN) (agpu_shader_signature_builder* shader_signature_builder);
typedef agpu_error (*agpuAddShaderSignatureBindingElement_FUN) (agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings);
typedef agpu_error (*agpuBeginShaderSignatureBindingBank_FUN) (agpu_shader_signature_builder* shader_signature_builder, agpu_uint maxBindings);
typedef agpu_error (*agpuAddShaderSignatureBindingBankElement_FUN) (agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount);

AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference(agpu_shader_signature_builder* shader_signature_builder);
AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder(agpu_shader_signature_builder* shader_signature_builder);
AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature(agpu_shader_signature_builder* shader_signature_builder);
AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant(agpu_shader_signature_builder* shader_signature_builder);
AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement(agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings);
AGPU_EXPORT agpu_error agpuBeginShaderSignatureBindingBank(agpu_shader_signature_builder* shader_signature_builder, agpu_uint maxBindings);
AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBankElement(agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount);

/* Methods for interface agpu_shader_signature. */
typedef agpu_error (*agpuAddShaderSignature_FUN) (agpu_shader_signature* shader_signature);
typedef agpu_error (*agpuReleaseShaderSignature_FUN) (agpu_shader_signature* shader_signature);
typedef agpu_shader_resource_binding* (*agpuCreateShaderResourceBinding_FUN) (agpu_shader_signature* shader_signature, agpu_uint element);

AGPU_EXPORT agpu_error agpuAddShaderSignature(agpu_shader_signature* shader_signature);
AGPU_EXPORT agpu_error agpuReleaseShaderSignature(agpu_shader_signature* shader_signature);
AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding(agpu_shader_signature* shader_signature, agpu_uint element);

/* Methods for interface agpu_shader_resource_binding. */
typedef agpu_error (*agpuAddShaderResourceBindingReference_FUN) (agpu_shader_resource_binding* shader_resource_binding);
typedef agpu_error (*agpuReleaseShaderResourceBinding_FUN) (agpu_shader_resource_binding* shader_resource_binding);
typedef agpu_error (*agpuBindUniformBuffer_FUN) (agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer);
typedef agpu_error (*agpuBindUniformBufferRange_FUN) (agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size);
typedef agpu_error (*agpuBindStorageBuffer_FUN) (agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer);
typedef agpu_error (*agpuBindStorageBufferRange_FUN) (agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size);
typedef agpu_error (*agpuBindSampledTextureView_FUN) (agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture_view* view);
typedef agpu_error (*agpuBindStorageImageView_FUN) (agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture_view* view);
typedef agpu_error (*agpuBindSampler_FUN) (agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler* sampler);

AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference(agpu_shader_resource_binding* shader_resource_binding);
AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding(agpu_shader_resource_binding* shader_resource_binding);
AGPU_EXPORT agpu_error agpuBindUniformBuffer(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer);
AGPU_EXPORT agpu_error agpuBindUniformBufferRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size);
AGPU_EXPORT agpu_error agpuBindStorageBuffer(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer);
AGPU_EXPORT agpu_error agpuBindStorageBufferRange(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* storage_buffer, agpu_size offset, agpu_size size);
AGPU_EXPORT agpu_error agpuBindSampledTextureView(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture_view* view);
AGPU_EXPORT agpu_error agpuBindStorageImageView(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture_view* view);
AGPU_EXPORT agpu_error agpuBindSampler(agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler* sampler);

/* Methods for interface agpu_fence. */
typedef agpu_error (*agpuAddFenceReference_FUN) (agpu_fence* fence);
typedef agpu_error (*agpuReleaseFenceReference_FUN) (agpu_fence* fence);
typedef agpu_error (*agpuWaitOnClient_FUN) (agpu_fence* fence);

AGPU_EXPORT agpu_error agpuAddFenceReference(agpu_fence* fence);
AGPU_EXPORT agpu_error agpuReleaseFenceReference(agpu_fence* fence);
AGPU_EXPORT agpu_error agpuWaitOnClient(agpu_fence* fence);

/* Methods for interface agpu_offline_shader_compiler. */
typedef agpu_error (*agpuAddOfflineShaderCompilerReference_FUN) (agpu_offline_shader_compiler* offline_shader_compiler);
typedef agpu_error (*agpuReleaseOfflineShaderCompiler_FUN) (agpu_offline_shader_compiler* offline_shader_compiler);
typedef agpu_bool (*agpuIsShaderLanguageSupportedByOfflineCompiler_FUN) (agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language);
typedef agpu_bool (*agpuIsTargetShaderLanguageSupportedByOfflineCompiler_FUN) (agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language);
typedef agpu_error (*agpuSetOfflineShaderCompilerSource_FUN) (agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language, agpu_shader_type stage, agpu_string sourceText, agpu_string_length sourceTextLength);
typedef agpu_error (*agpuCompileOfflineShader_FUN) (agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language target_language, agpu_cstring options);
typedef agpu_size (*agpuGetOfflineShaderCompilationLogLength_FUN) (agpu_offline_shader_compiler* offline_shader_compiler);
typedef agpu_error (*agpuGetOfflineShaderCompilationLog_FUN) (agpu_offline_shader_compiler* offline_shader_compiler, agpu_size buffer_size, agpu_string_buffer buffer);
typedef agpu_size (*agpuGetOfflineShaderCompilationResultLength_FUN) (agpu_offline_shader_compiler* offline_shader_compiler);
typedef agpu_error (*agpuGetOfflineShaderCompilationResult_FUN) (agpu_offline_shader_compiler* offline_shader_compiler, agpu_size buffer_size, agpu_string_buffer buffer);
typedef agpu_shader* (*agpuGetOfflineShaderCompilerResultAsShader_FUN) (agpu_offline_shader_compiler* offline_shader_compiler);

AGPU_EXPORT agpu_error agpuAddOfflineShaderCompilerReference(agpu_offline_shader_compiler* offline_shader_compiler);
AGPU_EXPORT agpu_error agpuReleaseOfflineShaderCompiler(agpu_offline_shader_compiler* offline_shader_compiler);
AGPU_EXPORT agpu_bool agpuIsShaderLanguageSupportedByOfflineCompiler(agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language);
AGPU_EXPORT agpu_bool agpuIsTargetShaderLanguageSupportedByOfflineCompiler(agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language);
AGPU_EXPORT agpu_error agpuSetOfflineShaderCompilerSource(agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language language, agpu_shader_type stage, agpu_string sourceText, agpu_string_length sourceTextLength);
AGPU_EXPORT agpu_error agpuCompileOfflineShader(agpu_offline_shader_compiler* offline_shader_compiler, agpu_shader_language target_language, agpu_cstring options);
AGPU_EXPORT agpu_size agpuGetOfflineShaderCompilationLogLength(agpu_offline_shader_compiler* offline_shader_compiler);
AGPU_EXPORT agpu_error agpuGetOfflineShaderCompilationLog(agpu_offline_shader_compiler* offline_shader_compiler, agpu_size buffer_size, agpu_string_buffer buffer);
AGPU_EXPORT agpu_size agpuGetOfflineShaderCompilationResultLength(agpu_offline_shader_compiler* offline_shader_compiler);
AGPU_EXPORT agpu_error agpuGetOfflineShaderCompilationResult(agpu_offline_shader_compiler* offline_shader_compiler, agpu_size buffer_size, agpu_string_buffer buffer);
AGPU_EXPORT agpu_shader* agpuGetOfflineShaderCompilerResultAsShader(agpu_offline_shader_compiler* offline_shader_compiler);

/* Methods for interface agpu_state_tracker_cache. */
typedef agpu_error (*agpuAddStateTrackerCacheReference_FUN) (agpu_state_tracker_cache* state_tracker_cache);
typedef agpu_error (*agpuReleaseStateTrackerCacheReference_FUN) (agpu_state_tracker_cache* state_tracker_cache);
typedef agpu_state_tracker* (*agpuCreateStateTracker_FUN) (agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue);
typedef agpu_state_tracker* (*agpuCreateStateTrackerWithCommandAllocator_FUN) (agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_command_allocator* command_allocator);
typedef agpu_state_tracker* (*agpuCreateStateTrackerWithFrameBuffering_FUN) (agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_uint framebuffering_count);
typedef agpu_immediate_renderer* (*agpuCreateImmediateRenderer_FUN) (agpu_state_tracker_cache* state_tracker_cache);

AGPU_EXPORT agpu_error agpuAddStateTrackerCacheReference(agpu_state_tracker_cache* state_tracker_cache);
AGPU_EXPORT agpu_error agpuReleaseStateTrackerCacheReference(agpu_state_tracker_cache* state_tracker_cache);
AGPU_EXPORT agpu_state_tracker* agpuCreateStateTracker(agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue);
AGPU_EXPORT agpu_state_tracker* agpuCreateStateTrackerWithCommandAllocator(agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_command_allocator* command_allocator);
AGPU_EXPORT agpu_state_tracker* agpuCreateStateTrackerWithFrameBuffering(agpu_state_tracker_cache* state_tracker_cache, agpu_command_list_type type, agpu_command_queue* command_queue, agpu_uint framebuffering_count);
AGPU_EXPORT agpu_immediate_renderer* agpuCreateImmediateRenderer(agpu_state_tracker_cache* state_tracker_cache);

/* Methods for interface agpu_state_tracker. */
typedef agpu_error (*agpuAddStateTrackerReference_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuReleaseStateTrackerReference_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerBeginRecordingCommands_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_command_list* (*agpuStateTrackerEndRecordingCommands_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerEndRecordingAndFlushCommands_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerReset_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerResetGraphicsPipeline_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerResetComputePipeline_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerSetComputeStage_FUN) (agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
typedef agpu_error (*agpuStateTrackerSetVertexStage_FUN) (agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
typedef agpu_error (*agpuStateTrackerSetFragmentStage_FUN) (agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
typedef agpu_error (*agpuStateTrackerSetGeometryStage_FUN) (agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
typedef agpu_error (*agpuStateTrackerSetTessellationControlStage_FUN) (agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
typedef agpu_error (*agpuStateTrackerSetTessellationEvaluationStage_FUN) (agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
typedef agpu_error (*agpuStateTrackerSetBlendState_FUN) (agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_bool enabled);
typedef agpu_error (*agpuStateTrackerSetBlendFunction_FUN) (agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation);
typedef agpu_error (*agpuStateTrackerSetColorMask_FUN) (agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled);
typedef agpu_error (*agpuStateTrackerSetFrontFace_FUN) (agpu_state_tracker* state_tracker, agpu_face_winding winding);
typedef agpu_error (*agpuStateTrackerSetCullMode_FUN) (agpu_state_tracker* state_tracker, agpu_cull_mode mode);
typedef agpu_error (*agpuStateTrackerSetDepthBias_FUN) (agpu_state_tracker* state_tracker, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor);
typedef agpu_error (*agpuStateTrackerSetDepthState_FUN) (agpu_state_tracker* state_tracker, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function);
typedef agpu_error (*agpuStateTrackerSetPolygonMode_FUN) (agpu_state_tracker* state_tracker, agpu_polygon_mode mode);
typedef agpu_error (*agpuStateTrackerSetStencilState_FUN) (agpu_state_tracker* state_tracker, agpu_bool enabled, agpu_int writeMask, agpu_int readMask);
typedef agpu_error (*agpuStateTrackerSetStencilFrontFace_FUN) (agpu_state_tracker* state_tracker, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
typedef agpu_error (*agpuStateTrackerSetStencilBackFace_FUN) (agpu_state_tracker* state_tracker, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
typedef agpu_error (*agpuStateTrackerSetPrimitiveType_FUN) (agpu_state_tracker* state_tracker, agpu_primitive_topology type);
typedef agpu_error (*agpuStateTrackerSetVertexLayout_FUN) (agpu_state_tracker* state_tracker, agpu_vertex_layout* layout);
typedef agpu_error (*agpuStateTrackerSetShaderSignature_FUN) (agpu_state_tracker* state_tracker, agpu_shader_signature* signature);
typedef agpu_error (*agpuStateTrackerSetSampleDescription_FUN) (agpu_state_tracker* state_tracker, agpu_uint sample_count, agpu_uint sample_quality);
typedef agpu_error (*agpuStateTrackerSetViewport_FUN) (agpu_state_tracker* state_tracker, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
typedef agpu_error (*agpuStateTrackerSetScissor_FUN) (agpu_state_tracker* state_tracker, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
typedef agpu_error (*agpuStateTrackerUseVertexBinding_FUN) (agpu_state_tracker* state_tracker, agpu_vertex_binding* vertex_binding);
typedef agpu_error (*agpuStateTrackerUseIndexBuffer_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* index_buffer);
typedef agpu_error (*agpuStateTrackerUseIndexBufferAt_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size);
typedef agpu_error (*agpuStateTrackerUseDrawIndirectBuffer_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* draw_buffer);
typedef agpu_error (*agpuStateTrackerUseComputeDispatchIndirectBuffer_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* buffer);
typedef agpu_error (*agpuStateTrackerUseShaderResources_FUN) (agpu_state_tracker* state_tracker, agpu_shader_resource_binding* binding);
typedef agpu_error (*agpuStateTrackerUseComputeShaderResources_FUN) (agpu_state_tracker* state_tracker, agpu_shader_resource_binding* binding);
typedef agpu_error (*agpuStateTrackerDrawArrays_FUN) (agpu_state_tracker* state_tracker, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance);
typedef agpu_error (*agpuStateTrackerDrawArraysIndirect_FUN) (agpu_state_tracker* state_tracker, agpu_size offset, agpu_size drawcount);
typedef agpu_error (*agpuStateTrackerDrawElements_FUN) (agpu_state_tracker* state_tracker, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
typedef agpu_error (*agpuStateTrackerDrawElementsIndirect_FUN) (agpu_state_tracker* state_tracker, agpu_size offset, agpu_size drawcount);
typedef agpu_error (*agpuStateTrackerDispatchCompute_FUN) (agpu_state_tracker* state_tracker, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z);
typedef agpu_error (*agpuStateTrackerDispatchComputeIndirect_FUN) (agpu_state_tracker* state_tracker, agpu_size offset);
typedef agpu_error (*agpuStateTrackerSetStencilReference_FUN) (agpu_state_tracker* state_tracker, agpu_uint reference);
typedef agpu_error (*agpuStateTrackerExecuteBundle_FUN) (agpu_state_tracker* state_tracker, agpu_command_list* bundle);
typedef agpu_error (*agpuStateTrackerBeginRenderPass_FUN) (agpu_state_tracker* state_tracker, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content);
typedef agpu_error (*agpuStateTrackerEndRenderPass_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerResolveFramebuffer_FUN) (agpu_state_tracker* state_tracker, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer);
typedef agpu_error (*agpuStateTrackerResolveTexture_FUN) (agpu_state_tracker* state_tracker, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect);
typedef agpu_error (*agpuStateTrackerPushConstants_FUN) (agpu_state_tracker* state_tracker, agpu_uint offset, agpu_uint size, agpu_pointer values);
typedef agpu_error (*agpuStateTrackerMemoryBarrier_FUN) (agpu_state_tracker* state_tracker, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses);
typedef agpu_error (*agpuStateTrackerBufferMemoryBarrier_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size);
typedef agpu_error (*agpuStateTrackerTextureMemoryBarrier_FUN) (agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range);
typedef agpu_error (*agpuStateTrackerPushBufferTransitionBarrier_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage);
typedef agpu_error (*agpuStateTrackerPushTextureTransitionBarrier_FUN) (agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range);
typedef agpu_error (*agpuStateTrackerPopBufferTransitionBarrier_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerPopTextureTransitionBarrier_FUN) (agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuStateTrackerCopyBuffer_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size);
typedef agpu_error (*agpuStateTrackerCopyBufferToTexture_FUN) (agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region);
typedef agpu_error (*agpuStateTrackerCopyTextureToBuffer_FUN) (agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region);

AGPU_EXPORT agpu_error agpuAddStateTrackerReference(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuReleaseStateTrackerReference(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerBeginRecordingCommands(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_command_list* agpuStateTrackerEndRecordingCommands(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerEndRecordingAndFlushCommands(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerReset(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerResetGraphicsPipeline(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerResetComputePipeline(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerSetComputeStage(agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
AGPU_EXPORT agpu_error agpuStateTrackerSetVertexStage(agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
AGPU_EXPORT agpu_error agpuStateTrackerSetFragmentStage(agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
AGPU_EXPORT agpu_error agpuStateTrackerSetGeometryStage(agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
AGPU_EXPORT agpu_error agpuStateTrackerSetTessellationControlStage(agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
AGPU_EXPORT agpu_error agpuStateTrackerSetTessellationEvaluationStage(agpu_state_tracker* state_tracker, agpu_shader* shader, agpu_cstring entryPoint);
AGPU_EXPORT agpu_error agpuStateTrackerSetBlendState(agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_bool enabled);
AGPU_EXPORT agpu_error agpuStateTrackerSetBlendFunction(agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation);
AGPU_EXPORT agpu_error agpuStateTrackerSetColorMask(agpu_state_tracker* state_tracker, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled);
AGPU_EXPORT agpu_error agpuStateTrackerSetFrontFace(agpu_state_tracker* state_tracker, agpu_face_winding winding);
AGPU_EXPORT agpu_error agpuStateTrackerSetCullMode(agpu_state_tracker* state_tracker, agpu_cull_mode mode);
AGPU_EXPORT agpu_error agpuStateTrackerSetDepthBias(agpu_state_tracker* state_tracker, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor);
AGPU_EXPORT agpu_error agpuStateTrackerSetDepthState(agpu_state_tracker* state_tracker, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function);
AGPU_EXPORT agpu_error agpuStateTrackerSetPolygonMode(agpu_state_tracker* state_tracker, agpu_polygon_mode mode);
AGPU_EXPORT agpu_error agpuStateTrackerSetStencilState(agpu_state_tracker* state_tracker, agpu_bool enabled, agpu_int writeMask, agpu_int readMask);
AGPU_EXPORT agpu_error agpuStateTrackerSetStencilFrontFace(agpu_state_tracker* state_tracker, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
AGPU_EXPORT agpu_error agpuStateTrackerSetStencilBackFace(agpu_state_tracker* state_tracker, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
AGPU_EXPORT agpu_error agpuStateTrackerSetPrimitiveType(agpu_state_tracker* state_tracker, agpu_primitive_topology type);
AGPU_EXPORT agpu_error agpuStateTrackerSetVertexLayout(agpu_state_tracker* state_tracker, agpu_vertex_layout* layout);
AGPU_EXPORT agpu_error agpuStateTrackerSetShaderSignature(agpu_state_tracker* state_tracker, agpu_shader_signature* signature);
AGPU_EXPORT agpu_error agpuStateTrackerSetSampleDescription(agpu_state_tracker* state_tracker, agpu_uint sample_count, agpu_uint sample_quality);
AGPU_EXPORT agpu_error agpuStateTrackerSetViewport(agpu_state_tracker* state_tracker, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
AGPU_EXPORT agpu_error agpuStateTrackerSetScissor(agpu_state_tracker* state_tracker, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
AGPU_EXPORT agpu_error agpuStateTrackerUseVertexBinding(agpu_state_tracker* state_tracker, agpu_vertex_binding* vertex_binding);
AGPU_EXPORT agpu_error agpuStateTrackerUseIndexBuffer(agpu_state_tracker* state_tracker, agpu_buffer* index_buffer);
AGPU_EXPORT agpu_error agpuStateTrackerUseIndexBufferAt(agpu_state_tracker* state_tracker, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size);
AGPU_EXPORT agpu_error agpuStateTrackerUseDrawIndirectBuffer(agpu_state_tracker* state_tracker, agpu_buffer* draw_buffer);
AGPU_EXPORT agpu_error agpuStateTrackerUseComputeDispatchIndirectBuffer(agpu_state_tracker* state_tracker, agpu_buffer* buffer);
AGPU_EXPORT agpu_error agpuStateTrackerUseShaderResources(agpu_state_tracker* state_tracker, agpu_shader_resource_binding* binding);
AGPU_EXPORT agpu_error agpuStateTrackerUseComputeShaderResources(agpu_state_tracker* state_tracker, agpu_shader_resource_binding* binding);
AGPU_EXPORT agpu_error agpuStateTrackerDrawArrays(agpu_state_tracker* state_tracker, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance);
AGPU_EXPORT agpu_error agpuStateTrackerDrawArraysIndirect(agpu_state_tracker* state_tracker, agpu_size offset, agpu_size drawcount);
AGPU_EXPORT agpu_error agpuStateTrackerDrawElements(agpu_state_tracker* state_tracker, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
AGPU_EXPORT agpu_error agpuStateTrackerDrawElementsIndirect(agpu_state_tracker* state_tracker, agpu_size offset, agpu_size drawcount);
AGPU_EXPORT agpu_error agpuStateTrackerDispatchCompute(agpu_state_tracker* state_tracker, agpu_uint group_count_x, agpu_uint group_count_y, agpu_uint group_count_z);
AGPU_EXPORT agpu_error agpuStateTrackerDispatchComputeIndirect(agpu_state_tracker* state_tracker, agpu_size offset);
AGPU_EXPORT agpu_error agpuStateTrackerSetStencilReference(agpu_state_tracker* state_tracker, agpu_uint reference);
AGPU_EXPORT agpu_error agpuStateTrackerExecuteBundle(agpu_state_tracker* state_tracker, agpu_command_list* bundle);
AGPU_EXPORT agpu_error agpuStateTrackerBeginRenderPass(agpu_state_tracker* state_tracker, agpu_renderpass* renderpass, agpu_framebuffer* framebuffer, agpu_bool bundle_content);
AGPU_EXPORT agpu_error agpuStateTrackerEndRenderPass(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerResolveFramebuffer(agpu_state_tracker* state_tracker, agpu_framebuffer* destFramebuffer, agpu_framebuffer* sourceFramebuffer);
AGPU_EXPORT agpu_error agpuStateTrackerResolveTexture(agpu_state_tracker* state_tracker, agpu_texture* sourceTexture, agpu_uint sourceLevel, agpu_uint sourceLayer, agpu_texture* destTexture, agpu_uint destLevel, agpu_uint destLayer, agpu_uint levelCount, agpu_uint layerCount, agpu_texture_aspect aspect);
AGPU_EXPORT agpu_error agpuStateTrackerPushConstants(agpu_state_tracker* state_tracker, agpu_uint offset, agpu_uint size, agpu_pointer values);
AGPU_EXPORT agpu_error agpuStateTrackerMemoryBarrier(agpu_state_tracker* state_tracker, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses);
AGPU_EXPORT agpu_error agpuStateTrackerBufferMemoryBarrier(agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_size offset, agpu_size size);
AGPU_EXPORT agpu_error agpuStateTrackerTextureMemoryBarrier(agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_pipeline_stage_flags source_stage, agpu_pipeline_stage_flags dest_stage, agpu_access_flags source_accesses, agpu_access_flags dest_accesses, agpu_subresource_range* subresource_range);
AGPU_EXPORT agpu_error agpuStateTrackerPushBufferTransitionBarrier(agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_buffer_usage_mask new_usage);
AGPU_EXPORT agpu_error agpuStateTrackerPushTextureTransitionBarrier(agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_texture_usage_mode_mask new_usage, agpu_subresource_range* subresource_range);
AGPU_EXPORT agpu_error agpuStateTrackerPopBufferTransitionBarrier(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerPopTextureTransitionBarrier(agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuStateTrackerCopyBuffer(agpu_state_tracker* state_tracker, agpu_buffer* source_buffer, agpu_size source_offset, agpu_buffer* dest_buffer, agpu_size dest_offset, agpu_size copy_size);
AGPU_EXPORT agpu_error agpuStateTrackerCopyBufferToTexture(agpu_state_tracker* state_tracker, agpu_buffer* buffer, agpu_texture* texture, agpu_buffer_image_copy_region* copy_region);
AGPU_EXPORT agpu_error agpuStateTrackerCopyTextureToBuffer(agpu_state_tracker* state_tracker, agpu_texture* texture, agpu_buffer* buffer, agpu_buffer_image_copy_region* copy_region);

/* Methods for interface agpu_immediate_renderer. */
typedef agpu_error (*agpuAddImmediateRendererReference_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuReleaseImmediateRendererReference_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuBeginImmediateRendering_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_state_tracker* state_tracker);
typedef agpu_error (*agpuEndImmediateRendering_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererSetBlendState_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_bool enabled);
typedef agpu_error (*agpuImmediateRendererSetBlendFunction_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation);
typedef agpu_error (*agpuImmediateRendererSetColorMask_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled);
typedef agpu_error (*agpuImmediateRendererSetFrontFace_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_face_winding winding);
typedef agpu_error (*agpuImmediateRendererSetCullMode_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_cull_mode mode);
typedef agpu_error (*agpuImmediateRendererSetDepthBias_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor);
typedef agpu_error (*agpuImmediateRendererSetDepthState_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function);
typedef agpu_error (*agpuImmediateRendererSetPolygonMode_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_polygon_mode mode);
typedef agpu_error (*agpuImmediateRendererSetStencilState_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_bool enabled, agpu_int writeMask, agpu_int readMask);
typedef agpu_error (*agpuImmediateRendererSetStencilFrontFace_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
typedef agpu_error (*agpuImmediateRendererSetStencilBackFace_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
typedef agpu_error (*agpuImmediateRendererSetViewport_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
typedef agpu_error (*agpuImmediateRendererSetScissor_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
typedef agpu_error (*agpuImmediateRendererSetStencilReference_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_uint reference);
typedef agpu_error (*agpuImmediateRendererProjectionMatrixMode_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererModelViewMatrixMode_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererTextureMatrixMode_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererIdentity_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererPushMatrix_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererPopMatrix_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererLoadMatrix_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
typedef agpu_error (*agpuImmediateRendererLoadTransposeMatrix_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
typedef agpu_error (*agpuImmediateRendererMultiplyMatrix_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
typedef agpu_error (*agpuImmediateRendererMultiplyTransposeMatrix_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
typedef agpu_error (*agpuImmediateRendererOrtho_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far);
typedef agpu_error (*agpuImmediateRendererFrustum_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far);
typedef agpu_error (*agpuImmediateRendererPerspective_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far);
typedef agpu_error (*agpuImmediateRendererRotate_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float angle, agpu_float x, agpu_float y, agpu_float z);
typedef agpu_error (*agpuImmediateRendererTranslate_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
typedef agpu_error (*agpuImmediateRendererScale_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
typedef agpu_error (*agpuImmediateRendererSetFlatShading_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_bool enabled);
typedef agpu_error (*agpuImmediateRendererSetLightingEnabled_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_bool enabled);
typedef agpu_error (*agpuImmediateRendererClearLights_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuImmediateRendererSetAmbientLighting_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a);
typedef agpu_error (*agpuImmediateRendererSetLight_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state);
typedef agpu_error (*agpuImmediateRendererSetMaterial_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_immediate_renderer_material* state);
typedef agpu_error (*agpuImmediateRendererSetTextureEnabled_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_bool enabled);
typedef agpu_error (*agpuImmediateRendererBindTexture_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_texture* texture);
typedef agpu_error (*agpuImmediateRendererSetClipPlane_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_uint index, agpu_bool enabled, agpu_float p1, agpu_float p2, agpu_float p3, agpu_float p4);
typedef agpu_error (*agpuImmediateRendererSetFogMode_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_immediate_renderer_fog_mode mode);
typedef agpu_error (*agpuImmediateRendererSetFogColor_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a);
typedef agpu_error (*agpuImmediateRendererSetFogDistances_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float start, agpu_float end);
typedef agpu_error (*agpuImmediateRendererSetFogDensity_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float density);
typedef agpu_error (*agpuBeginImmediateRendererPrimitives_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology type);
typedef agpu_error (*agpuEndImmediateRendererPrimitives_FUN) (agpu_immediate_renderer* immediate_renderer);
typedef agpu_error (*agpuSetImmediateRendererColor_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a);
typedef agpu_error (*agpuSetImmediateRendererTexcoord_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y);
typedef agpu_error (*agpuSetImmediateRendererNormal_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
typedef agpu_error (*agpuAddImmediateRendererVertex_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
typedef agpu_error (*agpuBeginImmediateRendererMeshWithVertices_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer vertices);
typedef agpu_error (*agpuBeginImmediateRendererMeshWithVertexBinding_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_vertex_layout* layout, agpu_vertex_binding* vertices);
typedef agpu_error (*agpuImmediateRendererUseIndexBuffer_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_buffer* index_buffer);
typedef agpu_error (*agpuImmediateRendererUseIndexBufferAt_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size);
typedef agpu_error (*agpuSetImmediateRendererCurrentMeshColors_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer colors);
typedef agpu_error (*agpuSetImmediateRendererCurrentMeshNormals_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer normals);
typedef agpu_error (*agpuSetImmediateRendererCurrentMeshTexCoords_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer texcoords);
typedef agpu_error (*agpuImmediateRendererSetPrimitiveType_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology type);
typedef agpu_error (*agpuImmediateRendererDrawArrays_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance);
typedef agpu_error (*agpuImmediateRendererDrawElements_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
typedef agpu_error (*agpuImmediateRendererDrawElementsWithIndices_FUN) (agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology mode, agpu_pointer indices, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
typedef agpu_error (*agpuEndImmediateRendererMesh_FUN) (agpu_immediate_renderer* immediate_renderer);

AGPU_EXPORT agpu_error agpuAddImmediateRendererReference(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuReleaseImmediateRendererReference(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuBeginImmediateRendering(agpu_immediate_renderer* immediate_renderer, agpu_state_tracker* state_tracker);
AGPU_EXPORT agpu_error agpuEndImmediateRendering(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererSetBlendState(agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_bool enabled);
AGPU_EXPORT agpu_error agpuImmediateRendererSetBlendFunction(agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation);
AGPU_EXPORT agpu_error agpuImmediateRendererSetColorMask(agpu_immediate_renderer* immediate_renderer, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled);
AGPU_EXPORT agpu_error agpuImmediateRendererSetFrontFace(agpu_immediate_renderer* immediate_renderer, agpu_face_winding winding);
AGPU_EXPORT agpu_error agpuImmediateRendererSetCullMode(agpu_immediate_renderer* immediate_renderer, agpu_cull_mode mode);
AGPU_EXPORT agpu_error agpuImmediateRendererSetDepthBias(agpu_immediate_renderer* immediate_renderer, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor);
AGPU_EXPORT agpu_error agpuImmediateRendererSetDepthState(agpu_immediate_renderer* immediate_renderer, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function);
AGPU_EXPORT agpu_error agpuImmediateRendererSetPolygonMode(agpu_immediate_renderer* immediate_renderer, agpu_polygon_mode mode);
AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilState(agpu_immediate_renderer* immediate_renderer, agpu_bool enabled, agpu_int writeMask, agpu_int readMask);
AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilFrontFace(agpu_immediate_renderer* immediate_renderer, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilBackFace(agpu_immediate_renderer* immediate_renderer, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction);
AGPU_EXPORT agpu_error agpuImmediateRendererSetViewport(agpu_immediate_renderer* immediate_renderer, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
AGPU_EXPORT agpu_error agpuImmediateRendererSetScissor(agpu_immediate_renderer* immediate_renderer, agpu_int x, agpu_int y, agpu_int w, agpu_int h);
AGPU_EXPORT agpu_error agpuImmediateRendererSetStencilReference(agpu_immediate_renderer* immediate_renderer, agpu_uint reference);
AGPU_EXPORT agpu_error agpuImmediateRendererProjectionMatrixMode(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererModelViewMatrixMode(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererTextureMatrixMode(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererIdentity(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererPushMatrix(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererPopMatrix(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererLoadMatrix(agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
AGPU_EXPORT agpu_error agpuImmediateRendererLoadTransposeMatrix(agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
AGPU_EXPORT agpu_error agpuImmediateRendererMultiplyMatrix(agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
AGPU_EXPORT agpu_error agpuImmediateRendererMultiplyTransposeMatrix(agpu_immediate_renderer* immediate_renderer, agpu_float* elements);
AGPU_EXPORT agpu_error agpuImmediateRendererOrtho(agpu_immediate_renderer* immediate_renderer, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far);
AGPU_EXPORT agpu_error agpuImmediateRendererFrustum(agpu_immediate_renderer* immediate_renderer, agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far);
AGPU_EXPORT agpu_error agpuImmediateRendererPerspective(agpu_immediate_renderer* immediate_renderer, agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far);
AGPU_EXPORT agpu_error agpuImmediateRendererRotate(agpu_immediate_renderer* immediate_renderer, agpu_float angle, agpu_float x, agpu_float y, agpu_float z);
AGPU_EXPORT agpu_error agpuImmediateRendererTranslate(agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
AGPU_EXPORT agpu_error agpuImmediateRendererScale(agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
AGPU_EXPORT agpu_error agpuImmediateRendererSetFlatShading(agpu_immediate_renderer* immediate_renderer, agpu_bool enabled);
AGPU_EXPORT agpu_error agpuImmediateRendererSetLightingEnabled(agpu_immediate_renderer* immediate_renderer, agpu_bool enabled);
AGPU_EXPORT agpu_error agpuImmediateRendererClearLights(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuImmediateRendererSetAmbientLighting(agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a);
AGPU_EXPORT agpu_error agpuImmediateRendererSetLight(agpu_immediate_renderer* immediate_renderer, agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state);
AGPU_EXPORT agpu_error agpuImmediateRendererSetMaterial(agpu_immediate_renderer* immediate_renderer, agpu_immediate_renderer_material* state);
AGPU_EXPORT agpu_error agpuImmediateRendererSetTextureEnabled(agpu_immediate_renderer* immediate_renderer, agpu_bool enabled);
AGPU_EXPORT agpu_error agpuImmediateRendererBindTexture(agpu_immediate_renderer* immediate_renderer, agpu_texture* texture);
AGPU_EXPORT agpu_error agpuImmediateRendererSetClipPlane(agpu_immediate_renderer* immediate_renderer, agpu_uint index, agpu_bool enabled, agpu_float p1, agpu_float p2, agpu_float p3, agpu_float p4);
AGPU_EXPORT agpu_error agpuImmediateRendererSetFogMode(agpu_immediate_renderer* immediate_renderer, agpu_immediate_renderer_fog_mode mode);
AGPU_EXPORT agpu_error agpuImmediateRendererSetFogColor(agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a);
AGPU_EXPORT agpu_error agpuImmediateRendererSetFogDistances(agpu_immediate_renderer* immediate_renderer, agpu_float start, agpu_float end);
AGPU_EXPORT agpu_error agpuImmediateRendererSetFogDensity(agpu_immediate_renderer* immediate_renderer, agpu_float density);
AGPU_EXPORT agpu_error agpuBeginImmediateRendererPrimitives(agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology type);
AGPU_EXPORT agpu_error agpuEndImmediateRendererPrimitives(agpu_immediate_renderer* immediate_renderer);
AGPU_EXPORT agpu_error agpuSetImmediateRendererColor(agpu_immediate_renderer* immediate_renderer, agpu_float r, agpu_float g, agpu_float b, agpu_float a);
AGPU_EXPORT agpu_error agpuSetImmediateRendererTexcoord(agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y);
AGPU_EXPORT agpu_error agpuSetImmediateRendererNormal(agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
AGPU_EXPORT agpu_error agpuAddImmediateRendererVertex(agpu_immediate_renderer* immediate_renderer, agpu_float x, agpu_float y, agpu_float z);
AGPU_EXPORT agpu_error agpuBeginImmediateRendererMeshWithVertices(agpu_immediate_renderer* immediate_renderer, agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer vertices);
AGPU_EXPORT agpu_error agpuBeginImmediateRendererMeshWithVertexBinding(agpu_immediate_renderer* immediate_renderer, agpu_vertex_layout* layout, agpu_vertex_binding* vertices);
AGPU_EXPORT agpu_error agpuImmediateRendererUseIndexBuffer(agpu_immediate_renderer* immediate_renderer, agpu_buffer* index_buffer);
AGPU_EXPORT agpu_error agpuImmediateRendererUseIndexBufferAt(agpu_immediate_renderer* immediate_renderer, agpu_buffer* index_buffer, agpu_size offset, agpu_size index_size);
AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshColors(agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer colors);
AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshNormals(agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer normals);
AGPU_EXPORT agpu_error agpuSetImmediateRendererCurrentMeshTexCoords(agpu_immediate_renderer* immediate_renderer, agpu_size stride, agpu_size elementCount, agpu_pointer texcoords);
AGPU_EXPORT agpu_error agpuImmediateRendererSetPrimitiveType(agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology type);
AGPU_EXPORT agpu_error agpuImmediateRendererDrawArrays(agpu_immediate_renderer* immediate_renderer, agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance);
AGPU_EXPORT agpu_error agpuImmediateRendererDrawElements(agpu_immediate_renderer* immediate_renderer, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
AGPU_EXPORT agpu_error agpuImmediateRendererDrawElementsWithIndices(agpu_immediate_renderer* immediate_renderer, agpu_primitive_topology mode, agpu_pointer indices, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance);
AGPU_EXPORT agpu_error agpuEndImmediateRendererMesh(agpu_immediate_renderer* immediate_renderer);

/* Installable client driver interface. */
typedef struct _agpu_icd_dispatch {
	int icd_interface_version;
	agpuGetPlatforms_FUN agpuGetPlatforms;
	agpuOpenDevice_FUN agpuOpenDevice;
	agpuGetPlatformName_FUN agpuGetPlatformName;
	agpuGetPlatformGpuCount_FUN agpuGetPlatformGpuCount;
	agpuGetPlatformGpuName_FUN agpuGetPlatformGpuName;
	agpuGetPlatformVersion_FUN agpuGetPlatformVersion;
	agpuGetPlatformImplementationVersion_FUN agpuGetPlatformImplementationVersion;
	agpuPlatformHasRealMultithreading_FUN agpuPlatformHasRealMultithreading;
	agpuIsNativePlatform_FUN agpuIsNativePlatform;
	agpuIsCrossPlatform_FUN agpuIsCrossPlatform;
	agpuCreateOfflineShaderCompiler_FUN agpuCreateOfflineShaderCompiler;
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
	agpuCreateComputePipelineBuilder_FUN agpuCreateComputePipelineBuilder;
	agpuCreateCommandAllocator_FUN agpuCreateCommandAllocator;
	agpuCreateCommandList_FUN agpuCreateCommandList;
	agpuGetPreferredShaderLanguage_FUN agpuGetPreferredShaderLanguage;
	agpuGetPreferredIntermediateShaderLanguage_FUN agpuGetPreferredIntermediateShaderLanguage;
	agpuGetPreferredHighLevelShaderLanguage_FUN agpuGetPreferredHighLevelShaderLanguage;
	agpuCreateFrameBuffer_FUN agpuCreateFrameBuffer;
	agpuCreateRenderPass_FUN agpuCreateRenderPass;
	agpuCreateTexture_FUN agpuCreateTexture;
	agpuCreateSampler_FUN agpuCreateSampler;
	agpuCreateFence_FUN agpuCreateFence;
	agpuGetMultiSampleQualityLevels_FUN agpuGetMultiSampleQualityLevels;
	agpuHasTopLeftNdcOrigin_FUN agpuHasTopLeftNdcOrigin;
	agpuHasBottomLeftTextureCoordinates_FUN agpuHasBottomLeftTextureCoordinates;
	agpuIsFeatureSupportedOnDevice_FUN agpuIsFeatureSupportedOnDevice;
	agpuGetVRSystem_FUN agpuGetVRSystem;
	agpuCreateOfflineShaderCompilerForDevice_FUN agpuCreateOfflineShaderCompilerForDevice;
	agpuCreateStateTrackerCache_FUN agpuCreateStateTrackerCache;
	agpuFinishDeviceExecution_FUN agpuFinishDeviceExecution;
	agpuAddVRSystemReference_FUN agpuAddVRSystemReference;
	agpuReleaseVRSystem_FUN agpuReleaseVRSystem;
	agpuGetVRSystemName_FUN agpuGetVRSystemName;
	agpuGetVRSystemNativeHandle_FUN agpuGetVRSystemNativeHandle;
	agpuGetVRRecommendedRenderTargetSize_FUN agpuGetVRRecommendedRenderTargetSize;
	agpuGetVREyeToHeadTransformInto_FUN agpuGetVREyeToHeadTransformInto;
	agpuGetVRProjectionMatrix_FUN agpuGetVRProjectionMatrix;
	agpuGetVRProjectionFrustumTangents_FUN agpuGetVRProjectionFrustumTangents;
	agpuSubmitVREyeRenderTargets_FUN agpuSubmitVREyeRenderTargets;
	agpuWaitAndFetchVRPoses_FUN agpuWaitAndFetchVRPoses;
	agpuGetMaxVRTrackedDevicePoseCount_FUN agpuGetMaxVRTrackedDevicePoseCount;
	agpuGetCurrentVRTrackedDevicePoseCount_FUN agpuGetCurrentVRTrackedDevicePoseCount;
	agpuGetCurrentVRTrackedDevicePoseInto_FUN agpuGetCurrentVRTrackedDevicePoseInto;
	agpuGetMaxVRRenderTrackedDevicePoseCount_FUN agpuGetMaxVRRenderTrackedDevicePoseCount;
	agpuGetCurrentVRRenderTrackedDevicePoseCount_FUN agpuGetCurrentVRRenderTrackedDevicePoseCount;
	agpuGetCurrentVRRenderTrackedDevicePoseInto_FUN agpuGetCurrentVRRenderTrackedDevicePoseInto;
	agpuPollVREvent_FUN agpuPollVREvent;
	agpuAddSwapChainReference_FUN agpuAddSwapChainReference;
	agpuReleaseSwapChain_FUN agpuReleaseSwapChain;
	agpuSwapBuffers_FUN agpuSwapBuffers;
	agpuGetCurrentBackBuffer_FUN agpuGetCurrentBackBuffer;
	agpuGetCurrentBackBufferIndex_FUN agpuGetCurrentBackBufferIndex;
	agpuGetFramebufferCount_FUN agpuGetFramebufferCount;
	agpuSetSwapChainOverlayPosition_FUN agpuSetSwapChainOverlayPosition;
	agpuAddComputePipelineBuilderReference_FUN agpuAddComputePipelineBuilderReference;
	agpuReleaseComputePipelineBuilder_FUN agpuReleaseComputePipelineBuilder;
	agpuBuildComputePipelineState_FUN agpuBuildComputePipelineState;
	agpuAttachComputeShader_FUN agpuAttachComputeShader;
	agpuAttachComputeShaderWithEntryPoint_FUN agpuAttachComputeShaderWithEntryPoint;
	agpuGetComputePipelineBuildingLogLength_FUN agpuGetComputePipelineBuildingLogLength;
	agpuGetComputePipelineBuildingLog_FUN agpuGetComputePipelineBuildingLog;
	agpuSetComputePipelineShaderSignature_FUN agpuSetComputePipelineShaderSignature;
	agpuAddPipelineBuilderReference_FUN agpuAddPipelineBuilderReference;
	agpuReleasePipelineBuilder_FUN agpuReleasePipelineBuilder;
	agpuBuildPipelineState_FUN agpuBuildPipelineState;
	agpuAttachShader_FUN agpuAttachShader;
	agpuAttachShaderWithEntryPoint_FUN agpuAttachShaderWithEntryPoint;
	agpuGetPipelineBuildingLogLength_FUN agpuGetPipelineBuildingLogLength;
	agpuGetPipelineBuildingLog_FUN agpuGetPipelineBuildingLog;
	agpuSetBlendState_FUN agpuSetBlendState;
	agpuSetBlendFunction_FUN agpuSetBlendFunction;
	agpuSetColorMask_FUN agpuSetColorMask;
	agpuSetFrontFace_FUN agpuSetFrontFace;
	agpuSetCullMode_FUN agpuSetCullMode;
	agpuSetDepthBias_FUN agpuSetDepthBias;
	agpuSetDepthState_FUN agpuSetDepthState;
	agpuSetPolygonMode_FUN agpuSetPolygonMode;
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
	agpuUsePipelineState_FUN agpuUsePipelineState;
	agpuUseVertexBinding_FUN agpuUseVertexBinding;
	agpuUseIndexBuffer_FUN agpuUseIndexBuffer;
	agpuUseIndexBufferAt_FUN agpuUseIndexBufferAt;
	agpuUseDrawIndirectBuffer_FUN agpuUseDrawIndirectBuffer;
	agpuUseComputeDispatchIndirectBuffer_FUN agpuUseComputeDispatchIndirectBuffer;
	agpuUseShaderResources_FUN agpuUseShaderResources;
	agpuUseComputeShaderResources_FUN agpuUseComputeShaderResources;
	agpuDrawArrays_FUN agpuDrawArrays;
	agpuDrawArraysIndirect_FUN agpuDrawArraysIndirect;
	agpuDrawElements_FUN agpuDrawElements;
	agpuDrawElementsIndirect_FUN agpuDrawElementsIndirect;
	agpuDispatchCompute_FUN agpuDispatchCompute;
	agpuDispatchComputeIndirect_FUN agpuDispatchComputeIndirect;
	agpuSetStencilReference_FUN agpuSetStencilReference;
	agpuExecuteBundle_FUN agpuExecuteBundle;
	agpuCloseCommandList_FUN agpuCloseCommandList;
	agpuResetCommandList_FUN agpuResetCommandList;
	agpuResetBundleCommandList_FUN agpuResetBundleCommandList;
	agpuBeginRenderPass_FUN agpuBeginRenderPass;
	agpuEndRenderPass_FUN agpuEndRenderPass;
	agpuResolveFramebuffer_FUN agpuResolveFramebuffer;
	agpuResolveTexture_FUN agpuResolveTexture;
	agpuPushConstants_FUN agpuPushConstants;
	agpuMemoryBarrier_FUN agpuMemoryBarrier;
	agpuBufferMemoryBarrier_FUN agpuBufferMemoryBarrier;
	agpuTextureMemoryBarrier_FUN agpuTextureMemoryBarrier;
	agpuPushBufferTransitionBarrier_FUN agpuPushBufferTransitionBarrier;
	agpuPushTextureTransitionBarrier_FUN agpuPushTextureTransitionBarrier;
	agpuPopBufferTransitionBarrier_FUN agpuPopBufferTransitionBarrier;
	agpuPopTextureTransitionBarrier_FUN agpuPopTextureTransitionBarrier;
	agpuCopyBuffer_FUN agpuCopyBuffer;
	agpuCopyBufferToTexture_FUN agpuCopyBufferToTexture;
	agpuCopyTextureToBuffer_FUN agpuCopyTextureToBuffer;
	agpuAddTextureReference_FUN agpuAddTextureReference;
	agpuReleaseTexture_FUN agpuReleaseTexture;
	agpuGetTextureDescription_FUN agpuGetTextureDescription;
	agpuMapTextureLevel_FUN agpuMapTextureLevel;
	agpuUnmapTextureLevel_FUN agpuUnmapTextureLevel;
	agpuReadTextureData_FUN agpuReadTextureData;
	agpuReadTextureSubData_FUN agpuReadTextureSubData;
	agpuUploadTextureData_FUN agpuUploadTextureData;
	agpuUploadTextureSubData_FUN agpuUploadTextureSubData;
	agpuGetTextureFullViewDescription_FUN agpuGetTextureFullViewDescription;
	agpuCreateTextureView_FUN agpuCreateTextureView;
	agpuGetOrCreateFullTextureView_FUN agpuGetOrCreateFullTextureView;
	agpuAddTextureViewReference_FUN agpuAddTextureViewReference;
	agpuReleaseTextureView_FUN agpuReleaseTextureView;
	agpuGetTextureFromView_FUN agpuGetTextureFromView;
	agpuAddSamplerReference_FUN agpuAddSamplerReference;
	agpuReleaseSampler_FUN agpuReleaseSampler;
	agpuAddBufferReference_FUN agpuAddBufferReference;
	agpuReleaseBuffer_FUN agpuReleaseBuffer;
	agpuMapBuffer_FUN agpuMapBuffer;
	agpuUnmapBuffer_FUN agpuUnmapBuffer;
	agpuGetBufferDescription_FUN agpuGetBufferDescription;
	agpuUploadBufferData_FUN agpuUploadBufferData;
	agpuReadBufferData_FUN agpuReadBufferData;
	agpuFlushWholeBuffer_FUN agpuFlushWholeBuffer;
	agpuInvalidateWholeBuffer_FUN agpuInvalidateWholeBuffer;
	agpuAddVertexBindingReference_FUN agpuAddVertexBindingReference;
	agpuReleaseVertexBinding_FUN agpuReleaseVertexBinding;
	agpuBindVertexBuffers_FUN agpuBindVertexBuffers;
	agpuBindVertexBuffersWithOffsets_FUN agpuBindVertexBuffersWithOffsets;
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
	agpuAddRenderPassReference_FUN agpuAddRenderPassReference;
	agpuReleaseRenderPass_FUN agpuReleaseRenderPass;
	agpuSetDepthStencilClearValue_FUN agpuSetDepthStencilClearValue;
	agpuSetColorClearValue_FUN agpuSetColorClearValue;
	agpuSetColorClearValueFrom_FUN agpuSetColorClearValueFrom;
	agpuGetRenderPassColorAttachmentFormats_FUN agpuGetRenderPassColorAttachmentFormats;
	agpuGetRenderPassDepthStencilAttachmentFormat_FUN agpuGetRenderPassDepthStencilAttachmentFormat;
	agpuGetRenderPassSampleCount_FUN agpuGetRenderPassSampleCount;
	agpuGetRenderPassSampleQuality_FUN agpuGetRenderPassSampleQuality;
	agpuAddShaderSignatureBuilderReference_FUN agpuAddShaderSignatureBuilderReference;
	agpuReleaseShaderSignatureBuilder_FUN agpuReleaseShaderSignatureBuilder;
	agpuBuildShaderSignature_FUN agpuBuildShaderSignature;
	agpuAddShaderSignatureBindingConstant_FUN agpuAddShaderSignatureBindingConstant;
	agpuAddShaderSignatureBindingElement_FUN agpuAddShaderSignatureBindingElement;
	agpuBeginShaderSignatureBindingBank_FUN agpuBeginShaderSignatureBindingBank;
	agpuAddShaderSignatureBindingBankElement_FUN agpuAddShaderSignatureBindingBankElement;
	agpuAddShaderSignature_FUN agpuAddShaderSignature;
	agpuReleaseShaderSignature_FUN agpuReleaseShaderSignature;
	agpuCreateShaderResourceBinding_FUN agpuCreateShaderResourceBinding;
	agpuAddShaderResourceBindingReference_FUN agpuAddShaderResourceBindingReference;
	agpuReleaseShaderResourceBinding_FUN agpuReleaseShaderResourceBinding;
	agpuBindUniformBuffer_FUN agpuBindUniformBuffer;
	agpuBindUniformBufferRange_FUN agpuBindUniformBufferRange;
	agpuBindStorageBuffer_FUN agpuBindStorageBuffer;
	agpuBindStorageBufferRange_FUN agpuBindStorageBufferRange;
	agpuBindSampledTextureView_FUN agpuBindSampledTextureView;
	agpuBindStorageImageView_FUN agpuBindStorageImageView;
	agpuBindSampler_FUN agpuBindSampler;
	agpuAddFenceReference_FUN agpuAddFenceReference;
	agpuReleaseFenceReference_FUN agpuReleaseFenceReference;
	agpuWaitOnClient_FUN agpuWaitOnClient;
	agpuAddOfflineShaderCompilerReference_FUN agpuAddOfflineShaderCompilerReference;
	agpuReleaseOfflineShaderCompiler_FUN agpuReleaseOfflineShaderCompiler;
	agpuIsShaderLanguageSupportedByOfflineCompiler_FUN agpuIsShaderLanguageSupportedByOfflineCompiler;
	agpuIsTargetShaderLanguageSupportedByOfflineCompiler_FUN agpuIsTargetShaderLanguageSupportedByOfflineCompiler;
	agpuSetOfflineShaderCompilerSource_FUN agpuSetOfflineShaderCompilerSource;
	agpuCompileOfflineShader_FUN agpuCompileOfflineShader;
	agpuGetOfflineShaderCompilationLogLength_FUN agpuGetOfflineShaderCompilationLogLength;
	agpuGetOfflineShaderCompilationLog_FUN agpuGetOfflineShaderCompilationLog;
	agpuGetOfflineShaderCompilationResultLength_FUN agpuGetOfflineShaderCompilationResultLength;
	agpuGetOfflineShaderCompilationResult_FUN agpuGetOfflineShaderCompilationResult;
	agpuGetOfflineShaderCompilerResultAsShader_FUN agpuGetOfflineShaderCompilerResultAsShader;
	agpuAddStateTrackerCacheReference_FUN agpuAddStateTrackerCacheReference;
	agpuReleaseStateTrackerCacheReference_FUN agpuReleaseStateTrackerCacheReference;
	agpuCreateStateTracker_FUN agpuCreateStateTracker;
	agpuCreateStateTrackerWithCommandAllocator_FUN agpuCreateStateTrackerWithCommandAllocator;
	agpuCreateStateTrackerWithFrameBuffering_FUN agpuCreateStateTrackerWithFrameBuffering;
	agpuCreateImmediateRenderer_FUN agpuCreateImmediateRenderer;
	agpuAddStateTrackerReference_FUN agpuAddStateTrackerReference;
	agpuReleaseStateTrackerReference_FUN agpuReleaseStateTrackerReference;
	agpuStateTrackerBeginRecordingCommands_FUN agpuStateTrackerBeginRecordingCommands;
	agpuStateTrackerEndRecordingCommands_FUN agpuStateTrackerEndRecordingCommands;
	agpuStateTrackerEndRecordingAndFlushCommands_FUN agpuStateTrackerEndRecordingAndFlushCommands;
	agpuStateTrackerReset_FUN agpuStateTrackerReset;
	agpuStateTrackerResetGraphicsPipeline_FUN agpuStateTrackerResetGraphicsPipeline;
	agpuStateTrackerResetComputePipeline_FUN agpuStateTrackerResetComputePipeline;
	agpuStateTrackerSetComputeStage_FUN agpuStateTrackerSetComputeStage;
	agpuStateTrackerSetVertexStage_FUN agpuStateTrackerSetVertexStage;
	agpuStateTrackerSetFragmentStage_FUN agpuStateTrackerSetFragmentStage;
	agpuStateTrackerSetGeometryStage_FUN agpuStateTrackerSetGeometryStage;
	agpuStateTrackerSetTessellationControlStage_FUN agpuStateTrackerSetTessellationControlStage;
	agpuStateTrackerSetTessellationEvaluationStage_FUN agpuStateTrackerSetTessellationEvaluationStage;
	agpuStateTrackerSetBlendState_FUN agpuStateTrackerSetBlendState;
	agpuStateTrackerSetBlendFunction_FUN agpuStateTrackerSetBlendFunction;
	agpuStateTrackerSetColorMask_FUN agpuStateTrackerSetColorMask;
	agpuStateTrackerSetFrontFace_FUN agpuStateTrackerSetFrontFace;
	agpuStateTrackerSetCullMode_FUN agpuStateTrackerSetCullMode;
	agpuStateTrackerSetDepthBias_FUN agpuStateTrackerSetDepthBias;
	agpuStateTrackerSetDepthState_FUN agpuStateTrackerSetDepthState;
	agpuStateTrackerSetPolygonMode_FUN agpuStateTrackerSetPolygonMode;
	agpuStateTrackerSetStencilState_FUN agpuStateTrackerSetStencilState;
	agpuStateTrackerSetStencilFrontFace_FUN agpuStateTrackerSetStencilFrontFace;
	agpuStateTrackerSetStencilBackFace_FUN agpuStateTrackerSetStencilBackFace;
	agpuStateTrackerSetPrimitiveType_FUN agpuStateTrackerSetPrimitiveType;
	agpuStateTrackerSetVertexLayout_FUN agpuStateTrackerSetVertexLayout;
	agpuStateTrackerSetShaderSignature_FUN agpuStateTrackerSetShaderSignature;
	agpuStateTrackerSetSampleDescription_FUN agpuStateTrackerSetSampleDescription;
	agpuStateTrackerSetViewport_FUN agpuStateTrackerSetViewport;
	agpuStateTrackerSetScissor_FUN agpuStateTrackerSetScissor;
	agpuStateTrackerUseVertexBinding_FUN agpuStateTrackerUseVertexBinding;
	agpuStateTrackerUseIndexBuffer_FUN agpuStateTrackerUseIndexBuffer;
	agpuStateTrackerUseIndexBufferAt_FUN agpuStateTrackerUseIndexBufferAt;
	agpuStateTrackerUseDrawIndirectBuffer_FUN agpuStateTrackerUseDrawIndirectBuffer;
	agpuStateTrackerUseComputeDispatchIndirectBuffer_FUN agpuStateTrackerUseComputeDispatchIndirectBuffer;
	agpuStateTrackerUseShaderResources_FUN agpuStateTrackerUseShaderResources;
	agpuStateTrackerUseComputeShaderResources_FUN agpuStateTrackerUseComputeShaderResources;
	agpuStateTrackerDrawArrays_FUN agpuStateTrackerDrawArrays;
	agpuStateTrackerDrawArraysIndirect_FUN agpuStateTrackerDrawArraysIndirect;
	agpuStateTrackerDrawElements_FUN agpuStateTrackerDrawElements;
	agpuStateTrackerDrawElementsIndirect_FUN agpuStateTrackerDrawElementsIndirect;
	agpuStateTrackerDispatchCompute_FUN agpuStateTrackerDispatchCompute;
	agpuStateTrackerDispatchComputeIndirect_FUN agpuStateTrackerDispatchComputeIndirect;
	agpuStateTrackerSetStencilReference_FUN agpuStateTrackerSetStencilReference;
	agpuStateTrackerExecuteBundle_FUN agpuStateTrackerExecuteBundle;
	agpuStateTrackerBeginRenderPass_FUN agpuStateTrackerBeginRenderPass;
	agpuStateTrackerEndRenderPass_FUN agpuStateTrackerEndRenderPass;
	agpuStateTrackerResolveFramebuffer_FUN agpuStateTrackerResolveFramebuffer;
	agpuStateTrackerResolveTexture_FUN agpuStateTrackerResolveTexture;
	agpuStateTrackerPushConstants_FUN agpuStateTrackerPushConstants;
	agpuStateTrackerMemoryBarrier_FUN agpuStateTrackerMemoryBarrier;
	agpuStateTrackerBufferMemoryBarrier_FUN agpuStateTrackerBufferMemoryBarrier;
	agpuStateTrackerTextureMemoryBarrier_FUN agpuStateTrackerTextureMemoryBarrier;
	agpuStateTrackerPushBufferTransitionBarrier_FUN agpuStateTrackerPushBufferTransitionBarrier;
	agpuStateTrackerPushTextureTransitionBarrier_FUN agpuStateTrackerPushTextureTransitionBarrier;
	agpuStateTrackerPopBufferTransitionBarrier_FUN agpuStateTrackerPopBufferTransitionBarrier;
	agpuStateTrackerPopTextureTransitionBarrier_FUN agpuStateTrackerPopTextureTransitionBarrier;
	agpuStateTrackerCopyBuffer_FUN agpuStateTrackerCopyBuffer;
	agpuStateTrackerCopyBufferToTexture_FUN agpuStateTrackerCopyBufferToTexture;
	agpuStateTrackerCopyTextureToBuffer_FUN agpuStateTrackerCopyTextureToBuffer;
	agpuAddImmediateRendererReference_FUN agpuAddImmediateRendererReference;
	agpuReleaseImmediateRendererReference_FUN agpuReleaseImmediateRendererReference;
	agpuBeginImmediateRendering_FUN agpuBeginImmediateRendering;
	agpuEndImmediateRendering_FUN agpuEndImmediateRendering;
	agpuImmediateRendererSetBlendState_FUN agpuImmediateRendererSetBlendState;
	agpuImmediateRendererSetBlendFunction_FUN agpuImmediateRendererSetBlendFunction;
	agpuImmediateRendererSetColorMask_FUN agpuImmediateRendererSetColorMask;
	agpuImmediateRendererSetFrontFace_FUN agpuImmediateRendererSetFrontFace;
	agpuImmediateRendererSetCullMode_FUN agpuImmediateRendererSetCullMode;
	agpuImmediateRendererSetDepthBias_FUN agpuImmediateRendererSetDepthBias;
	agpuImmediateRendererSetDepthState_FUN agpuImmediateRendererSetDepthState;
	agpuImmediateRendererSetPolygonMode_FUN agpuImmediateRendererSetPolygonMode;
	agpuImmediateRendererSetStencilState_FUN agpuImmediateRendererSetStencilState;
	agpuImmediateRendererSetStencilFrontFace_FUN agpuImmediateRendererSetStencilFrontFace;
	agpuImmediateRendererSetStencilBackFace_FUN agpuImmediateRendererSetStencilBackFace;
	agpuImmediateRendererSetViewport_FUN agpuImmediateRendererSetViewport;
	agpuImmediateRendererSetScissor_FUN agpuImmediateRendererSetScissor;
	agpuImmediateRendererSetStencilReference_FUN agpuImmediateRendererSetStencilReference;
	agpuImmediateRendererProjectionMatrixMode_FUN agpuImmediateRendererProjectionMatrixMode;
	agpuImmediateRendererModelViewMatrixMode_FUN agpuImmediateRendererModelViewMatrixMode;
	agpuImmediateRendererTextureMatrixMode_FUN agpuImmediateRendererTextureMatrixMode;
	agpuImmediateRendererIdentity_FUN agpuImmediateRendererIdentity;
	agpuImmediateRendererPushMatrix_FUN agpuImmediateRendererPushMatrix;
	agpuImmediateRendererPopMatrix_FUN agpuImmediateRendererPopMatrix;
	agpuImmediateRendererLoadMatrix_FUN agpuImmediateRendererLoadMatrix;
	agpuImmediateRendererLoadTransposeMatrix_FUN agpuImmediateRendererLoadTransposeMatrix;
	agpuImmediateRendererMultiplyMatrix_FUN agpuImmediateRendererMultiplyMatrix;
	agpuImmediateRendererMultiplyTransposeMatrix_FUN agpuImmediateRendererMultiplyTransposeMatrix;
	agpuImmediateRendererOrtho_FUN agpuImmediateRendererOrtho;
	agpuImmediateRendererFrustum_FUN agpuImmediateRendererFrustum;
	agpuImmediateRendererPerspective_FUN agpuImmediateRendererPerspective;
	agpuImmediateRendererRotate_FUN agpuImmediateRendererRotate;
	agpuImmediateRendererTranslate_FUN agpuImmediateRendererTranslate;
	agpuImmediateRendererScale_FUN agpuImmediateRendererScale;
	agpuImmediateRendererSetFlatShading_FUN agpuImmediateRendererSetFlatShading;
	agpuImmediateRendererSetLightingEnabled_FUN agpuImmediateRendererSetLightingEnabled;
	agpuImmediateRendererClearLights_FUN agpuImmediateRendererClearLights;
	agpuImmediateRendererSetAmbientLighting_FUN agpuImmediateRendererSetAmbientLighting;
	agpuImmediateRendererSetLight_FUN agpuImmediateRendererSetLight;
	agpuImmediateRendererSetMaterial_FUN agpuImmediateRendererSetMaterial;
	agpuImmediateRendererSetTextureEnabled_FUN agpuImmediateRendererSetTextureEnabled;
	agpuImmediateRendererBindTexture_FUN agpuImmediateRendererBindTexture;
	agpuImmediateRendererSetClipPlane_FUN agpuImmediateRendererSetClipPlane;
	agpuImmediateRendererSetFogMode_FUN agpuImmediateRendererSetFogMode;
	agpuImmediateRendererSetFogColor_FUN agpuImmediateRendererSetFogColor;
	agpuImmediateRendererSetFogDistances_FUN agpuImmediateRendererSetFogDistances;
	agpuImmediateRendererSetFogDensity_FUN agpuImmediateRendererSetFogDensity;
	agpuBeginImmediateRendererPrimitives_FUN agpuBeginImmediateRendererPrimitives;
	agpuEndImmediateRendererPrimitives_FUN agpuEndImmediateRendererPrimitives;
	agpuSetImmediateRendererColor_FUN agpuSetImmediateRendererColor;
	agpuSetImmediateRendererTexcoord_FUN agpuSetImmediateRendererTexcoord;
	agpuSetImmediateRendererNormal_FUN agpuSetImmediateRendererNormal;
	agpuAddImmediateRendererVertex_FUN agpuAddImmediateRendererVertex;
	agpuBeginImmediateRendererMeshWithVertices_FUN agpuBeginImmediateRendererMeshWithVertices;
	agpuBeginImmediateRendererMeshWithVertexBinding_FUN agpuBeginImmediateRendererMeshWithVertexBinding;
	agpuImmediateRendererUseIndexBuffer_FUN agpuImmediateRendererUseIndexBuffer;
	agpuImmediateRendererUseIndexBufferAt_FUN agpuImmediateRendererUseIndexBufferAt;
	agpuSetImmediateRendererCurrentMeshColors_FUN agpuSetImmediateRendererCurrentMeshColors;
	agpuSetImmediateRendererCurrentMeshNormals_FUN agpuSetImmediateRendererCurrentMeshNormals;
	agpuSetImmediateRendererCurrentMeshTexCoords_FUN agpuSetImmediateRendererCurrentMeshTexCoords;
	agpuImmediateRendererSetPrimitiveType_FUN agpuImmediateRendererSetPrimitiveType;
	agpuImmediateRendererDrawArrays_FUN agpuImmediateRendererDrawArrays;
	agpuImmediateRendererDrawElements_FUN agpuImmediateRendererDrawElements;
	agpuImmediateRendererDrawElementsWithIndices_FUN agpuImmediateRendererDrawElementsWithIndices;
	agpuEndImmediateRendererMesh_FUN agpuEndImmediateRendererMesh;
} agpu_icd_dispatch;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _AGPU_H_ */
