#include "PositionColorBufferStructs.hlsli"

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// ������ ��ġ�� �������ǵ� �������� ��ȯ�մϴ�.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// ���� ���� ���� ����մϴ�.
	output.color = input.color;

	return output;
}