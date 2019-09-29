#ifndef SAMPLE_MESH_HPP
#define SAMPLE_MESH_HPP

#include "SampleBase.hpp"
#include "SampleVertex.hpp"
#include <vector>
#include <memory>

/**
 * A sample submesh.
 */
class SampleSubmesh
{
public:
    SampleSubmesh(uint32_t cstartIndex=0, uint32_t cindexCount = 0)
        : startIndex(cstartIndex), indexCount(cindexCount) {}
    ~SampleSubmesh() {}

    uint32_t startIndex;
    uint32_t indexCount;
};

/**
 * A sample mesh
 */
class SampleMesh
{
public:
    SampleMesh()
    {
    }

    ~SampleMesh()
    {
    }

    void beginDrawingWithImmediateRenderer(const agpu_immediate_renderer_ref &immediateRenderer, bool explicitBuffers)
    {
        if(vertices.empty())
            return;

        if(explicitBuffers)
        {
            immediateRenderer->beginMeshWithVertexBinding(vertexLayout, vertexBinding);
            immediateRenderer->useIndexBuffer(indexBuffer);
        }
        else
        {
            immediateRenderer->beginMeshWithVertices(vertices.size(), sizeof(SampleVertex), 3, &vertices[0].position);
            immediateRenderer->setCurrentMeshColors(sizeof(SampleVertex), 4, &vertices[0].color);
            immediateRenderer->setCurrentMeshNormals(sizeof(SampleVertex), 3, &vertices[0].normal);
            immediateRenderer->setCurrentMeshTexCoords(sizeof(SampleVertex), 2, &vertices[0].texcoord);
        }
    }

    void drawWithImmediateRenderer(const agpu_immediate_renderer_ref &immediateRenderer, bool explicitBuffers, size_t instanceCount = 1)
    {
        if(vertices.empty())
            return;

        if(explicitBuffers)
        {
            immediateRenderer->setPrimitiveType(AGPU_TRIANGLES);
            for(auto &submesh : submeshes)
                immediateRenderer->drawElements(submesh.indexCount, instanceCount, submesh.startIndex, 0, 0);
        }
        else
        {
            for(auto &submesh : submeshes)
                immediateRenderer->drawElementsWithIndices(AGPU_TRIANGLES, &indices[0], submesh.indexCount, instanceCount, submesh.startIndex, 0, 0);
        }
    }

    void endDrawingWithImmediateRenderer(const agpu_immediate_renderer_ref &immediateRenderer)
    {
        if(vertices.empty())
            return;
        immediateRenderer->endMesh();
    }

    void drawWithStateTracker(const agpu_state_tracker_ref &stateTracker, size_t instanceCount = 1)
    {
        stateTracker->useVertexBinding(vertexBinding);
        stateTracker->useIndexBuffer(indexBuffer);
        for(auto &submesh : submeshes)
            stateTracker->drawElements(submesh.indexCount, instanceCount, submesh.startIndex, 0, 0);
    }

    void drawWithCommandList(const agpu_command_list_ref &commandList, size_t instanceCount = 1)
    {
        commandList->useVertexBinding(vertexBinding);
        commandList->useIndexBuffer(indexBuffer);
        for(auto &submesh : submeshes)
            commandList->drawElements(submesh.indexCount, instanceCount, submesh.startIndex, 0, 0);
    }

    agpu_buffer_ref vertexBuffer;
    agpu_buffer_ref indexBuffer;
    agpu_vertex_layout_ref vertexLayout;
    agpu_vertex_binding_ref vertexBinding;
    std::vector<SampleVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SampleSubmesh> submeshes;
};
typedef std::shared_ptr<SampleMesh> SampleMeshPtr;

/**
 * A sample mesh builder.
 */
