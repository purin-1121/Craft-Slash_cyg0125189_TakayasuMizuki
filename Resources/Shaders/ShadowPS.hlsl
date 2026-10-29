#include "Shadow.hlsli"

float4 main(PS_INPUT input) : SV_TARGET0
{
    float4 output = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	// 原点を左上から中心に移行
    float2 position = input.uv * 2.0f - 1.0f;
	// 距離を計算
    float dis = length(position);
	// 中心ほど濃くなるように
    float fade = 1.0f - smoothstep(0.2f, 1.0f, dis);
	
    output.a = alpha * fade;
	
	return output;
}