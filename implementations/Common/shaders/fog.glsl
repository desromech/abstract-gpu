#include "common_state.glsl"

vec4 applyFogWithState(in FogState fogState, vec4 cleanColor, vec3 inPosition)
{
    uint mode = fogState.mode;
    if(mode == FogMode_None)
        return cleanColor;

    float d = length(inPosition);
    float f = 0.0;
    if(mode == FogMode_Linear)
    {
        f = max(0.0, (fogState.endDistance - d) / (fogState.endDistance - fogState.startDistance));
    }
    else if(mode == FogMode_Exponential)
    {
        f = exp(-d*fogState.density);
    }
    else if(mode == FogMode_ExponentialSquared)
    {
        float dc = d*fogState.density;
        f = exp(-dc*dc);
    }

    return mix(fogState.color, cleanColor, f);
}

vec4 applyFog(vec4 cleanColor, vec3 inPosition)
{
    return applyFogWithState(ExtraRenderingState.fogState, cleanColor, inPosition);
}
