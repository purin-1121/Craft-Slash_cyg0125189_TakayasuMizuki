#include "Shadow.hlsli"

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output;
    
    float4 position = mul(float4(input.position, 1.0f), worldViewProj);
    
    output.uv = input.uv;
    output.pos = position;
    
	return output;
}