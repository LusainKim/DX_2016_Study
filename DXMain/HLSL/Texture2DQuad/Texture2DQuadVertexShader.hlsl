#include "..\BindingBuffer.hlsli"
#include "Texture2DQuadBufferStruct.hlsli"

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// 꼭짓점 위치를 프로젝션된 공간으로 변환합니다.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// 수정 없이 색을 통과합니다.
	output.texcoord = input.texcoord;

	return output;
}
