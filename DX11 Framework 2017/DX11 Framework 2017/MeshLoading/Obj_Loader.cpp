#include "Obj_Loader.h"
#include <fstream>
#include<iostream>
#include <Math.h>
#include <map>
#include <algorithm>

/*
	Obj Loader code taken and rewritten from Benny Box Mordern OpenGL obj_loader class
	Link: https://github.com/BennyQBD/ModernOpenGLTutorial
*/

using namespace std;

namespace math
{
	// XMFLOAT3 Cross Product
	XMFLOAT3 CrossV3(const XMFLOAT3 a, const XMFLOAT3 b)
	{
		return XMFLOAT3(a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
	}

	// XMFLOAT3 Magnitude Calculation
	float MagnitudeV3(const XMFLOAT3 in)
	{
		return (sqrtf(powf(in.x, 2) + powf(in.y, 2) + powf(in.z, 2)));
	}

	// XMFLOAT3 Normailization Calculations
	XMFLOAT3 Normalize(const XMFLOAT3 n)
	{
		float xN = n.x * n.x;
		float yN = n.y * n.y;
		float zN = n.z * n.z;

		float unitVector = sqrt(xN + yN + zN);

		return XMFLOAT3(xN / unitVector, yN / unitVector, zN / unitVector);
	}

	// XMFLOAT3 DotProduct
	float DotV3(const XMFLOAT3 a, const XMFLOAT3 b)
	{
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	// Angle between 2 XMFLOAT3 Objects
	float AngleBetweenV3(const XMFLOAT3 a, const XMFLOAT3 b)
	{
		float angle = DotV3(a, b);
		angle /= (MagnitudeV3(a) * MagnitudeV3(b));
		return angle = acosf(angle);
	}
}


static bool CompareIndexPtr(const Index* a, const Index* b);
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline unsigned int ParseIndexValue(const std::string& token, unsigned int start, unsigned int end);
static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> SplitString(const std::string &s, char delim);

OBJModel::OBJModel(const std::string& fileName)
{
	hasUVs = false;
	hasNormals = false;
	std::ifstream file;
	file.open(fileName.c_str());

	std::string line;
	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);

			unsigned int lineLength = line.length();

			if (lineLength < 2)
				continue;

			const char* lineCStr = line.c_str();

			switch (lineCStr[0])
			{
			case 'v':
				if (lineCStr[1] == 't')
					this->uvs.push_back(ParseOBJVec2(line));
				else if (lineCStr[1] == 'n')
					this->normals.push_back(ParseOBJVec3(line));
				else if (lineCStr[1] == ' ' || lineCStr[1] == '\t')
					this->positions.push_back(ParseOBJVec3(line));
				break;
			case 'f':
				CreateOBJFace(line);
				break;
			default: break;
			};
		}
	}
	else
	{
		std::cerr << "Unable to load mesh: " << fileName << std::endl;
	}
}

void IndexedModel::CalculateNormals()
{
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];

		XMFLOAT3 v1 = positions[i1] - positions[i0];
		XMFLOAT3 v2 = positions[i2] - positions[i0];

		XMFLOAT3 normal = math::Normalize(math::CrossV3(v1, v2));

		normals[i0] = normals[i0] + normal;
		normals[i1] = normals[i1] + normal;
		normals[i2] = normals[i2] + normal;
	}

	for (unsigned int i = 0; i < positions.size(); i++)
		normals[i] = math::Normalize(normals[i]);
}

