#include "Application.h"

HRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pCubeVertexBuffer = nullptr;
	_pCubeIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	
	// Texture Pointers
	_pGoalTexture = nullptr;
	_pfootballTexture = nullptr;
	_pPitchTexture = nullptr;

	_pSamplerLinear = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	carPosition = XMFLOAT3(0.0f, -4.0f, 5.0f); // Planet object size

	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		return E_FAIL;
	}

	RECT rc;
	GetClientRect(_hWnd, &rc);
	_windowWidth = rc.right - rc.left;
	_windowHeight = rc.bottom - rc.top;


	if (FAILED(InitDevice()))
	{
		Cleanup();

		return E_FAIL;
	}

	// Initialize the world matrix
	XMStoreFloat4x4(&_redCarWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_blueCarWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_footballWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_skyBoxWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_pitchWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_goalWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_goalWorld2, XMMatrixIdentity());
	XMStoreFloat4x4(&_crowdWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_crowdWorld2, XMMatrixIdentity());
	XMStoreFloat4x4(&_crowdWorld3, XMMatrixIdentity());
	XMStoreFloat4x4(&_crowdWorld4, XMMatrixIdentity());
	XMStoreFloat4x4(&_crowdWorld5, XMMatrixIdentity());
	XMStoreFloat4x4(&_crowdWorld6, XMMatrixIdentity());

	//Initialise Lighting
	_lights = new Lights();

	// Initialise camera
	for (int i = 0; i < noOfCameras; i++)
	{
		_camera[i] = new Camera(_windowHeight, _windowWidth);

		switch (i)
		{
		case 0: _camera[i]->SetWorldPosition(XMFLOAT3(rand() % 10, rand() % 10, -(rand() % 10) + 1));
			break;
		case 1:_camera[i]->SetWorldPosition(XMFLOAT3(10.0f, 10.0f, -30.0f));
			break;
		case 2:
			_camera[i]->SetWorldPosition(XMFLOAT3(0.0f, 25.0f, -0.05f));
			break;
		case 3: 
			_camera[i]->SetWorldPosition(XMFLOAT3(-6.0f, 0.0f, 5.0f));
			_camera[i]->SetAt(XMFLOAT3(_currentCarPos.x, _currentCarPos.y, _currentCarPos.z));
			break;
		default:
			break;

		}

		_camera[i]->CameraMovement();

	}
		CreateSamplerState();

		return S_OK;
}

