#pragma once

#include <DirectXMath.h>
#include <array>

#include "Vertex.h"
#include "IndexedTriangleList.h"

class Plane {
public:

	static IndexedTriangleList GetPlain(int divisions_x = 7, int divisions_y = 7, float width = 1.0f, float height = 1.0f) {
		const int nVertices_x = divisions_x + 1;
		const int nVertices_y = divisions_y + 1;
		std::vector<Vertex> vertices(nVertices_x * nVertices_y);

		// give V a ctor for pos only %%%improvements
		{
			const float side_x = width / 2.0f;
			const float side_y = height / 2.0f;
			const float divisionSize_x = width / float(divisions_x);
			const float divisionSize_y = height / float(divisions_y);
			const DirectX::XMFLOAT3 bottomLeft(-side_x, -side_y, 0.0f);

			for (int y = 0, i = 0; y < nVertices_y; y++) {
				const float y_pos = float(y) * divisionSize_y;
				for (int x = 0; x < nVertices_x; x++, i++) {

					DirectX::XMFLOAT4 pos(bottomLeft.x, bottomLeft.y, bottomLeft.z, 1.0f);
					pos.x += float(x) * divisionSize_x;
					pos.y += y_pos;

					vertices[i].pos = pos;
				}
			}
		}

		std::vector<size_t> indices;
		indices.reserve((divisions_x * divisions_y) * (divisions_x * divisions_y) * 6);
		{
			const auto vxy2i = [nVertices_x](size_t x, size_t y) {
				return y * nVertices_x + x;
			};
			for (size_t y = 0; y < divisions_y; y++) {
				for (size_t x = 0; x < divisions_x; x++) {

					const std::array<size_t, 4> indexArray = { vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x, y + 1), vxy2i(x + 1, y + 1) };
					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}

		return{ std::move(vertices),std::move(indices) };
	}
	
	static IndexedTriangleList GetSkinned(int divisions_x = 7, int divisions_y = 7, float width = 1.0f, float height = 1.0f, float tScale = 1.0f) {

		auto itlist = GetPlain(divisions_x, divisions_y, width, height);
		{
			const int nVertices_x = divisions_x + 1;
			const int nVertices_y = divisions_y + 1;
			const float tDivisionSize_x = width / float(divisions_x);
			const float tDivisionSize_y = height / float(divisions_y);
			const DirectX::XMFLOAT2 tBottomLeft = { 0.0f, 1.0f };

			for (int y = 0, i = 0; y < nVertices_y; y++) {
				const float y_t = (-float(y) * tDivisionSize_y) / tScale;
				for (int x = 0; x < nVertices_x; x++, i++) {

					DirectX::XMFLOAT2 tex = tBottomLeft;
					tex.x += float(x) * tDivisionSize_x;
					tex.y += y_t;

					itlist.vertices[i].t = tex;
				}
			}
		}

		return itlist;
	}
	
	static IndexedTriangleList GetNormals(int divisions_x = 7, int divisions_y = 7, float width = 1.0f, float height = 1.0f) {

		auto itlist = GetPlain(divisions_x, divisions_y, width, height);
		for (auto& v : itlist.vertices) {
			v.n = { 0.0f, 0.0f, -1.0f };
		}

		return itlist;
	}
	
	static IndexedTriangleList GetSkinnedNormals(int divisions_x = 7, int divisions_y = 7, float width = 1.0f, float height = 1.0f, float tScale = 1.0f) {

		auto itlist = GetSkinned(divisions_x, divisions_y, width, height, tScale);
		for (auto& v : itlist.vertices) {
			v.n = { 0.0f, 0.0f, -1.0f };
		}

		return itlist;
	}
};