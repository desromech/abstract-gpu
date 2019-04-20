#ifndef AGPU_CONSTANTS_HPP_
#define AGPU_CONSTANTS_HPP_

#include "device.hpp"

namespace AgpuGL
{

inline GLenum mapCompareFunction(agpu_compare_function function)
{
    switch (function)
    {
    case AGPU_ALWAYS: return GL_ALWAYS;
    case AGPU_NEVER: return GL_NEVER;
    case AGPU_LESS: return GL_LESS;
    case AGPU_LESS_EQUAL: return GL_LEQUAL;
    case AGPU_EQUAL: return GL_EQUAL;
    case AGPU_NOT_EQUAL: return GL_NOTEQUAL;
    case AGPU_GREATER: return GL_GREATER;
    case AGPU_GREATER_EQUAL: return GL_GEQUAL;
    default:
        abort();
    }
}

inline GLenum mapStencilOperation(agpu_stencil_operation operation)
{
    switch (operation)
    {
    case AGPU_KEEP: return GL_KEEP;
    case AGPU_ZERO: return GL_ZERO;
    case AGPU_REPLACE: return GL_REPLACE;
    case AGPU_INVERT: return GL_INVERT;
    case AGPU_INCREASE: return GL_INCR;
    case AGPU_INCREASE_WRAP: return GL_INCR_WRAP;
    case AGPU_DECREASE: return GL_DECR;
    case AGPU_DECREASE_WRAP: return GL_DECR_WRAP;
    default:
        abort();
    }
}

inline GLenum mapBlendFactor(agpu_blending_factor factor, bool alpha)
{
    switch (factor)
    {
    case AGPU_BLENDING_ZERO: return GL_ZERO;
    case AGPU_BLENDING_ONE: return GL_ONE;
    case AGPU_BLENDING_SRC_COLOR: return GL_SRC_COLOR;
    case AGPU_BLENDING_INVERTED_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
    case AGPU_BLENDING_SRC_ALPHA: return GL_SRC_ALPHA;
    case AGPU_BLENDING_INVERTED_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
    case AGPU_BLENDING_DEST_ALPHA: return GL_DST_ALPHA;
    case AGPU_BLENDING_INVERTED_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
    case AGPU_BLENDING_DEST_COLOR: return GL_DST_COLOR;
    case AGPU_BLENDING_INVERTED_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
    case AGPU_BLENDING_SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
    case AGPU_BLENDING_CONSTANT_FACTOR: return alpha ? GL_CONSTANT_ALPHA : GL_CONSTANT_COLOR;
    case AGPU_BLENDING_INVERTED_CONSTANT_FACTOR: return alpha ? GL_ONE_MINUS_CONSTANT_ALPHA : GL_ONE_MINUS_CONSTANT_COLOR;
    case AGPU_BLENDING_SRC_1COLOR: return GL_SRC1_COLOR;
    case AGPU_BLENDING_INVERTED_SRC_1COLOR: return GL_ONE_MINUS_SRC1_COLOR;
    case AGPU_BLENDING_SRC_1ALPHA: return GL_SRC1_ALPHA;
    case AGPU_BLENDING_INVERTED_SRC_1ALPHA: return GL_ONE_MINUS_SRC1_ALPHA;
    default:
        abort();
    }
}

inline GLenum mapBlendOperation(agpu_blending_operation operation)
{
    switch (operation)
    {
    case AGPU_BLENDING_OPERATION_ADD: return GL_FUNC_ADD;
    case AGPU_BLENDING_OPERATION_SUBTRACT: return GL_FUNC_SUBTRACT;
    case AGPU_BLENDING_OPERATION_REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
    case AGPU_BLENDING_OPERATION_MIN: // Unsupported
    case AGPU_BLENDING_OPERATION_MAX: // Unsupported
    default:
        abort();
    }
}

inline GLenum mapFaceWinding(agpu_face_winding winding)
{
    switch(winding)
    {
    case AGPU_CLOCKWISE: return GL_CW;
    case AGPU_COUNTER_CLOCKWISE: return GL_CCW;
    default:
        abort();
    }
}

inline GLenum mapCullingMode(agpu_cull_mode cullingMode)
{
    switch(cullingMode)
    {
    case AGPU_CULL_MODE_NONE: return GL_NONE;
    case AGPU_CULL_MODE_FRONT: return GL_FRONT;
    case AGPU_CULL_MODE_BACK: return GL_BACK;
    case AGPU_CULL_MODE_FRONT_AND_BACK: return GL_FRONT_AND_BACK;
    default:
        abort();
    }
}

} // End of namespace AgpuGL

#endif //AGPU_CONSTANTS_HPP_
