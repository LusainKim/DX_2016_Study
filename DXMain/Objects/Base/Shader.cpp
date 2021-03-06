#include "stdafx.h"
#include "Shader.h"

CShaderBase::~CShaderBase()
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
/// CSO 파일을 읽어들여 vector로 반환합니다.
/// vector 형식은 byte 입니다.
/// </summary>
/// <param name = "path"> cso 경로입니다. </param>
inline vector<byte> CShaderBase::ReadCompiledShaderOutputFile(LPCTSTR path)
{
	ifstream fcso(path, ios::in | ios::binary);

	if (!fcso.is_open())
	{
		MessageBox(NULL, TEXT("Shader Load Fail!"), TEXT("프로그램 구동 실패"), MB_OK);
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
/// 리소스에 저장된 CSO 파일을 읽어들여 vector로 반환합니다.
/// vector 형식은 byte 입니다.
/// </summary>
/// <param name = "res"> 리소스 파일에서 부여한 cso 파일의 ID입니다. define된 단어입니다. </param>
inline vector<byte> CShaderBase::ReadCompliedShaderInterResource(int res)
{
	HINSTANCE hInst = GetHInstance();
	// Resource 검색
	HANDLE hResInfo = FindResource(hInst, MAKEINTRESOURCE(res), TEXT("CSO"));
	
	// Resource 불러오기
	HANDLE hRes = LoadResource(static_cast<HMODULE>(hInst), static_cast<HRSRC>(hResInfo));

	// 데이터의 시작주소를 가리키는 포인터 획득
	auto lpRes = reinterpret_cast<byte*>(LockResource(hRes));

	// size check
	auto szData = SizeofResource(hInst, static_cast<HRSRC>(hResInfo));

	// 반환할 vector 객체
	vector<byte> vbinarycso;
	vbinarycso.resize(szData);

	// 메모리 복사
	CopyMemory(&(vbinarycso[0]), lpRes, szData);

	return vbinarycso;
}

/// <summary>
/// <para>Vertex Shader 와 Input Layout 을 생성합니다.</para>
/// <para>반드시 <value> m_vInputElementDescs </value> 에 유효한 값이 있어야만 합니다.</para>
/// <para>m_vInputElementDescs 에 값이 없을 경우 오류가 발생하고 함수가 실패합니다.</para>
/// </summary>
/// <param name = "path"> cso 경로입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> 해당 함수는 m_vInputElementDescs 가 반드시 초기화되어 유효한 값이 있어야만 정상 동작합니다. </remarks>
/// <returns> 정상적으로 Vertex Shader 와 Input Layout 이 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateVertexShaderAndInputLayoutByPath(LPCTSTR path, ID3D11Device* pd3dDevice)
{
	SafeRelease(m_pVertexShader);
	SafeRelease(m_pInputLayout);

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
	if (FailureMessage(hr, TEXT("Vertex Shader 생성에 실패했습니다."))) return false;

	hr = pd3dDevice->CreateInputLayout(
		&(m_vInputElementDescs[0])
		, static_cast<UINT>(m_vInputElementDescs.size())
		, &(loadVS[0])
		, loadVS.size()
		, &m_pInputLayout
	);
	if (FailureMessage(hr, TEXT("Input Layout 생성에 실패했습니다."))) return false;

	return true;

}

/// <summary>
/// <para>Hull Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "path"> cso 경로입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Hull Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateHullShaderByPath(LPCTSTR path, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pHullShader);

	auto loadHS = ReadCompiledShaderOutputFile(path);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreateHullShader(
		  &(loadHS[0])
		, loadHS.size()
		, nullptr
		, &m_pHullShader
	);
	if (FailureMessage(hr, TEXT("Hull Shader 생성에 실패했습니다."))) return false;

	return true;
}

/// <summary>
/// <para>Domain Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "path"> cso 경로입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Domain Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateDomainShaderByPath(LPCTSTR path, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pDomainShader);
	auto loadDS = ReadCompiledShaderOutputFile(path);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreateDomainShader(
		  &(loadDS[0])
		, loadDS.size()
		, nullptr
		, &m_pDomainShader
	);
	if (FailureMessage(hr, TEXT("Domain Shader 생성에 실패했습니다."))) return false;

	return true;
}

/// <summary>
/// <para>Geometry Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "path"> cso 경로입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Geometry Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateGeometryShaderByPath(LPCTSTR path, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pGeometryShader);
	auto loadGS = ReadCompiledShaderOutputFile(path);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreateGeometryShader(
		  &(loadGS[0])
		, loadGS.size()
		, nullptr
		, &m_pGeometryShader
	);
	if (FailureMessage(hr, TEXT("Geometry Shader 생성에 실패했습니다."))) return false;

	return true;
}

/// <summary>
/// <para>Pixel Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "path"> cso 경로입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Pixel Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreatePixelShaderByPath(LPCTSTR path, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pPixelShader);
	auto loadPS = ReadCompiledShaderOutputFile(path);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreatePixelShader(
		  &(loadPS[0])
		, loadPS.size()
		, nullptr
		, &m_pPixelShader
	);
	if (FailureMessage(hr, TEXT("Pixel Shader 생성에 실패했습니다."))) return false;

	return true;
}



