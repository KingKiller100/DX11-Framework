#include "Model.h"
#include "DDSTextureLoader.h"

Model::Model(const MeshData &meshData, const Material & material) : material(material), meshData(meshData)
{
}

Model::~Model()
{
	// if (_textureRV) _textureRV->Release();
}

void Model::Update(const float& deltaTime)
{
}


void Model::Draw(ID3D11DeviceContext* _deviceContext, ID3D11BlendState* _blendState, const bool& transparent) const
{
	// "fine-tune" the blending equation
	const float blendFactor[4] = { 0.95f, 0.95f, 0.995f, 1.0f };

	transparent ?
		// Set the blend state for transparent objects
		_deviceContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff) :	
		// Set the blend state for transparent objects
		_deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	
	_deviceContext->IASetVertexBuffers(0, 1, &meshData.VertexBuffer, &meshData.VBStride, &meshData.VBOffset);
	_deviceContext->IASetIndexBuffer(meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);	

	// Renders
	_deviceContext->DrawIndexed(meshData.IndexCount, 0, 0);
}
