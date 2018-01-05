#include "Lights.h"

Lights::Lights()
{
	// Light direction from surface (XYZ)
	lightVecW = XMFLOAT3(0.25f, 0.5f, -1.0f);

	// Diffuse material properties (RGBA)
	diffuseMtrl = XMFLOAT4(0.65f, 0.35f, 0.15f, 0.5f);
	// Diffuse light colour (RGBA)
	diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);

	// Ambient material properties (RGBA)
	ambientMtrl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	// Ambient light colour (RGBA)
	ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	// Specular light colour (RGBA)
	specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	// Specular material properties
	specularMtrl = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//Intensity of specular light
	specularPower = 0.25f;
}