void Application::CreateSamplerState()
{
	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, -30, 720, 720/1.5};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width =  _windowWidth;
    sd.BufferDesc.Height = _windowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // non negative, normalised number
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // pipeline will render to this buffer and then render to screen - render target
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext); //relays a poiter to the device and the immediate contents
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	//Setup depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width =  _windowWidth;
	depthStencilDesc.Height = _windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);


    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT) _windowWidth; //specifies whoch areas of the screen to render to
    vp.Height = (FLOAT)_windowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	//InitCubeVertexBuffer();

    // Set vertex buffer
    //UINT stride = sizeof(SimpleVertex);
    //UINT offset = 0;
	//_pImmediateContext->IASetVertexBuffers(0, 1, &_pCubeVertexBuffer, &stride, &offset);

	_pImmediateContext2 = _pImmediateContext;

	_cubeMesh = OBJLoader::Load("Models/cube.obj", _pd3dDevice, false);
	_car = OBJLoader::Load("Models/car.obj", _pd3dDevice, false);
	//_footballMesh = OBJLoader::Load("Models/sphere.obj", _pd3dDevice, false);
	_skyBox = OBJLoader::Load("Models/cube.obj", _pd3dDevice, true);

	
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/Goal.dds", nullptr, &_pGoalTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/football.dds", nullptr, &_pfootballTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/footballPitch.dds", nullptr, &_pPitchTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/car.dds", nullptr, &_pRedCarTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/BlueCar.dds", nullptr, &_pBlueCarTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/fence.dds", nullptr, &_pskyBoxTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/crowd.dds", nullptr, &_pCrowdTexture);

	_pImmediateContext2->IASetVertexBuffers(0, 1, &_car.VertexBuffer, &_car.VBStride, &_car.VBOffset);
	_pImmediateContext2->IASetIndexBuffer(_car.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    _pImmediateContext->IASetVertexBuffers(0, 1, &_skyBox.VertexBuffer, &_skyBox.VBStride, &_skyBox.VBOffset);
	_pImmediateContext->IASetIndexBuffer(_skyBox.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	
	//_pImmediateContext->IASetVertexBuffers(8, 1, &_footballMesh.VertexBuffer, &_footballMesh.VBStride, &_footballMesh.VBOffset);
	//_pImmediateContext->IASetIndexBuffer(_footballMesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	


	
	//InitCubeIndexBuffer();

    // Set index buffer
    //_pImmediateContext->IASetIndexBuffer(_pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateConstantBuffer(hr);

	CreateRasterizerState(hr);


	// Initialise Model
	//_goalModel = new Model(OBJLoader::Load("Models/cube.obj", _pd3dDevice, false), L"Textures/Crate_COLOR.dds", _world, _pd3dDevice, _pImmediateContext, _pSamplerLinear);

//	_footballModel = new Model(OBJLoader::Load("Models/Hercules.obj", _pd3dDevice, true), L"Textures/football.dds", _worldX, _pd3dDevice, _pImmediateContext, _pSamplerLinear);

    if (FAILED(hr))
        return hr;

	CreateBlendingEquations();

	return S_OK;

}

void Application::CreateBlendingEquations()
{
	//Create and defines blending equations
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	_pd3dDevice->CreateBlendState(&blendDesc, &_transparency);
}

void Application::CreateConstantBuffer(HRESULT hr)
{
	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);
}

void Application::CreateRasterizerState(HRESULT hr)
{
	//Create a Wire frame view
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	/*//No backface culling
	D3D11_RASTERIZER_DESC noBackCull;
	ZeroMemory(&noBackCull, sizeof(D3D11_RASTERIZER_DESC));
	noBackCull.FillMode = D3D11_FILL_SOLID;
	noBackCull.CullMode = D3D11_CULL_NONE;
	noBackCull.FrontCounterClockwise = false;
	noBackCull.DepthClipEnable = true;

	ID3D11RasterizerState* noBackCullRS;
	hr = (_pd3dDevice->CreateRasterizerState(&noBackCull, &noBackCullRS));*/
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pCubeVertexBuffer) _pCubeVertexBuffer->Release();
    if (_pCubeIndexBuffer) _pCubeIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	
	// Release textures
	if (_pfootballTexture) _pfootballTexture->Release();
	if (_pBlueCarTexture) _pBlueCarTexture->Release();
	if (_pRedCarTexture) _pRedCarTexture->Release();
	if (_pskyBoxTexture) _pskyBoxTexture->Release();
	if (_pCrowdTexture) _pCrowdTexture->Release();
	if (_pPitchTexture) _pPitchTexture->Release();
	if (_pGoalTexture) _pGoalTexture->Release();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	//Releases COM objects
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	//Release wireframe
	if (_wireFrame) _wireFrame->Release();

	//Release alpha blender
	if (_transparency) _transparency->Release();

	for (int i = 0; i < noOfCameras; i++)
	{
		if (_camera[i]) _camera[i] = nullptr;
	}
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

	_camera[_currentCamera]->UpdateFrameTimer(t);

	ObjectAnimation(t);
	
	KeyboardFunctions();
}

