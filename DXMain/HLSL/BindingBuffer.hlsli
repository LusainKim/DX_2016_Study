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