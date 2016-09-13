// 꼭짓점 셰이더에 대한 입력으로 사용되는 꼭짓점별 데이터입니다.

Texture2D g_tex2D_Quad : register(t10);
SamplerState g_ss_Quad : register(s10);

struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD0;
};

// 픽셀 셰이더를 통과한 픽셀당 색 데이터입니다.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};