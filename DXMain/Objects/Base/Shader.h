#pragma once

/// <summary>
/// 추상 Shader 클래스입니다. 인스턴스 생성이 불가능합니다.
/// 실제 구현은 CShader에서 처리합니다.
/// </summary>
class CShaderBase {

public:
	
	
	CShaderBase() = delete;
	virtual ~CShaderBase() = 0;

	static inline vector<byte> ReadCompiledShaderOutputFile(LPCTSTR path);

	bool CreateVertexShaderAndInputLayout(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreateHullShader		(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreateDomainShader		(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreateGeometryShader	(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreatePixelShader		(LPCTSTR path, ID3D11Device* pd3dDevice);



	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

protected:

	CShaderBase(vector<D3D11_INPUT_ELEMENT_DESC>&& v) { m_vInputElementDescs = move(v); }

	vector<D3D11_INPUT_ELEMENT_DESC>		m_vInputElementDescs						;
	ID3D11InputLayout					*	m_pInputLayout					{ nullptr }	;
	ID3D11VertexShader					*	m_pVertexShader					{ nullptr }	;
	ID3D11PixelShader					*	m_pPixelShader					{ nullptr }	;
	ID3D11HullShader					*	m_pHullShader					{ nullptr }	;
	ID3D11DomainShader					*	m_pDomainShader					{ nullptr }	;
	ID3D11GeometryShader				*	m_pGeometryShader				{ nullptr }	;
	ID3D11VertexShader					*	m_pStreamOutputVertexShader		{ nullptr }	;
	ID3D11GeometryShader				*	m_pStreamOutputGeometryShader	{ nullptr }	;

};

template<	  typename DescType
			, bool _IsConstant = is_base_of<ConstantBuffer, DescType>::value
		>
class CShader;

template<typename DescType>
class CShader<DescType, true> : public CShaderBase {

public:

	CShader() : CShaderBase { DescType::GetInputElementDesc() } {}
	virtual ~CShader() {}

};
