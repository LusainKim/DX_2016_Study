#include "PositionColorBufferStructs.hlsli"

// (������) �� �����Ϳ� ���� ��� �Լ��Դϴ�.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.color, 0.7f);
}
