#pragma once

// 카메라 변환에 필요한 인자입니다.
struct CameraConstantBuffer
{
	DirectX::XMFLOAT4X4A projection;
	DirectX::XMFLOAT4X4A view;
};

// MVP 매트릭스를 꼭짓점 셰이더로 보내는 데 사용되는 상수 버퍼입니다.
struct ModelConstantBuffer
{
	DirectX::XMFLOAT4X4A model;
};

// 꼭짓점별 데이터를 꼭짓점 셰이더로 보내는 데 사용됩니다.
struct VertexPositionColor
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};