IndexedModel OBJModel::ToIndexedModel()
{
	IndexedModel result;
	IndexedModel normalModel;

	unsigned int numIndices = OBJIndices.size();

	std::vector<Index*> indexLookup;

	for (unsigned int i = 0; i < numIndices; i++)
		indexLookup.push_back(&OBJIndices[i]);

	std::sort(indexLookup.begin(), indexLookup.end(), CompareIndexPtr);
	
	std::map<Index, unsigned int> normalModelIndexMap;
	std::map<unsigned int, unsigned int> indexMap;

	for (unsigned int i = 0; i < numIndices; i++)
	{
		Index* currentIndex = &OBJIndices[i];

		XMFLOAT3 currentPosition = positions[currentIndex->vertexIndex];
		XMFLOAT2 currentTexCoord;
		XMFLOAT3 currentNormal;

		if (hasUVs)
			currentTexCoord = uvs[currentIndex->uvIndex];
		else
			currentTexCoord = XMFLOAT2(0, 0);

		if (hasNormals)
			currentNormal = normals[currentIndex->normalIndex];
		else
			currentNormal = XMFLOAT3(0, 0, 0);

		unsigned int normalModelIndex;
		unsigned int resultModelIndex;

		//Create model to properly generate normals on
		std::map<Index, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
		if (it == normalModelIndexMap.end())
		{
			normalModelIndex = normalModel.positions.size();

			normalModelIndexMap.insert(std::pair<Index, unsigned int>(*currentIndex, normalModelIndex));
			normalModel.positions.push_back(currentPosition);
			normalModel.texCoords.push_back(currentTexCoord);
			normalModel.normals.push_back(currentNormal);
		}
		else
			normalModelIndex = it->second;

		//Create model which properly separates texture coordinates
		unsigned int previousVertexLocation = FindLastVertexIndex(indexLookup, currentIndex, result);

		if (previousVertexLocation == (unsigned int)-1)
		{
			resultModelIndex = result.positions.size();

			result.positions.push_back(currentPosition);
			result.texCoords.push_back(currentTexCoord);
			result.normals.push_back(currentNormal);
		}
		else
			resultModelIndex = previousVertexLocation;

		normalModel.indices.push_back(normalModelIndex);
		result.indices.push_back(resultModelIndex);
		indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
	}

	if (!hasNormals)
	{
		normalModel.CalculateNormals();

		for (unsigned int i = 0; i < result.positions.size(); i++)
			result.normals[i] = normalModel.normals[indexMap[i]];
	}

	return result;
};

unsigned int OBJModel::FindLastVertexIndex(const std::vector<Index*>& indexLookup, const Index* currentIndex, const IndexedModel& result)
{
	unsigned int start = 0;
	unsigned int end = indexLookup.size();
	unsigned int current = (end - start) / 2 + start;
	unsigned int previous = start;

	while (current != previous)
	{
		Index* testIndex = indexLookup[current];

		if (testIndex->vertexIndex == currentIndex->vertexIndex)
		{
			unsigned int countStart = current;

			for (unsigned int i = 0; i < current; i++)
			{
				Index* possibleIndex = indexLookup[current - i];

				if (possibleIndex == currentIndex)
					continue;

				if (possibleIndex->vertexIndex != currentIndex->vertexIndex)
					break;

				countStart--;
			}

			for (unsigned int i = countStart; i < indexLookup.size() - countStart; i++)
			{
				Index* possibleIndex = indexLookup[current + i];

				if (possibleIndex == currentIndex)
					continue;

				if (possibleIndex->vertexIndex != currentIndex->vertexIndex)
					break;
				else if ((!hasUVs || possibleIndex->uvIndex == currentIndex->uvIndex)
					&& (!hasNormals || possibleIndex->normalIndex == currentIndex->normalIndex))
				{
					XMFLOAT3 currentPosition = positions[currentIndex->vertexIndex];
					XMFLOAT2 currentTexCoord;
					XMFLOAT3 currentNormal;

					if (hasUVs)
						currentTexCoord = uvs[currentIndex->uvIndex];
					else
						currentTexCoord.x = 0.0f;
						currentTexCoord.y = 0.0f;

					if (hasNormals)
						currentNormal = normals[currentIndex->normalIndex];
					else
						currentNormal = XMFLOAT3(0, 0, 0);

					for (unsigned int j = 0; j < result.positions.size(); j++)
					{
						if (currentPosition == result.positions[j]
							&& ((!hasUVs || currentTexCoord == result.texCoords[j])
								&& (!hasNormals || currentNormal == result.normals[j])))
						{
							return j;
						}
					}
				}
			}

			return -1;
		}
		else
		{
			if (testIndex->vertexIndex < currentIndex->vertexIndex)
				start = current;
			else
				end = current;
		}

		previous = current;
		current = (end - start) / 2 + start;
	}

	return -1;
}

