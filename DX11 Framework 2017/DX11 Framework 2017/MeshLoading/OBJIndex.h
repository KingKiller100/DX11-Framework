#pragma once

struct Index
{
	unsigned int vertexIndex, uvIndex, normalIndex;

	bool operator<(const Index& p) const { return vertexIndex < p.vertexIndex; }
};