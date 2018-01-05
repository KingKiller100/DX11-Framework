#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Lights
{
public:
	Lights();
	~Lights();

	//Light Vector + padding
	XMFLOAT3 lightVecW;
	float padding;

	//Diffuse
	XMFLOAT4 diffuseMtrl;
	XMFLOAT4 diffuseLight;
	
	//Ambient
	XMFLOAT4 ambientMtrl;
	XMFLOAT4 ambientLight;

	//
	//Specular
	//
	XMFLOAT4 specularMtrl;
	XMFLOAT4 specularLight;
	float specularPower;
};