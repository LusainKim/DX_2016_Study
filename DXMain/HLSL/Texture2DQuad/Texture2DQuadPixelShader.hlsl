#include "Texture2DQuadBufferStruct.hlsli"

// (������) �� �����Ϳ� ���� ��� �Լ��Դϴ�.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 cColor = g_tex2D_Quad.Sample(g_ss_Quad, input.texcoord);
	return cColor;
}
