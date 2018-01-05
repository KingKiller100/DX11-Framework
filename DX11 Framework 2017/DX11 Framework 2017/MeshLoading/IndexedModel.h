#pragma once
#include <vector>


class IndexedModel
{
public:
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texCoords;
		std::vector<unsigned int> indices;

		void CalculateNormals();
};