void OBJModel::CreateOBJFace(const std::string& line)
{
	std::vector<std::string> tokens = SplitString(line, ' ');

	this->OBJIndices.push_back(ParseIndex(tokens[1], &this->hasUVs, &this->hasNormals));
	this->OBJIndices.push_back(ParseIndex(tokens[2], &this->hasUVs, &this->hasNormals));
	this->OBJIndices.push_back(ParseIndex(tokens[3], &this->hasUVs, &this->hasNormals));

	
	if ((int)tokens.size() > 4)
	{
		this->OBJIndices.push_back(ParseIndex(tokens[1], &this->hasUVs, &this->hasNormals));
		this->OBJIndices.push_back(ParseIndex(tokens[3], &this->hasUVs, &this->hasNormals));
		this->OBJIndices.push_back(ParseIndex(tokens[4], &this->hasUVs, &this->hasNormals));
	}
}

Index OBJModel::ParseIndex(const std::string& token, bool* hasUVs, bool* hasNormals)
{
	unsigned int tokenLength = token.length();
	const char* tokenString = token.c_str();

	unsigned int vertIndexStart = 0;
	unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');

	Index result;
	result.vertexIndex = ParseIndexValue(token, vertIndexStart, vertIndexEnd);
	result.uvIndex = 0;
	result.normalIndex = 0;

	if (vertIndexEnd >= tokenLength)
		return result;

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');

	result.uvIndex = ParseIndexValue(token, vertIndexStart, vertIndexEnd);
	*hasUVs = true;

	if (vertIndexEnd >= tokenLength)
		return result;

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');

	result.normalIndex = ParseIndexValue(token, vertIndexStart, vertIndexEnd);
	*hasNormals = true;

	return result;
}

XMFLOAT3 OBJModel::ParseOBJVec3(const std::string& line)
{
	unsigned int tokenLength = line.length();
	const char* tokenString = line.c_str();

	unsigned int vertIndexStart = 2;

	while (vertIndexStart < tokenLength)
	{
		if (tokenString[vertIndexStart] != ' ')
			break;
		vertIndexStart++;
	}

	unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

	return XMFLOAT3(x, y, z);

	//XMFLOAT3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()))
}

XMFLOAT2 OBJModel::ParseOBJVec2(const std::string& line)
{
	unsigned int tokenLength = line.length();
	const char* tokenString = line.c_str();

	unsigned int vertIndexStart = 3;

	while (vertIndexStart < tokenLength)
	{
		if (tokenString[vertIndexStart] != ' ')
			break;
		vertIndexStart++;
	}

	unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

	return XMFLOAT2(x, y);
}

static bool CompareIndexPtr(const Index* a, const Index* b)
{
	return a->vertexIndex < b->vertexIndex;
}

static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
	unsigned int result = start;
	while (result < length)
	{
		result++;
		if (str[result] == token)
			break;
	}

	return result;
}

static inline unsigned int ParseIndexValue(const std::string& token, unsigned int start, unsigned int end)
{
	return atoi(token.substr(start, end - start).c_str()) - 1;
}

static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
	return atof(token.substr(start, end - start).c_str());
}

static inline std::vector<std::string> SplitString(const std::string &s, char delim)
{
	std::vector<std::string> elems;

	const char* cstr = s.c_str();
	unsigned int strLength = s.length();
	unsigned int start = 0;
	unsigned int end = 0;

	while (end <= strLength)
	{
		while (end <= strLength)
		{
			if (cstr[end] == delim)
				break;
			end++;
		}

		elems.push_back(s.substr(start, end - start));
		start = end + 1;
		end = start;
	}

	return elems;
}