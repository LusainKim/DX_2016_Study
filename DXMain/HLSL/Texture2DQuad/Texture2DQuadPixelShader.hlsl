#include "Texture2DQuadBufferStruct.hlsli"

// (보간된) 색 데이터에 대한 통과 함수입니다.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 cColor = g_tex2D_Quad.Sample(g_ss_Quad, input.texcoord);
	return cColor;
}
