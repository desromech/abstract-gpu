#ifndef AGPU_IMMEDIATE_RENDERER_HPP
#define AGPU_IMMEDIATE_RENDERER_HPP

#include "state_tracker_cache.hpp"
#include "vector_math.hpp"
#include <vector>

namespace AgpuCommon
{

/**
 * Immediate renderer vertex.
 */
struct ImmediateRendererVertex
{
    Vector2F texcoord;
    Vector3F normal;
    Vector3F position;
    Vector4F color;
};

/**
 * I am an immediate renderer that emulates a classic OpenGL style
 * glBegin()/glEnd() rendering interface.
 */
class ImmediateRenderer : public agpu::immediate_renderer
{
public:
    ImmediateRenderer(const agpu::state_tracker_cache_ref &stateTrackerCache);
    ~ImmediateRenderer();

    static agpu::immediate_renderer_ref create(const agpu::state_tracker_cache_ref &cache);

    virtual agpu_error beginRendering(const agpu::state_tracker_ref & state_tracker) override;
	virtual agpu_error endRendering() override;
	virtual agpu_error beginPrimitives(agpu_primitive_topology type) override;
	virtual agpu_error endPrimitives() override;
	virtual agpu_error color(agpu_float r, agpu_float g, agpu_float b, agpu_float a) override;
	virtual agpu_error texcoord(agpu_float x, agpu_float y) override;
	virtual agpu_error normal(agpu_float x, agpu_float y, agpu_float z) override;
	virtual agpu_error vertex(agpu_float x, agpu_float y, agpu_float z) override;

    agpu::state_tracker_cache_ref stateTrackerCache;
    agpu::state_tracker_ref currentStateTracker;

    agpu_primitive_topology activePrimitiveTopology;
    Vector2F currentTexcoord;
    Vector3F currentNormal;
    Vector4F currentColor;
};

} // End of namespace AgpuCommon

#endif //AGPU_IMMEDIATE_RENDERER_HPP
