#include "ImageLoader.h"
#include <WICTextureLoader.h>
using namespace DirectX;
ID3D11ShaderResourceView* ImageLoader::GetImage(std::string fileLocation)
{
	std::wstring path = std::wstring(fileLocation.begin(), fileLocation.end());
	ID3D11ShaderResourceView* ptr(nullptr);
	CreateWICTextureFromFile(nullptr, path.c_str(), nullptr, &ptr);
	return ptr;
}
