#pragma once

// ī�޶� ��ȯ�� �ʿ��� �����Դϴ�.
struct CameraConstantBuffer
{
	DirectX::XMFLOAT4X4A projection;
	DirectX::XMFLOAT4X4A view;
};

// MVP ��Ʈ������ ������ ���̴��� ������ �� ���Ǵ� ��� �����Դϴ�.
struct ModelConstantBuffer
{
	DirectX::XMFLOAT4X4A model;
};

// �������� �����͸� ������ ���̴��� ������ �� ���˴ϴ�.
struct VertexPositionColor
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};
