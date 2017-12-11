#ifndef _AGPU_METAL_CONSTANTS_HPP
#define _AGPU_METAL_CONSTANTS_HPP

inline MTLBlendFactor mapBlendFactor(agpu_blending_factor factor, bool alpha)
{
    switch(factor)
    {
    case AGPU_BLENDING_ZERO: return MTLBlendFactorZero;
    case AGPU_BLENDING_ONE: return MTLBlendFactorOne;
    case AGPU_BLENDING_SRC_COLOR: return MTLBlendFactorSourceColor;
    case AGPU_BLENDING_INVERTED_SRC_COLOR: return MTLBlendFactorOneMinusSourceColor;
    case AGPU_BLENDING_SRC_ALPHA: return MTLBlendFactorSourceAlpha;
    case AGPU_BLENDING_INVERTED_SRC_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
    case AGPU_BLENDING_DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
    case AGPU_BLENDING_INVERTED_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
    case AGPU_BLENDING_DEST_COLOR: return MTLBlendFactorDestinationColor;
    case AGPU_BLENDING_INVERTED_DEST_COLOR: return MTLBlendFactorOneMinusDestinationColor;
    case AGPU_BLENDING_SRC_ALPHA_SAT: return MTLBlendFactorSourceAlphaSaturated;
    case AGPU_BLENDING_CONSTANT_FACTOR: return alpha ? MTLBlendFactorBlendColor : MTLBlendFactorBlendAlpha;
    case AGPU_BLENDING_INVERTED_CONSTANT_FACTOR: return alpha ? MTLBlendFactorOneMinusBlendColor : MTLBlendFactorOneMinusBlendAlpha;
/*    case AGPU_BLENDING_SRC_1COLOR: return MTLBlendFactorSource1Color;
    case AGPU_BLENDING_INVERTED_SRC_1COLOR: return MTLBlendFactorOneMinusSource1Color;
    case AGPU_BLENDING_SRC_1ALPHA: return MTLBlendFactorSource1Alpha;
    case AGPU_BLENDING_INVERTED_SRC_1ALPHA: return MTLBlendFactorOneMinusSource1Alpha;
    */
    default: abort();
    }
}

inline MTLBlendOperation mapBlendOperation(agpu_blending_operation operation)
{
    switch(operation)
    {
    case AGPU_BLENDING_OPERATION_ADD: return MTLBlendOperationAdd;
    case AGPU_BLENDING_OPERATION_SUBTRACT: return MTLBlendOperationSubtract;
    case AGPU_BLENDING_OPERATION_REVERSE_SUBTRACT: return MTLBlendOperationReverseSubtract;
    case AGPU_BLENDING_OPERATION_MIN: return MTLBlendOperationMin;
    case AGPU_BLENDING_OPERATION_MAX: return MTLBlendOperationMax;
    default: abort();
    }
}

inline MTLCompareFunction mapCompareFunction(agpu_compare_function function)
{
    switch(function)
    {
    case AGPU_ALWAYS: return MTLCompareFunctionAlways;
    case AGPU_NEVER: return MTLCompareFunctionNever;
    case AGPU_LESS: return MTLCompareFunctionLess;
    case AGPU_LESS_EQUAL: return MTLCompareFunctionLessEqual;
    case AGPU_EQUAL: return MTLCompareFunctionEqual;
    case AGPU_NOT_EQUAL: return MTLCompareFunctionNotEqual;
    case AGPU_GREATER: return MTLCompareFunctionGreater;
    case AGPU_GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
    default: abort();
    }
}

inline MTLStencilOperation mapStencilOperation(agpu_stencil_operation operation)
{
    switch(operation)
    {
    case AGPU_KEEP: return MTLStencilOperationKeep;
	case AGPU_ZERO: return MTLStencilOperationZero;
	case AGPU_REPLACE: return MTLStencilOperationReplace;
	case AGPU_INVERT: return MTLStencilOperationInvert;
	case AGPU_INCREASE: return MTLStencilOperationIncrementClamp;
	case AGPU_INCREASE_WRAP: return MTLStencilOperationIncrementWrap;
	case AGPU_DECREASE: return MTLStencilOperationDecrementClamp;
	case AGPU_DECREASE_WRAP: return MTLStencilOperationDecrementWrap;
    default: abort();
    }
}

#endif //_AGPU_METAL_CONSTANTS_HPP
