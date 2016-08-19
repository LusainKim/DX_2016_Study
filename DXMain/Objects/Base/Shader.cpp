#include "stdafx.h"
#include "Shader.h"

CShader::~CShader()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexShader);
	SafeRelease(m_pPixelShader);
	SafeRelease(m_pHullShader);
	SafeRelease(m_pDomainShader);
	SafeRelease(m_pGeometryShader);
	SafeRelease(m_pStreamOutputVertexShader);
	SafeRelease(m_pStreamOutputGeometryShader);
}

/// <summary>
/// CSO ������ �о�鿩 vector�� ��ȯ�մϴ�.
/// vector ������ byte �Դϴ�.
/// </summary>
/// <param name = "path"> cso ����Դϴ�. </param>
inline vector<byte> CShader::ReadCompiledShaderOutputFile(LPCTSTR path)
{
	ifstream fcso(path, ios::in | ios::binary);

	if (!fcso.is_open())
	{
		MessageBox(NULL, TEXT("Shader Load Fail!"), TEXT("���α׷� ���� ����"), MB_OK);
		return vector<byte>();
	}

	// size check;
	fcso.seekg(0, fstream::end);
	size_t szData = fcso.tellg();
	fcso.seekg(0);

	vector<byte> vbinarycso;
	vbinarycso.resize(szData);

	fcso.read(reinterpret_cast<char*>(&(vbinarycso[0])), szData);
	fcso.close();

	return vbinarycso;
}

/// <summary>
/// <para>Vertex Shader �� Input Layout �� �����մϴ�.</para>
/// <para>�ݵ�� <value> m_vInputElementDescs </value> �� ��ȿ�� ���� �־�߸� �մϴ�.</para>
/// <para>m_vInputElementDescs �� ���� ���� ��� ������ �߻��ϰ� �Լ��� �����մϴ�.</para>
/// </summary>
/// <param name = "path"> cso ����Դϴ�. </param>
/// <param name = "pd3dDevice"> d3dDevice�Դϴ�. </param>
/// <remarks> �ش� �Լ��� m_vInputElementDescs �� �ݵ�� �ʱ�ȭ�Ǿ� ��ȿ�� ���� �־�߸� ���� �����մϴ�. </remarks>
/// <returns> ���������� Vertex Shader �� Input Layout �� �����Ǹ� true, �� �̿��� ��� ���� false�� ��ȯ�մϴ�. </returns>
bool CShader::CreateVertexShaderAndInputLayout(LPCTSTR path, ID3D11Device* pd3dDevice)
{
	if (m_vInputElementDescs.empty())
	{
		_DEBUG_ERROR("ElementDescription is Empty!");
		return false;
	}

	HRESULT hr = S_OK;

	auto loadVS = ReadCompiledShaderOutputFile(path);

	hr = pd3dDevice->CreateVertexShader(
		&(loadVS[0])
		, loadVS.size()
		, nullptr
		, &m_pVertexShader
	);
	if (FailureMessage(hr, TEXT("Vertex Shader ������ �����߽��ϴ�."))) return false;

	//	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	//	{
	//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	};

	hr = pd3dDevice->CreateInputLayout(
		&(m_vInputElementDescs[0])
		, static_cast<UINT>(m_vInputElementDescs.size())
		, &(loadVS[0])
		, loadVS.size()
		, &m_pInputLayout
	);
	if (FailureMessage(hr, TEXT("Input Layout ������ �����߽��ϴ�."))) return false;

	return true;

}

/// <summary>
/// <para>Pixel Shader �� �����մϴ�.</para>
/// </summary>
/// <param name = "path"> cso ����Դϴ�. </param>
/// <param name = "pd3dDevice"> d3dDevice�Դϴ�. </param>
/// <remarks> </remarks>
/// <returns> ���������� Pixel Shader �� �����Ǹ� true, �� �̿��� ��� ���� false�� ��ȯ�մϴ�. </returns>
bool CShader::CreatePixelShader(LPCTSTR path, ID3D11Device * pd3dDevice)
{
	auto loadPS = ReadCompiledShaderOutputFile(path);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreatePixelShader(
		  &(loadPS[0])
		, loadPS.size()
		, nullptr
		, &m_pPixelShader
	);
	if (FailureMessage(hr, TEXT("Pixel Shader ������ �����߽��ϴ�."))) return false;

	return true;
}

void CShader::OnPrepareRender(ID3D11DeviceContext * pd3dDeviceContext)
{
	if (m_pInputLayout)		pd3dDeviceContext->IASetInputLayout(m_pInputLayout);
	if (m_pVertexShader)	pd3dDeviceContext->VSSetShader(m_pVertexShader		, NULL, 0);
	if (m_pPixelShader)		pd3dDeviceContext->PSSetShader(m_pPixelShader		, NULL, 0);
	if (m_pGeometryShader)	pd3dDeviceContext->GSSetShader(m_pGeometryShader	, NULL, 0);
	if (m_pHullShader)		pd3dDeviceContext->HSSetShader(m_pHullShader		, NULL, 0);
	if (m_pDomainShader)	pd3dDeviceContext->DSSetShader(m_pDomainShader		, NULL, 0);
}

void CShader::Render(ID3D11DeviceContext * pd3dDeviceContext)
{
	OnPrepareRender(pd3dDeviceContext);
}
