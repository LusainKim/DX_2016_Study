// 기하 도형 작성을 위해 세 개의 기본 열 중심 행렬을 저장하는 상수 버퍼입니다.
// 그 중 카메라 부분입니다.
cbuffer CameraConstantBuffer : register(b0)
{
	matrix projection;
	matrix view;
}

// 기하 도형 작성을 위해 세 개의 기본 열 중심 행렬을 저장하는 상수 버퍼입니다.
// 그 중 모델 부분입니다.
cbuffer ModelConstantBuffer : register(b1)
{
	matrix model;
};

// 꼭짓점 셰이더에 대한 입력으로 사용되는 꼭짓점별 데이터입니다.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// 픽셀 셰이더를 통과한 픽셀당 색 데이터입니다.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};