void Application::ObjectAnimation(float t)
{
	// Animate the cube
	XMStoreFloat4x4(&_redCarWorld, XMMatrixTranslation(carPosition.x, carPosition.y, carPosition.z));
	XMStoreFloat4x4(&_blueCarWorld, XMMatrixTranslation(-carPosition.x, carPosition.y, -carPosition.z));
	XMStoreFloat4x4(&_pitchWorld, XMMatrixScaling(30.0f, 0.0f, 30.0f) * XMMatrixTranslation(0.0f, -5.0f, 0.0f));
	//_goalModel->ObjectAnimation(t);
	//_footballModel->ObjectAnimation(t);
	XMStoreFloat4x4(&_footballWorld, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationX(t * 1.5) * XMMatrixRotationZ(1.5 * t));
	XMStoreFloat4x4(&_skyBoxWorld, XMMatrixScaling(30.0f, 30.0f, 30.0f));
	
	XMStoreFloat4x4(&_goalWorld, XMMatrixScaling(6.0f, 6.0f, 7.0f) * XMMatrixTranslation(0.0f, -5.0f, 30.0f));
	XMStoreFloat4x4(&_goalWorld2, XMMatrixScaling(6.0f, 6.0f, 7.0f) * XMMatrixTranslation(0.0f, -5.0f, -30.0f));

	XMStoreFloat4x4(&_crowdWorld, XMMatrixScaling(30.0f, 30.0f, 15.0f) * XMMatrixTranslation(0.0f, -5.0f, 45.0f));
	XMStoreFloat4x4(&_crowdWorld2, XMMatrixScaling(30.0f, 30.0f, 15.0f) * XMMatrixTranslation(0.0f, -5.0f, -45.0f));
	XMStoreFloat4x4(&_crowdWorld3, XMMatrixScaling(30.0f, 30.0f, 15.0f) * XMMatrixTranslation(-60.0f, -5.0f, 22.0f));
	XMStoreFloat4x4(&_crowdWorld4, XMMatrixScaling(30.0f, 30.0f, 15.0f) * XMMatrixTranslation(60.0f, -5.0f, 22.0f));
	XMStoreFloat4x4(&_crowdWorld5, XMMatrixScaling(30.0f, 30.0f, 15.0f) * XMMatrixTranslation(-60.0f, -5.0f, -22.0f));
	XMStoreFloat4x4(&_crowdWorld6, XMMatrixScaling(30.0f, 30.0f, 15.0f) * XMMatrixTranslation(60.0f, -5.0f, -22.0f));

}