/// <summary>
/// <para>Vertex Shader 와 Input Layout 을 생성합니다.</para>
/// <para>반드시 <value> m_vInputElementDescs </value> 에 유효한 값이 있어야만 합니다.</para>
/// <para>m_vInputElementDescs 에 값이 없을 경우 오류가 발생하고 함수가 실패합니다.</para>
/// </summary>
/// <param name = "resource"> 리소스 파일에서 부여한 cso 파일의 ID입니다. define된 단어입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> 해당 함수는 m_vInputElementDescs 가 반드시 초기화되어 유효한 값이 있어야만 정상 동작합니다. </remarks>
/// <returns> 정상적으로 Vertex Shader 와 Input Layout 이 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateVertexShaderAndInputLayoutByResource(int resource, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pVertexShader);
	SafeRelease(m_pInputLayout);

	if (m_vInputElementDescs.empty())
	{
		_DEBUG_ERROR("ElementDescription is Empty!");
		return false;
	}

	HRESULT hr = S_OK;

	auto loadVS = ReadCompliedShaderInterResource(resource);

	hr = pd3dDevice->CreateVertexShader(
		&(loadVS[0])
		, loadVS.size()
		, nullptr
		, &m_pVertexShader
	);
	if (FailureMessage(hr, TEXT("Vertex Shader 생성에 실패했습니다."))) return false;

	hr = pd3dDevice->CreateInputLayout(
		&(m_vInputElementDescs[0])
		, static_cast<UINT>(m_vInputElementDescs.size())
		, &(loadVS[0])
		, loadVS.size()
		, &m_pInputLayout
	);
	if (FailureMessage(hr, TEXT("Input Layout 생성에 실패했습니다."))) return false;

	return true;

}

/// <summary>
/// <para>Hull Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "resource"> 리소스 파일에서 부여한 cso 파일의 ID입니다. define된 단어입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Hull Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateHullShaderByResource(int resource, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pHullShader);

	auto loadHS = ReadCompliedShaderInterResource(resource);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreateHullShader(
		  &(loadHS[0])
		, loadHS.size()
		, nullptr
		, &m_pHullShader
	);
	if (FailureMessage(hr, TEXT("Hull Shader 생성에 실패했습니다."))) return false;

	return true;
}

/// <summary>
/// <para>Domain Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "resource"> 리소스 파일에서 부여한 cso 파일의 ID입니다. define된 단어입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Domain Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateDomainShaderByResource(int resource, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pDomainShader);
	auto loadDS = ReadCompliedShaderInterResource(resource);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreateDomainShader(
		  &(loadDS[0])
		, loadDS.size()
		, nullptr
		, &m_pDomainShader
	);
	if (FailureMessage(hr, TEXT("Domain Shader 생성에 실패했습니다."))) return false;

	return true;
}

/// <summary>
/// <para>Geometry Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "resource"> 리소스 파일에서 부여한 cso 파일의 ID입니다. define된 단어입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Geometry Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreateGeometryShaderByResource(int resource, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pGeometryShader);
	auto loadGS = ReadCompliedShaderInterResource(resource);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreateGeometryShader(
		  &(loadGS[0])
		, loadGS.size()
		, nullptr
		, &m_pGeometryShader
	);
	if (FailureMessage(hr, TEXT("Geometry Shader 생성에 실패했습니다."))) return false;

	return true;
}

/// <summary>
/// <para>Pixel Shader 를 생성합니다.</para>
/// </summary>
/// <param name = "resource"> 리소스 파일에서 부여한 cso 파일의 ID입니다. define된 단어입니다. </param>
/// <param name = "pd3dDevice"> d3dDevice입니다. </param>
/// <remarks> </remarks>
/// <returns> 정상적으로 Pixel Shader 가 생성되면 true, 그 이외의 모든 경우는 false를 반환합니다. </returns>
bool CShaderBase::CreatePixelShaderByResource(int resource, ID3D11Device * pd3dDevice)
{
	SafeRelease(m_pPixelShader);
	auto loadPS = ReadCompliedShaderInterResource(resource);

	HRESULT hr = S_OK;

	hr = pd3dDevice->CreatePixelShader(
		  &(loadPS[0])
		, loadPS.size()
		, nullptr
		, &m_pPixelShader
	);
	if (FailureMessage(hr, TEXT("Pixel Shader 생성에 실패했습니다."))) return false;

	return true;
}



void CShaderBase::OnPrepareRender(ID3D11DeviceContext * pd3dDeviceContext)
{
	if (m_pInputLayout)		pd3dDeviceContext->IASetInputLayout(m_pInputLayout);
	if (m_pVertexShader)	pd3dDeviceContext->VSSetShader(m_pVertexShader		, NULL, 0);
	if (m_pPixelShader)		pd3dDeviceContext->PSSetShader(m_pPixelShader		, NULL, 0);
	if (m_pGeometryShader)	pd3dDeviceContext->GSSetShader(m_pGeometryShader	, NULL, 0);
	if (m_pHullShader)		pd3dDeviceContext->HSSetShader(m_pHullShader		, NULL, 0);
	if (m_pDomainShader)	pd3dDeviceContext->DSSetShader(m_pDomainShader		, NULL, 0);
}

void CShaderBase::Render(ID3D11DeviceContext * pd3dDeviceContext)
{
	OnPrepareRender(pd3dDeviceContext);
}
