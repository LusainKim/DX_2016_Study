// ������ ���̴��� ���� �Է����� ���Ǵ� �������� �������Դϴ�.

Texture2D g_tex2D_Quad : register(t10);
SamplerState g_ss_Quad : register(s10);

struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 texcoord : TEXCOORD0;
};

// �ȼ� ���̴��� ����� �ȼ��� �� �������Դϴ�.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};