void Application::KeyboardFunctions()
{
	if (GetAsyncKeyState('R') & 0x8000)
	{
		_camera[2]->SetWorldPosition(XMFLOAT3(_camera[2]->GetWorldPosition().x + 0.05f, _camera[2]->GetWorldPosition().y + 0.05f, _camera[2]->GetWorldPosition().z));
		_camera[2]->CameraMovement();
	}


	if (_currentCamera < 2)
	{

		_camera[_currentCamera]->CameraMovement();
	}


	if (GetAsyncKeyState('1') & 0x8000)
	{
		_currentCamera = 0;
	}

	if (GetAsyncKeyState('2') & 0x8000)
	{
		_currentCamera = 1;
	}

	if (GetAsyncKeyState('3') & 0x8000)
	{
		_currentCamera = 2;
	}

	if (GetAsyncKeyState('4') & 0x8000)
	{
		_currentCamera = 3;
	}


	if (GetAsyncKeyState(VK_SPACE))
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}
	else
	{
		_pImmediateContext->RSSetState(nullptr);
	}


	_currentCamPos = _camera[_currentCamera]->GetAt();


	if (GetAsyncKeyState('I') & 0x8000)
	{
		carPosition.z += 0.5f;
		if (_currentCamera == 3)
		{
			_camera[_currentCamera]->SetWorldPosition(XMFLOAT3(_currentCamPos.x, _currentCamPos.y, _currentCamPos.z + 0.5f));
			//_camera[_currentCamera]->SetAt(XMFLOAT3(_currentCarPos.x, _currentCarPos.y, _currentCarPos.z + 0.5f));
			_camera[_currentCamera]->CameraMovement();
		}
	}
	if (GetAsyncKeyState('K') & 0x8000)
	{
		carPosition.z -= 0.5f;
		if (_currentCamera == 3)
		{
			_camera[_currentCamera]->SetWorldPosition(XMFLOAT3(_currentCamPos.x, _currentCamPos.y, _currentCamPos.z - 0.5f));
			//_camera[_currentCamera]->SetAt(XMFLOAT3(_currentCarPos.x, _currentCarPos.y, _currentCarPos.z - 0.5f));
			_camera[_currentCamera]->CameraMovement();
		}
	}
	if (GetAsyncKeyState('L') & 0x8000)
	{
		carPosition.x += 0.5f;
		if (_currentCamera == 3)
		{
			_camera[_currentCamera]->SetWorldPosition(XMFLOAT3(_currentCamPos.x + 0.5f, _currentCamPos.y, _currentCamPos.z));
			//_camera[_currentCamera]->SetAt(XMFLOAT3(_currentCarPos.x + 0.5f, _currentCarPos.y, _currentCarPos.z));
			_camera[_currentCamera]->CameraMovement();
		}
	}
	if (GetAsyncKeyState('J') & 0x8000)
	{
		carPosition.x -= 0.5f;
		if (_currentCamera == 3)
		{
			_camera[_currentCamera]->SetWorldPosition(XMFLOAT3(_currentCamPos.x - 0.5f, _currentCamPos.y, _currentCamPos.z));
			//_camera[_currentCamera]->SetAt(XMFLOAT3(_currentCarPos.x -= 0.5f, _currentCarPos.y, _currentCarPos.z));
			_camera[_currentCamera]->CameraMovement();
		}
	}
}

