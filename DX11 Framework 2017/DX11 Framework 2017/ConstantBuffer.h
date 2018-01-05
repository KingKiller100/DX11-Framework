#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	//Diffuse Lighting
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;

	XMFLOAT3 lightDirection;
	float padding;

	//Ambient Lighting
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;

	//Specular Lighting
	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 eyePosW; 	// Camera position in world space
};

