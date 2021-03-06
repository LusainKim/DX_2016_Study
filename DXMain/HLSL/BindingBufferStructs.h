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

// 상수 버퍼라는 것을 알리는 추상 클래스입니다.
struct ConstantBuffer {};

struct CSOInfo {
	
	const int vs_ResID;
	const int ds_ResID;
	const int hs_ResID;
	const int gs_ResID;
	const int ps_ResID;

	const LPCTSTR vs_Path;
	const LPCTSTR ds_Path;
	const LPCTSTR hs_Path;
	const LPCTSTR gs_Path;
	const LPCTSTR ps_Path;

	CSOInfo(
		  int vs_id			, int hs_id			, int ds_id			, int gs_id			, int ps_id
		, LPCTSTR vs_path	, LPCTSTR hs_path	, LPCTSTR ds_path	, LPCTSTR gs_path	, LPCTSTR ps_path
	)
		: vs_ResID(vs_id)
		, ds_ResID(ds_id)
		, hs_ResID(hs_id)
		, gs_ResID(gs_id)
		, ps_ResID(ps_id)
		, vs_Path(vs_path)
		, ds_Path(ds_path)
		, hs_Path(hs_path)
		, gs_Path(gs_path)
		, ps_Path(ps_path)
	{ }
};

// 꼭짓점별 데이터를 꼭짓점 셰이더로 보내는 데 사용됩니다.
struct VertexPositionColor : ConstantBuffer
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 col;

	VertexPositionColor(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color) 
		: pos { position }
		, col { color } 
	{}

	static inline CSOInfo GetCSOInfo()
	{
		return CSOInfo {
			  CSO_PC_VS, 0, 0, 0, CSO_PC_PS
			, nullptr, nullptr, nullptr, nullptr, nullptr 
		};
	}

	static inline vector<D3D11_INPUT_ELEMENT_DESC> GetInputElementDesc()
	{
		return vector<D3D11_INPUT_ELEMENT_DESC>{
			  D3D11_INPUT_ELEMENT_DESC { "POSITION"	, 0	, DXGI_FORMAT_R32G32B32_FLOAT	, 0	, 0		, D3D11_INPUT_PER_VERTEX_DATA	, 0 }
			, D3D11_INPUT_ELEMENT_DESC { "COLOR"	, 0	, DXGI_FORMAT_R32G32B32_FLOAT	, 0	, 12	, D3D11_INPUT_PER_VERTEX_DATA	, 0 }
		};
	}
};

// 텍스처와 포지션이 있는 상수 버퍼입니다.
struct PositionTextureConstantBuffer : ConstantBuffer
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texcoord;

	PositionTextureConstantBuffer(DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 textureCoord)
		: pos { position }
		, texcoord { textureCoord }
	{}
	
	static inline CSOInfo GetCSOInfo()
	{
		return CSOInfo {
//			  0, 0, 0, 0, 0
			  CSO_PT_VS, 0, 0, 0, CSO_PT_PS
			, TEXT("CSO\\Texture2DQuadVertexShader.cso"), nullptr, nullptr, nullptr, TEXT("CSO\\Texture2DQuadPixelShader.cso")
		};
	}
	static inline vector<D3D11_INPUT_ELEMENT_DESC> GetInputElementDesc()
	{
		return vector<D3D11_INPUT_ELEMENT_DESC >{
			  D3D11_INPUT_ELEMENT_DESC { "POSITION"	, 0	, DXGI_FORMAT_R32G32B32_FLOAT	, 0	, 0		, D3D11_INPUT_PER_VERTEX_DATA	, 0 }
			, D3D11_INPUT_ELEMENT_DESC { "TEXCOORD"	, 0	, DXGI_FORMAT_R32G32_FLOAT		, 0	, 12	, D3D11_INPUT_PER_VERTEX_DATA	, 0 }
		};
	}
};