class SampleMeshBuilder
{
public:
    SampleMeshBuilder(AbstractSampleBase *csampleBase)
        : sampleBase(csampleBase)
    {
        baseVertex = 0;
        currentColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    ~SampleMeshBuilder()
    {
    }

    SampleMeshBuilder &beginTriangles()
    {
        return beginSubmesh();
    }

    SampleMeshBuilder &beginSubmesh()
    {
        if(submeshes.empty())
        {
            SampleSubmesh submesh;
            submesh.startIndex = indices.size();
            submeshes.push_back(submesh);
        }

        baseVertex = vertices.size();
        return *this;
    }

    SampleMeshBuilder &addVertexPNT(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texcoord)
    {
        vertices.push_back(SampleVertex(position, normal, currentColor, texcoord));
        return *this;
    }

    SampleMeshBuilder &addIndex(uint32_t i1)
    {
        indices.push_back(baseVertex + i1);
        return *this;
    }

    SampleMeshBuilder &addTriangle(uint32_t i1, uint32_t i2, uint32_t i3)
    {
        return addIndex(i1)
            .addIndex(i2)
            .addIndex(i3);
    }

    SampleMeshBuilder &addCubeWithExtent(const glm::vec3 &extent)
    {
        return addCube(extent*-0.5f, extent*0.5f);
    }

    SampleMeshBuilder &addCube(const glm::vec3 &min, const glm::vec3 &max)
    {
        // Left
        beginTriangles();
        addVertexPNT(glm::vec3(min.x, min.y, min.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
        addVertexPNT(glm::vec3(min.x, max.y, min.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        addVertexPNT(glm::vec3(min.x, max.y, max.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        addVertexPNT(glm::vec3(min.x, min.y, max.z), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        addTriangle(1, 0, 2);
        addTriangle(3, 2, 0);

        // Right
        beginTriangles();
        addVertexPNT(glm::vec3(max.x, min.y, min.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, max.y, min.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, max.y, max.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        addVertexPNT(glm::vec3(max.x, min.y, max.z), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        addTriangle(0, 1, 2);
        addTriangle(2, 3, 0);

        // Top
        beginTriangles();
        addVertexPNT(glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        addVertexPNT(glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        addTriangle(1, 0, 2);
        addTriangle(3, 2, 0);

        // Bottom
        beginTriangles();
        addVertexPNT(glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        addVertexPNT(glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        addTriangle(0, 1, 2);
        addTriangle(2, 3, 0);

        // Back
        beginTriangles();
        addVertexPNT(glm::vec3(min.x, min.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, min.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, max.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
        addVertexPNT(glm::vec3(min.x, max.y, min.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
        addTriangle(1, 0, 2);
        addTriangle(3, 2, 0);

        // Front
        beginTriangles();
        addVertexPNT(glm::vec3(min.x, min.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, min.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
        addVertexPNT(glm::vec3(max.x, max.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
        addVertexPNT(glm::vec3(min.x, max.y, max.z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
        addTriangle(0, 1, 2);
        addTriangle(2, 3, 0);

        return *this;
    }

    SampleMeshBuilder &finishCurrentSubmesh()
    {
        if(!submeshes.empty())
        {
            auto &submesh = submeshes.back();
            submesh.indexCount = indices.size() - submesh.startIndex;
        }

        return *this;
    }

    SampleMeshPtr mesh()
    {
        finishCurrentSubmesh();

        // Create the mesh object.
        auto result = std::make_shared<SampleMesh> ();
        result->vertexBuffer = sampleBase->createImmutableVertexBuffer(vertices.size(), sizeof(SampleVertex), &vertices[0]);
        result->indexBuffer = sampleBase->createImmutableIndexBuffer(indices.size(), sizeof(uint32_t), &indices[0]);
        result->submeshes = submeshes;
        result->vertices = vertices;
        result->indices = indices;
        {
            result->vertexLayout = sampleBase->getSampleVertexLayout();
            result->vertexBinding = sampleBase->device->createVertexBinding(result->vertexLayout);
            result->vertexBinding->bindVertexBuffers(1, &result->vertexBuffer);
        }

        return result;
    }

    AbstractSampleBase *sampleBase;

    glm::vec4 currentColor;
    uint32_t baseVertex;

    std::vector<SampleVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SampleSubmesh> submeshes;
};

#endif //SAMPLE_MESH_HPP
