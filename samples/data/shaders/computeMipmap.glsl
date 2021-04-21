#version 450

layout(local_size_x = 16, local_size_y = 16) in;

layout (set=0, binding = 0, rgba8) uniform readonly image2D inputImage;
layout (set=0, binding = 1, rgba8) uniform writeonly image2D outputImage;

shared vec4 fetchedTexels[16][16];

void main()
{
    uvec2 sourceExtent = imageSize(inputImage);
    uvec2 destExtent = imageSize(outputImage);

    ivec2 sourceCoord = ivec2(min(gl_GlobalInvocationID.xy, sourceExtent - 1));
    fetchedTexels[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = imageLoad(inputImage, sourceCoord);
    barrier();

    uvec2 halfWorkGroupSize = gl_WorkGroupSize.xy/2;
    if(gl_LocalInvocationID.x >= halfWorkGroupSize.x || gl_LocalInvocationID.y >= halfWorkGroupSize.y)
        return;

    ivec2 destCoord = ivec2(gl_WorkGroupID.xy * halfWorkGroupSize + gl_LocalInvocationID.xy);
    uvec2 sourceTexel = gl_LocalInvocationID.xy*2;
    vec4 reducedTexel = (fetchedTexels[sourceTexel.x][sourceTexel.y] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y] +
        fetchedTexels[sourceTexel.x][sourceTexel.y + 1] + fetchedTexels[sourceTexel.x + 1][sourceTexel.y + 1]) *0.25;

    if(destCoord.x < destExtent.x && destCoord.y < destExtent.y)
        imageStore(outputImage, destCoord, reducedTexel);
}
