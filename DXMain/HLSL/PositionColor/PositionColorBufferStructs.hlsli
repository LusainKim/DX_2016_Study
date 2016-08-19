// ���� ���� �ۼ��� ���� �� ���� �⺻ �� �߽� ����� �����ϴ� ��� �����Դϴ�.
// �� �� ī�޶� �κ��Դϴ�.
cbuffer CameraConstantBuffer : register(b0)
{
	matrix projection;
	matrix view;
}

// ���� ���� �ۼ��� ���� �� ���� �⺻ �� �߽� ����� �����ϴ� ��� �����Դϴ�.
// �� �� �� �κ��Դϴ�.
cbuffer ModelConstantBuffer : register(b1)
{
	matrix model;
};

// ������ ���̴��� ���� �Է����� ���Ǵ� �������� �������Դϴ�.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// �ȼ� ���̴��� ����� �ȼ��� �� �������Դϴ�.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};