#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <cstdlib>

#include "Model.h"
#include "Lights.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "OBJLoader.h"
#include "resource.h"
#include "ImageLoader.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	ID3D11DeviceContext* _pImmediateContext2;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pCubeVertexBuffer;
	ID3D11Buffer*           _pCubeIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;

	// Sets Textures
	ID3D11Texture2D* _pTexture2D;
	ID3D11ShaderResourceView* _pRedCarTexture;
	ID3D11ShaderResourceView* _pBlueCarTexture;
	ID3D11ShaderResourceView* _pskyBoxTexture;
	ID3D11ShaderResourceView* _pGoalTexture;
	ID3D11ShaderResourceView* _pfootballTexture;
	ID3D11ShaderResourceView* _pPitchTexture;
	ID3D11ShaderResourceView* _pCrowdTexture;
	ID3D11SamplerState * _pSamplerLinear;

	// Sets the wireframe state
	ID3D11RasterizerState* _wireFrame;

	// Depth buffer
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;

	// Blend State
	ID3D11BlendState* _transparency;

	// Object Lighting
	Lights* _lights;

	// World Matrix
	XMFLOAT4X4              _redCarWorld;
	XMFLOAT4X4              _blueCarWorld;
	XMFLOAT4X4				_skyBoxWorld;
	XMFLOAT4X4              _footballWorld;
	XMFLOAT4X4				_pitchWorld;
	XMFLOAT4X4				_goalWorld;
	XMFLOAT4X4				_goalWorld2;
	XMFLOAT4X4				_crowdWorld;
	XMFLOAT4X4				_crowdWorld2;
	XMFLOAT4X4				_crowdWorld3;
	XMFLOAT4X4				_crowdWorld4;
	XMFLOAT4X4				_crowdWorld5;
	XMFLOAT4X4				_crowdWorld6;

	// World Cameras
	Camera* _camera;

	// car position
	XMFLOAT3 carPosition;
	
	// Mesh Data
	MeshData _cubeMesh;
	MeshData _footballMesh;
	MeshData _skyBox;
	MeshData _car;
private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();

	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	UINT _windowHeight;
	UINT _windowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();

	void CreateConstantBuffer(HRESULT hr);
	void CreateRasterizerState(HRESULT hr);
	void CreateBlendingEquations();
	void CreateSamplerState();
	void ObjectAnimation(float t);
	void KeyboardFunctions();
};

