#pragma once
#include <d3d11_1.h>
#include <string>
class ImageLoader
{
public:
	static ID3D11ShaderResourceView* GetImage(std::string fileLocation);
};

