// Sample shader
int max(int a, int b)
{
    if(a > b)
        return a;
    return b;
}

void simpleWhile()
{
    while(true)
    {
        if(true)
            break;

        if(false)
            continue;
    }
}

struct VertexInput
{
    [location(0)] float3 position;
    [location(1)] float4 color;
};

struct VertexOutput
{
    float4 color;
};

kernel(vertex) void vertexShader(in VertexInput input, out VertexOutput output)
{
    float a;
    a = 1.0;

    output.color = input.color;
}

kernel(fragment) void fragmentShader(in VertexOutput input, out VertexOutput output)
{
}

