#ifndef SAMPLE_VERTEX_HPP
#define SAMPLE_VERTEX_HPP

/**
 * A simple vertex that is used by some samples.
 */
struct SampleVertex
{
    SampleVertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec4 &color, const glm::vec2 &texcoord)
        : position(position), normal(normal), color(color), texcoord(texcoord), boneWeights(1, 0, 0, 0) {}

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texcoord;
    glm::ivec4 boneIndices;
    glm::vec4 boneWeights;

    static SampleVertex onlyColor(float x, float y, float z, float r, float g, float b, float a)
    {
        return SampleVertex(glm::vec3(x, y, z), glm::vec3(0,0,0), glm::vec4(r, g, b, a), glm::vec2(0, 1));
    }

    static SampleVertex onlyColorTc(float x, float y, float z, float r, float g, float b, float a, float u, float v)
    {
        return SampleVertex(glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec4(r, g, b, a), glm::vec2(u, v));
    }

    static agpu_vertex_attrib_description Description[];
    static const int DescriptionSize;
};

#endif //SAMPLE_VERTEX_HPP
