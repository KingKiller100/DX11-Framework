#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include "OBJLoader.h"

using namespace DirectX;


struct Material
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	float specularPower;

	Material(const XMFLOAT4 &diffuse, const XMFLOAT4 &ambient, const XMFLOAT4 &specular, const float &power) : diffuse(diffuse), ambient(ambient), specular(specular), specularPower(power)
	{/*Empty*/}
};

class Model
{
public:
	Model(const MeshData &mesh, const Material &material);
	~Model();

	void Update(const float &deltaTime);
	void Draw(ID3D11DeviceContext* _deviceContext, ID3D11BlendState* _blendState, const bool& transparent) const;
	
	Material GetMaterial() const									{ return material; }

	void SetTextureRV(ID3D11ShaderResourceView * tRV)				{ _textureRV = tRV; }
	ID3D11ShaderResourceView *GetTextureRV() const					{ return _textureRV; }
	bool HasTexture() const											{ return _textureRV; }

	void SetMatrix(const XMMATRIX &_matrix)							{ matrix = _matrix; }
	XMMATRIX GetMatrix() const										{ return matrix; }

private:
	Material material;
	MeshData meshData;
	XMMATRIX matrix;

	ID3D11ShaderResourceView* _textureRV{};
};