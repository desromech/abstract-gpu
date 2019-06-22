#include "immediate_renderer.hpp"

namespace AgpuCommon
{

ImmediateRenderer::ImmediateRenderer(const agpu::state_tracker_cache_ref &stateTrackerCache)
    : stateTrackerCache(stateTrackerCache)
{
}

ImmediateRenderer::~ImmediateRenderer()
{
}

agpu::immediate_renderer_ref ImmediateRenderer::create(const agpu::state_tracker_cache_ref &cache)
{
    return agpu::makeObject<ImmediateRenderer> (cache);
}

agpu_error ImmediateRenderer::beginRendering(const agpu::state_tracker_ref &state_tracker)
{
    if(!state_tracker)
        return AGPU_NULL_POINTER;
    if(currentStateTracker)
        return AGPU_INVALID_OPERATION;
    currentStateTracker = state_tracker;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::endRendering()
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;
    currentStateTracker.reset();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::beginPrimitives(agpu_primitive_topology type)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;
    activePrimitiveTopology = type;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::endPrimitives()
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::color(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentColor = Vector4F(r, g, b, a);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::texcoord(agpu_float x, agpu_float y)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentTexcoord = Vector2F(x, y);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::normal(agpu_float x, agpu_float y, agpu_float z)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentNormal = Vector3F(x, y, z);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::vertex(agpu_float x, agpu_float y, agpu_float z)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    return AGPU_OK;
}

} // End of namespace AgpuCommon
