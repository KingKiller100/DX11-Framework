#pragma once
#include <vector>
#include <DirectXMath.h>
#include "OBJIndex.h"
using namespace DirectX;

class OBJModel
{
public:
	std::vector<Index> OBJIndices;
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMFLOAT3> normals;
	bool hasUVs;
	bool hasNormals;

	OBJModel(const std::string& fileName);

	IndexedModel ToIndexedModel();
private:
	unsigned int FindLastVertexIndex(const std::vector<Index*>& indexLookup, const Index* currentIndex, const IndexedModel& result);
	void CreateOBJFace(const std::string& line);

	XMFLOAT2 ParseOBJVec2(const std::string& line);
	XMFLOAT3 ParseOBJVec3(const std::string& line);
	Index ParseIndex(const std::string& token, bool* hasUVs, bool* hasNormals);
};
