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
	_pFootballTexture = nullptr;
	_pPitchTexture = nullptr;

	_pSamplerLinear = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	srand(145);

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

	// _pImmediateContext->IASetVertexBuffers(0, 1, &_car.VertexBuffer, &_car.VBStride, &_car.VBOffset);
	// _pImmediateContext->IASetIndexBuffer(_car.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//
	// _pImmediateContext->IASetVertexBuffers(0, 1, &_skyBox.VertexBuffer, &_skyBox.VBStride, &_skyBox.VBOffset);
	// _pImmediateContext->IASetIndexBuffer(_skyBox.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//
	// _pImmediateContext->IASetVertexBuffers(8, 1, &_footballMesh.VertexBuffer, &_footballMesh.VBStride, &_footballMesh.VBOffset);
	// _pImmediateContext->IASetIndexBuffer(_footballMesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/Goal.dds", nullptr, &_pGoalTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/football.dds", nullptr, &_pFootballTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/footballPitch.dds", nullptr, &_pPitchTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/car.dds", nullptr, &_pRedCarTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/BlueCar.dds", nullptr, &_pBlueCarTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/fence.dds", nullptr, &_pSkyBoxTexture);
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/crowd.dds", nullptr, &_pCrowdTexture);

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

	// Initialise Model
	// _goalModel = new Model(OBJLoader::Load("Models/cube.obj", _pd3dDevice, false), L"Textures/Crate_COLOR.dds", _world, _pd3dDevice, _pImmediateContext, _pSamplerLinear);

	// _footballModel = new Model(OBJLoader::Load("Models/Hercules.obj", _pd3dDevice, true), L"Textures/football.dds", _worldX, _pd3dDevice, _pImmediateContext, _pSamplerLinear);

	const auto _cubeMesh = OBJLoader::Load("Models/cube.obj", _pd3dDevice, false);
	const auto _car = OBJLoader::Load("Models/car.obj", _pd3dDevice, false);
	const auto _footballMesh = OBJLoader::Load("Models/sphere.obj", _pd3dDevice, false);
	const auto _skyBox = OBJLoader::Load("Models/cube.obj", _pd3dDevice, true);

	Material shinyMaterial(XMFLOAT4(.3f, .3f, .3f, 1.f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		10.f);

	Model *cubeModel = new Model(_cubeMesh, shinyMaterial);
	cubeModel->SetTextureRV(_pBlueCarTexture);
	cubeModel->SetMatrix(blueCarWorld);
	modelList.emplace_back(cubeModel);

	Model *skyBoxModel = new Model(_footballMesh, shinyMaterial);
	skyBoxModel->SetTextureRV(_pSkyBoxTexture);
	skyBoxModel->SetMatrix(skyBoxWorld);
	modelList.emplace_back(skyBoxModel);

	//Initialise Lighting
	_lights = new Lights();

	// Initialise camera
	_camera = new Camera(_windowHeight, _windowWidth);

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

	const UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	CreateSamplerState();

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
    wcex.hIcon = LoadIcon(hInstance, LPCTSTR(IDI_TUTORIAL1));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"FYPWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, reinterpret_cast<LPCTSTR>(IDI_TUTORIAL1));
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, -30, 720, 720/1.5};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"FYPWindowClass", L"Cloth Simulation Framework", WS_OVERLAPPEDWINDOW,
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
            OutputDebugStringA(static_cast<char*>(pErrorBlob->GetBufferPointer()));

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

    const UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	const UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	const UINT sampleNum = 4;

	const auto sd = CreateSwapChain(sampleNum);

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

	SetViewport();

	InitShadersAndInputLayout();

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateConstantBuffer(hr);

	if (FAILED(hr))
		return hr;

	CreateDepthBuffer(sampleNum);

	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	CreateRasterizerState(hr);
	
    if (FAILED(hr))
        return hr;

	CreateBlendingEquations();

	return S_OK;

}

DXGI_SWAP_CHAIN_DESC Application::CreateSwapChain(const UINT &sampleNum) const
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _windowWidth;
	sd.BufferDesc.Height = _windowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // non negative, normalised number
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // pipeline will render to this buffer and then render to screen - render target
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleNum;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	return sd;
}

void Application::SetViewport() const
{
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = FLOAT(_windowWidth); //specifies which areas of the screen to render to
	vp.Height = FLOAT(_windowHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);	
}

void Application::CreateDepthBuffer(const UINT & sampleNum)
{
	//Setup depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = _windowWidth;
	depthStencilDesc.Height = _windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleNum;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
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

	_pd3dDevice->CreateBlendState(&blendDesc, &_blendState);
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
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);
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
	if (_pFootballTexture) _pFootballTexture->Release();
	if (_pBlueCarTexture) _pBlueCarTexture->Release();
	if (_pRedCarTexture) _pRedCarTexture->Release();
	if (_pSkyBoxTexture) _pSkyBoxTexture->Release();
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
	if (_blendState) _blendState->Release();

	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	for (Model *model : modelList)
		if (model)
		{
			delete model;
			model = nullptr;
		}
}

