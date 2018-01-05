#pragma once
#ifndef OBJ_LOADER_H_INCLUDED
#define OBJ_LOADER_H_INCLUDED

#include <DirectXMath.h>
#include <vector>
#include <string>
#include "IndexedModel.h"
#include "OBJModel.h"
#include "OBJIndex.h"

class Obj_Loader
{
public:
	bool LoadOBJ(const char * path,
		std::vector < XMFLOAT3 > & out_vertices,
		std::vector < XMFLOAT2 > & out_uvs,
		std::vector < XMFLOAT3 > & out_normals);
};

#endif