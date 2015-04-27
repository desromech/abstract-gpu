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

kernel(vertex) void vertexShader()
{
}

kernel(fragment) void fragmentShader()
{
}

