#include "PositionColorBufferStructs.hlsli"

// (보간된) 색 데이터에 대한 통과 함수입니다.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.color, 0.7f);
}
