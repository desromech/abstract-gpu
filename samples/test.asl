// Sample shader
int max(int a, int b)
{
    if(a > b)
        return a;
    return b;
}

void simpleWhile()
{
    if(true)
        return;

    while(1 > 0)
    {
        if(true)
            break;

        while(true)
        {
            if(false)
                break;
            if(true)
                continue;
        }

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

struct FragmentOutput
{
    float4 color;
};

kernel(vertex) void vertexShader(in VertexInput input, out VertexOutput output)
{
    float a;
    a = 1.0;

    if(a > 0.0)
        output.color = input.color;

    simpleWhile();
}

kernel(fragment) void fragmentShader(in VertexOutput input, out FragmentOutput output)
{
    output.color = input.color; 
}

