#ifndef AGPU_METAL_PIPELINE_COMMAND_STATE_HPP
#define AGPU_METAL_PIPELINE_COMMAND_STATE_HPP

#include "device.hpp"

/**
 * I am used to store pipeline states that in metal are set when encoding a command
 * buffer.
 */
struct agpu_pipeline_command_state
{
    agpu_pipeline_command_state()
    {
        primitiveType = MTLPrimitiveTypePoint;
        cullMode = MTLCullModeNone;
        frontFace = MTLWindingClockwise;
    }

    MTLPrimitiveType primitiveType;
    MTLCullMode cullMode;
    MTLWinding frontFace;
};

#endif //AGPU_METAL_PIPELINE_COMMAND_STATE_HPP
