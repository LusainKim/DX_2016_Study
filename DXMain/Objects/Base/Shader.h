#pragma once

class CShader {

public:

	CShader() = default;
	virtual ~CShader();

	static inline vector<byte> ReadCompiledShaderOutputFile(LPCTSTR path);

	virtual bool DefineInputElementDescriptions() = 0;

	bool CreateVertexShaderAndInputLayout(LPCTSTR path, ID3D11Device* pd3dDevice);
	bool CreatePixelShader(LPCTSTR path, ID3D11Device* pd3dDevice);



	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

protected:

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