void Application::Update()
{
    // Update our time
    const float t = 1.f/60.f;

	_camera->Update();

	ObjectAnimation(t);	
	KeyboardFunctions();
}

void Application::ObjectAnimation(const float& t)
{
	//_goalModel->ObjectAnimation(t);
	//_footballModel->ObjectAnimation(t);
	XMStoreFloat4x4(&_pitchWorld, XMMatrixScaling(30.0f, 0.0f, 30.0f) * XMMatrixTranslation(0.0f, -5.0f, 0.0f));
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
		_camera->SetWorldPosition(XMFLOAT3(.0f, 5.f, .0f));

	_pImmediateContext->RSSetState(GetAsyncKeyState(VK_SPACE) ? _wireFrame : nullptr);
}

void Application::Draw()
{   
	// "fine-tune" the blending equation
	float blendFactor[4] = { 0.99f, 0.99f, 0.99f, 1.0f };

    // Clear the back buffer
    float ClearColor[4] = {0.30f, 0.20f, 0.60f, 1.0f}; // Background's RGBA values
	
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);	
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // Clears Depth Buffer

	// Sets up object textures
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	// Set up Shaders
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	// Set up constant buffers
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);

	// _pImmediateContext->PSSetShaderResources(0, 1, &_pFootballTexture);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	
	// Sets up camera view matrices
	XMMATRIX mainView = XMLoadFloat4x4(&_camera->GetViewMatrix());

	// Sets up camera projection matrices
	XMMATRIX mainProjection = XMLoadFloat4x4(&_camera->GetProjectionMatrix());    
		
	// Creates constant buffer
	ConstantBuffer cb;
    
	// Update Lighting variables
	cb.lightDirection = _lights->lightVecW;
	cb.padding = _lights->padding;
	cb.diffuseLight = _lights->diffuseLight;
	cb.ambientLight = _lights->ambientLight;
	cb.specularLight = _lights->specularLight;
	cb.specularPower = _lights->specularPower;

	// Updates camera position, view and projection
	cb.eyePosW = _camera->GetWorldPosition();
	cb.mView = mainView;
	cb.mProjection = mainProjection;
	
	// Draws Planet
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pFootballTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	for (auto model : modelList)
	{
		const auto material = model->GetMaterial();
		cb.diffuseMaterial = material.diffuse;
		cb.ambientMaterial = material.ambient;
		cb.specularMaterial = material.specular;

		cb.mWorld = XMMatrixTranspose(model->GetMatrix());

		if (model->HasTexture())
		{
			auto *tRV = model->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &tRV);
			cb.hasTexture = 1.f;
		}
		else
			cb.hasTexture = .0f;

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		model->Draw(_pImmediateContext, _blendState, false);
	}
	
	// _pImmediateContext->IASetVertexBuffers(0,1,&_pCubeVertexBuffer,&_vert)
	// _pImmediateContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);	// Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0); // Renders planet cube 

	// // Draws Sky Box
	// cb.mWorld = XMMatrixTranspose(skyBoxWorld);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pSkyBoxTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);	
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	//
	//
	// // Draws Pitch
	// cb.mWorld = XMMatrixTranspose(pitch);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pPitchTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the blend state for transparent objects
	// _pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	// // Renders Extra
	// _pImmediateContext->DrawIndexed(_skyBox.IndexCount, 0, 0);	
	//
	// // Draws Goal
	// cb.mWorld = XMMatrixTranspose(goal);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pGoalTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the blend state for transparent objects
	// _pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	// // Renders Extra
	// _pImmediateContext->DrawIndexed(_skyBox.IndexCount, 0, 0);
	//
	// // Draws Goal
	// cb.mWorld = XMMatrixTranspose(goal2);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pGoalTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the blend state for transparent objects
	// _pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	// // Renders Extra
	// _pImmediateContext->DrawIndexed(_skyBox.IndexCount, 0, 0);
	//
	// // Draws crowd
	// cb.mWorld = XMMatrixTranspose(crowd);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pSkyBoxTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);	
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	//
	//
	// // Draws crowd
	// cb.mWorld = XMMatrixTranspose(crowd2);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pSkyBoxTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	//
	//
	// // Draws crowd
	// cb.mWorld = XMMatrixTranspose(crowd3);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pSkyBoxTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	//
	// // Draws crowd
	// cb.mWorld = XMMatrixTranspose(crowd4);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pSkyBoxTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	//
	//
	// // Draws crowd
	// cb.mWorld = XMMatrixTranspose(crowd5);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pSkyBoxTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);
	//
	//
	// // Draws crowd
	// cb.mWorld = XMMatrixTranspose(crowd6);
	// // Sets up object textures
	// _pImmediateContext->PSSetShaderResources(0, 1, &_pSkyBoxTexture);
	// _pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	// _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	// // Set the default blend state (no blending) for opaque objects
	// _pImmediateContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);
	// _pImmediateContext->DrawIndexed(_cubeMesh.IndexCount, 0, 0);

	// Present our back buffer to our front buffer
    _pSwapChain->Present(1, 0);
}