void Application::Draw()
{   
	// Clears Depth Buffer
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_pImmediateContext2->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    // Clear the back buffer
    float ClearColor[4] = {0.30f, 0.20f, 0.60f, 1.0f}; // Background's RGBA values
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

	// "fine-tune" the blending equation
	float blendFactor[4] = { 0.99f, 0.99f, 0.99f, 1.0f };

	// Sets up world space
	XMMATRIX redCarWorld = XMLoadFloat4x4(&_redCarWorld);
	XMMATRIX blueCarWorld = XMLoadFloat4x4(&_blueCarWorld);
	XMMATRIX skyBoxWorld = XMLoadFloat4x4(&_skyBoxWorld);
	XMMATRIX footballWorld = XMLoadFloat4x4(&_footballWorld);
	XMMATRIX pitch = XMLoadFloat4x4(&_pitchWorld);
	XMMATRIX goal = XMLoadFloat4x4(&_goalWorld);
	XMMATRIX goal2 = XMLoadFloat4x4(&_goalWorld2);
	XMMATRIX crowd = XMLoadFloat4x4(&_crowdWorld);
	XMMATRIX crowd2 = XMLoadFloat4x4(&_crowdWorld2);
	XMMATRIX crowd3 = XMLoadFloat4x4(&_crowdWorld3);
	XMMATRIX crowd4 = XMLoadFloat4x4(&_crowdWorld4);	
	XMMATRIX crowd5 = XMLoadFloat4x4(&_crowdWorld5);
	XMMATRIX crowd6 = XMLoadFloat4x4(&_crowdWorld6);

	
	// Sets up camera view matrices
	XMMATRIX mainView = XMLoadFloat4x4(&_camera[_currentCamera]->GetViewMatrix());

	// Sets up camera projection matrices
	XMMATRIX mainProjection = XMLoadFloat4x4(&_camera[_currentCamera]->GetProjectionMatrix());
    
	// Creates constant buffer
	ConstantBuffer cb;
    
	// Update Lighting variables
	cb.lightDirection = _lights->lightVecW;
	cb.padding = _lights->padding;
	cb.diffuseLight = _lights->diffuseLight;
	cb.diffuseMaterial = _lights->diffuseMtrl;
	cb.ambientLight = _lights->ambientLight;
	cb.ambientMaterial = _lights->ambientMtrl;
	cb.specularLight = _lights->specularLight;
	cb.specularMaterial = _lights->specularMtrl;
	cb.specularPower = _lights->specularPower;

	// Update variables
	cb.mWorld = XMMatrixTranspose(redCarWorld);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Updates camera eye world position
	cb.eyePosW = _camera[_currentCamera]->GetWorldPosition();


	// Sets up object textures
	_pImmediateContext2->PSSetShaderResources(0, 1, &_pRedCarTexture);
	_pImmediateContext2->PSSetSamplers(0, 1, &_pSamplerLinear);
	// Uses constant buffers in shader
	_pImmediateContext2->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the blend state for transparent objects
	_pImmediateContext2->OMSetBlendState(0, 0, 0xffffffff);
    // Sends shader data
	_pImmediateContext2->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext2->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext2->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext2->PSSetShader(_pPixelShader, nullptr, 0);
	// Renders sun cube 
	_pImmediateContext2->DrawIndexed(_car.IndexCount, 0, 0);
	

	cb.mWorld = XMMatrixTranspose(blueCarWorld);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext2->PSSetShaderResources(0, 1, &_pBlueCarTexture);
	_pImmediateContext2->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext2->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext2->OMSetBlendState(0, 0, 0xffffffff);
	// Renders planet cube 
	_pImmediateContext2->DrawIndexed(_car.IndexCount, 0, 0);

	// Draws Planet
	cb.mWorld = XMMatrixTranspose(footballWorld);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pfootballTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	// Renders planet cube 
	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);

	// Draws Sky Box
	cb.mWorld = XMMatrixTranspose(skyBoxWorld);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pskyBoxTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);
	
	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);


	// Draws Pitch
	cb.mWorld = XMMatrixTranspose(pitch);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pPitchTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the blend state for transparent objects
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	// Renders Extra
	_pImmediateContext->DrawIndexed(_skyBox.IndexCount, 0, 0);	

	// Draws Goal
	cb.mWorld = XMMatrixTranspose(goal);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pGoalTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the blend state for transparent objects
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	// Renders Extra
	_pImmediateContext->DrawIndexed(_skyBox.IndexCount, 0, 0);

	// Draws Goal
	cb.mWorld = XMMatrixTranspose(goal2);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pGoalTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the blend state for transparent objects
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	// Renders Extra
	_pImmediateContext->DrawIndexed(_skyBox.IndexCount, 0, 0);

	// Draws crowd
	cb.mWorld = XMMatrixTranspose(crowd);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pskyBoxTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);

	
	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);


	// Draws crowd
	cb.mWorld = XMMatrixTranspose(crowd2);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pskyBoxTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);


	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	

	// Draws crowd
	cb.mWorld = XMMatrixTranspose(crowd3);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pskyBoxTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);


	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);

	// Draws crowd
	cb.mWorld = XMMatrixTranspose(crowd4);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pskyBoxTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);


	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);


	// Draws crowd
	cb.mWorld = XMMatrixTranspose(crowd5);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pskyBoxTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);


	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);


	// Draws crowd
	cb.mWorld = XMMatrixTranspose(crowd6);
	cb.mView = XMMatrixTranspose(mainView);
	cb.mProjection = XMMatrixTranspose(mainProjection);
	// Sets up object textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pskyBoxTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// Set the default blend state (no blending) for opaque objects
	_pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);


	_pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	//_goalModel->DrawModel(_pVertexShader, _pPixelShader, _pConstantBuffer, _camera, _currentCamera, _transparency, false);

	//_footballModel->DrawModel(_pVertexShader, _pPixelShader, _pConstantBuffer, _camera, _currentCamera, _transparency, true);

	// Present our back buffer to our front buffer
    _pSwapChain->Present(1, 0);
}
