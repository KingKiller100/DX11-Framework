#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Lights.h"
#include "Camera.h"
#include "OBJLoader.h"

using namespace DirectX;

class Model
{
public:
	Model(MeshData mesh, wchar_t* texturePath, XMFLOAT4X4 worldMatrix, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11SamplerState* samplerLinear);
	~Model();

	void ObjectAnimation(float t);
	void Draw(ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader, ID3D11Buffer* _pConstantBuffer, Camera* _camera, ID3D11BlendState* _transparency, const
	          bool& transparency) const;
private:
	MeshData _meshData;
	XMFLOAT4X4 _worldMatrix;
	Lights* _lights;
	bool _transparent;

	ID3D11SamplerState * _pSamplerLinear;
	ID3D11DeviceContext* _deviceContext;
	ID3D11Device* _device;
	ID3D11ShaderResourceView* _pTextureRV{};
};