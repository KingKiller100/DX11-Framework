#include "Model.h"
#include "DDSTextureLoader.h"
#include "ConstantBuffer.h"

Model::Model(MeshData meshData, wchar_t* texturePath, XMFLOAT4X4 worldMatrix, ID3D11Device * device, ID3D11DeviceContext* deviceContext, ID3D11SamplerState* samplerLinear)
{
	_meshData = meshData;
	_worldMatrix = worldMatrix;
	_transparent = false;

	_device = device;
	_deviceContext = deviceContext;
	_pSamplerLinear = samplerLinear;

	_lights = new Lights();

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

	_device->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	CreateDDSTextureFromFile(_device, texturePath, nullptr, &_pTextureRV);
	_deviceContext->IASetVertexBuffers(0, 1, &_meshData.VertexBuffer, &_meshData.VBStride, &_meshData.VBOffset);
	_deviceContext->IASetIndexBuffer(_meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

Model::~Model()
{
	if(_device) _device->Release();
	if (_deviceContext) _deviceContext->Release();
	if (_pSamplerLinear) _pSamplerLinear->Release();
	if (_lights) { delete _lights;  _lights = nullptr; }
	if (_pTextureRV) _pTextureRV->Release();
}

void Model::ObjectAnimation(float t)
{
	// Animate the cube
	XMStoreFloat4x4(&_worldMatrix, XMMatrixRotationZ(t) * XMMatrixRotationY(t) * XMMatrixRotationX(t));
}

void Model::Draw(ID3D11VertexShader* _pVS, ID3D11PixelShader* _pPS, ID3D11Buffer* _pCB, Camera* _camera, ID3D11BlendState* _transparency, const bool& transparency) const
{
	// "fine-tune" the blending equation
	const float blendFactor[4] = { 0.95f, 0.95f, 0.995f, 1.0f };

	if (_transparent)
	{
		// Set the blend state for transparent objects
		_deviceContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);
	}
	else
	{
		// Set the blend state for transparent objects
		_deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}
	
	// Sends shader data
	_deviceContext->VSSetShader(_pVS, nullptr, 0);
	_deviceContext->VSSetConstantBuffers(0, 1, &_pCB);
	_deviceContext->PSSetConstantBuffers(0, 1, &_pCB);
	_deviceContext->PSSetShader(_pPS, nullptr, 0);

	// Renders
	_deviceContext->DrawIndexed(_meshData.IndexCount, 0, 0);
}
