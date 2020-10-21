#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "Vertex.h"
#include "tiny_obj_loader.h"

class IndexedTriangleList {
public:

	IndexedTriangleList(std::vector<Vertex> verts_in, std::vector<unsigned short> indices_in) : vertices(std::move(verts_in)), indices(std::move(indices_in)) {}

	IndexedTriangleList() = default;


	static IndexedTriangleList Load(const std::string& filename) {
		IndexedTriangleList tl;

		// check first line of file to see if CCW winding comment exists
		bool isCCW = false;
		{
			std::ifstream file(filename);
			std::string firstline;
			std::getline(file, firstline);
			std::transform(firstline.begin(), firstline.end(), firstline.begin(), ::tolower);
			if (firstline.find("ccw") != std::string::npos) {
				isCCW = true;
			}
		}

		// these will be filled by obj loading function
		using namespace tinyobj;
		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::vector<material_t> materials;
		std::string err;

		// load/parse the obj file
		const bool ret = LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());

		// check for errors
		if (!err.empty() && err.substr(0, 4) != "WARN") {
			throw std::runtime_error(("LoadObj returned error:" + err + " File:" + filename).c_str());
		}
		if (!ret) {
			throw std::runtime_error(("LoadObj returned false  File:" + filename).c_str());
		}
		if (shapes.size() == 0u) {
			throw std::runtime_error(("LoadObj object file had no shapes  File:" + filename).c_str());
		}

		// extract vertex data
		// attrib.vertices is a flat std::vector of floats corresponding
		// to vertex positions, laid out as xyzxyzxyz... etc.
		// first preallocate required space in OUR std::vector
		tl.vertices.reserve(attrib.vertices.size() / 3u);
		// iterate over individual vertices, construct Vec3s in OUR vector
		for (int i = 0; i < attrib.vertices.size(); i += 3) {
			tl.vertices.emplace_back(Vertex(DirectX::XMFLOAT4(attrib.vertices[i + 0], attrib.vertices[i + 1], attrib.vertices[i + 2], 1.0f)));
		}

		// extract index data
		// obj file can contain multiple meshes, we assume just 1
		const auto& mesh = shapes[0].mesh;
		// mesh contains a std::vector of num_face_vertices (uchar)
		// and a flat std::vector of indices. If all faces are triangles
		// then for any face f, the first index of that faces is [f * 3n]
		tl.indices.reserve(mesh.indices.size());
		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
			// make sure there are no non-triangle faces
			if (mesh.num_face_vertices[f] != 3u) {
				std::stringstream ss;
				ss << "LoadObj error face #" << f << " has "
					<< mesh.num_face_vertices[f] << " vertices";
				throw std::runtime_error(ss.str().c_str());
			}

			// load set of 3 indices for each face into OUR index std::vector
			for (size_t vn = 0; vn < 3u; vn++) {
				const auto idx = mesh.indices[f * 3u + vn];
				tl.indices.push_back(size_t(idx.vertex_index));
			}

			// reverse winding if file marked as CCW
			if (isCCW) {
				// swapping any two indices reverse the winding dir of triangle
				std::swap(tl.indices.back(), *std::prev(tl.indices.end(), 2));
			}
		}

		return tl;
	}

	static IndexedTriangleList LoadNormals(const std::string& filename) {
		IndexedTriangleList tl;

		// check first line of file to see if CCW winding comment exists
		bool isCCW = false;
		{
			std::ifstream file(filename);
			std::string firstline;
			std::getline(file, firstline);
			std::transform(firstline.begin(), firstline.end(), firstline.begin(), ::tolower);
			if (firstline.find("ccw") != std::string::npos) {
				isCCW = true;
			}
		}

		// these will be filled by obj loading function
		using namespace tinyobj;
		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::vector<material_t> materials;
		std::string err;

		// load/parse the obj file
		const bool ret = LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());

		// check for errors
		if (!err.empty() && err.substr(0, 4) != "WARN") {
			throw std::runtime_error(("LoadObj returned error:" + err + " File:" + filename).c_str());
		}
		if (!ret) {
			throw std::runtime_error(("LoadObj returned false  File:" + filename).c_str());
		}
		if (shapes.size() == 0u) {
			throw std::runtime_error(("LoadObj object file had no shapes  File:" + filename).c_str());
		}

		// extract vertex data
		// attrib.vertices is a flat std::vector of floats corresponding
		// to vertex positions, laid out as xyzxyzxyz... etc.
		// first preallocate required space in OUR std::vector
		tl.vertices.reserve(attrib.vertices.size() / 3u);
		// iterate over individual vertices, construct Vec3s in OUR vector
		for (int i = 0; i < attrib.vertices.size(); i += 3) {
			tl.vertices.emplace_back(
				Vertex(
					DirectX::XMFLOAT4(
						attrib.vertices[i + 0],
						attrib.vertices[i + 1],
						attrib.vertices[i + 2],
						1.0f
					)
				)
			);
		}

		// extract index data
		// obj file can contain multiple meshes, we assume just 1
		const auto& mesh = shapes[0].mesh;
		// mesh contains a std::vector of num_face_vertices (uchar)
		// and a flat std::vector of indices. If all faces are triangles
		// then for any face f, the first index of that faces is [f * 3n]
		tl.indices.reserve(mesh.indices.size());
		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
			// make sure there are no non-triangle faces
			if (mesh.num_face_vertices[f] != 3u) {
				std::stringstream ss;
				ss << "LoadObj error face #" << f << " has " << mesh.num_face_vertices[f] << " vertices";
				throw std::runtime_error(ss.str().c_str());
			}

			// load set of 3 indices for each face into OUR index std::vector
			for (size_t vn = 0; vn < 3u; vn++) {
				const auto idx = mesh.indices[f * 3u + vn];
				tl.indices.push_back(size_t(idx.vertex_index));
				// write normals into the vertices
				tl.vertices[(size_t)idx.vertex_index].n = DirectX::XMFLOAT3(
					attrib.normals[3 * idx.normal_index + 0],
					attrib.normals[3 * idx.normal_index + 1],
					attrib.normals[3 * idx.normal_index + 2]
				);
			}

			// reverse winding if file marked as CCW
			if (isCCW) {
				// swapping any two indices reverse the winding dir of triangle
				std::swap(tl.indices.back(), *std::prev(tl.indices.end(), 2));
			}
		}

		return tl;
	}

	static IndexedTriangleList LoadUVNormals(const std::string& filename) {
		IndexedTriangleList tl;

		// these will be filled by obj loading function
		using namespace tinyobj;
		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::vector<material_t> materials;
		std::string err;

		// load/parse the obj file
		const bool ret = LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());

		const mesh_t& mesh = shapes[0].mesh;
		tl.vertices.reserve(mesh.indices.size());
		tl.indices.reserve(mesh.indices.size());
		size_t ff = 0;

		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {

			for (size_t vn = 0; vn < 3u; vn++) {
				const index_t idx = mesh.indices[f * 3u + vn];
				//tl.indices.push_back(size_t(f * 3u + vn));
				tl.indices.push_back(ff++);

				Vertex v {
					// write pos into the vertices
					DirectX::XMFLOAT4(
						attrib.vertices[3 * idx.vertex_index + 0],
						attrib.vertices[3 * idx.vertex_index + 1],
						attrib.vertices[3 * idx.vertex_index + 2],
						1.0f
					),
					// write normals into the vertices
					DirectX::XMFLOAT3(
						attrib.normals[3 * idx.normal_index + 0],
						attrib.normals[3 * idx.normal_index + 1],
						attrib.normals[3 * idx.normal_index + 2]
					),
					// write uv into the vertices
					DirectX::XMFLOAT2(
						attrib.texcoords[2 * idx.texcoord_index + 0],
						attrib.texcoords[2 * idx.texcoord_index + 1]
					)
				};

				tl.vertices.push_back(v);
			}
		}

		//size_t sz = 0;
		//for(size_t i = 0; i < shapes.size(); i++) {
		//	sz += shapes[i].mesh.indices.size();
		//}

		//tl.vertices.reserve(sz);
		//tl.indices.reserve(sz);
		//size_t ff = 0;
		//for (size_t i = 0; i < shapes.size(); i++) {
		//	const mesh_t& mesh = shapes[i].mesh;
		//	for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {

		//		for (size_t vn = 0; vn < 3u; vn++) {
		//			const index_t idx = mesh.indices[f * 3u + vn];
		//			//tl.indices.push_back(size_t(f * 3u + vn));
		//			tl.indices.push_back(ff++);

		//			Vertex v{
		//				// write pos into the vertices
		//				DirectX::XMFLOAT4(
		//					attrib.vertices[3 * idx.vertex_index + 0],
		//					attrib.vertices[3 * idx.vertex_index + 1],
		//					attrib.vertices[3 * idx.vertex_index + 2],
		//					1.0f
		//				),
		//				// write normals into the vertices
		//				DirectX::XMFLOAT3(
		//					attrib.normals[3 * idx.normal_index + 0],
		//					attrib.normals[3 * idx.normal_index + 1],
		//					attrib.normals[3 * idx.normal_index + 2]
		//				),
		//				// write uv into the vertices
		//				DirectX::XMFLOAT2(
		//					attrib.texcoords[2 * idx.texcoord_index + 0],
		//					attrib.texcoords[2 * idx.texcoord_index + 1]
		//				)
		//			};

		//			tl.vertices.push_back(v);
		//		}
		//	}
		//}

		CalculateTangentsAndNormals(tl.vertices, tl.indices);

		return tl;
	}

	static void CalculateTangentsAndNormals(std::vector<Vertex>& verts, std::vector<unsigned short>& idx) {

		using namespace DirectX;

		// Computing Tangent Space Basis Vectors for an Arbitrary Mesh
		DirectX::XMFLOAT3 zero(0.0f, 0.0f, 0.0f);
		const DirectX::XMVECTOR NullVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		const DirectX::XMVECTOR W_Null = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
		const DirectX::XMVECTOR signature = DirectX::XMVectorSet(1.0f, -1.0f, -1.0f, 1.0f);
		DirectX::XMMATRIX st(NullVector, NullVector, NullVector, NullVector);

		const UINT NumberOfVertices = (UINT)verts.size();
		std::vector<DirectX::XMFLOAT3> vTangents;
		std::vector<DirectX::XMFLOAT3> vBitangents;
		//std::vector<XMVECTOR> vTangents(NumberOfVertices);
		//std::vector<XMVECTOR> vBitangents(NumberOfVertices);

		vTangents.resize(NumberOfVertices, zero);
		vBitangents.resize(NumberOfVertices, zero);

		const UINT NumberOfIndices = (UINT)idx.size();
		for (UINT i = 0; i < NumberOfIndices; ++i) {
			const UINT i0 = idx[i];
			const UINT i1 = idx[++i];
			const UINT i2 = idx[++i];

			const DirectX::XMVECTOR v0 = DirectX::XMLoadFloat4(&verts[i0].pos);
			const DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&verts[i1].pos);
			const DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&verts[i2].pos);

			const DirectX::XMVECTOR e0 = DirectX::XMVectorMultiply(v1 - v0, W_Null);
			const DirectX::XMVECTOR e1 = DirectX::XMVectorMultiply(v2 - v0, W_Null);

			const DirectX::XMMATRIX e01(e0, e1, NullVector, NullVector);
			/*      | e0.x e0.y e0.z 0 |
					| e1.x e1.y e1.z 0 |
			e01 =   | 0    0    0    0 |
					| 0    0    0    0 |
			*/
			const DirectX::XMVECTOR t0 = DirectX::XMLoadFloat2(&verts[i0].t);
			const DirectX::XMVECTOR t1 = DirectX::XMLoadFloat2(&verts[i1].t);
			const DirectX::XMVECTOR t2 = DirectX::XMLoadFloat2(&verts[i2].t);

			DirectX::XMVECTOR s = DirectX::XMVectorMergeXY(t1 - t0, t2 - t0); // s = (t1x - t0x, t2x - t0x, t1y - t0y, t2y - t0y)

			DirectX::XMFLOAT4 d;
			DirectX::XMStoreFloat4(&d, s);

			const float DetInv = 1.0f / (d.x * d.w - d.z * d.y);

			s *= DetInv;
			s = DirectX::XMVectorMultiply(s, signature); // s = (sx, -sy, -sz, sw)

			const UINT* const pSrc = reinterpret_cast<const UINT* const>(&s);
			UINT* pDst = reinterpret_cast<UINT*>(&st.r[0]);
			pDst[0] = pSrc[3];  // _00 = sw
			pDst[1] = pSrc[2];  // _01 = -sz
			pDst = reinterpret_cast<UINT*>(&st.r[1]);
			pDst[0] = pSrc[1];  // _10 = -sy
			pDst[1] = pSrc[0];  // _11 = sz

			/*      | sw  -sz 0 0 |
					| -sy sx  0 0 |
			st =    | 0    0  0 0 |*DetInv
					| 0    0  0 0 |
			*/

			const DirectX::XMMATRIX uv(DirectX::XMMatrixMultiply(st, e01));

			DirectX::XMVECTOR vt0 = DirectX::XMLoadFloat3(&vTangents[i0]);
			DirectX::XMVECTOR vt1 = DirectX::XMLoadFloat3(&vTangents[i1]);
			DirectX::XMVECTOR vt2 = DirectX::XMLoadFloat3(&vTangents[i2]);
			vt0 += uv.r[0];
			vt1 += uv.r[0];
			vt2 += uv.r[0];
			DirectX::XMStoreFloat3(&vTangents[i0], vt0);
			DirectX::XMStoreFloat3(&vTangents[i1], vt1);
			DirectX::XMStoreFloat3(&vTangents[i2], vt1);

			DirectX::XMVECTOR vbt0 = DirectX::XMLoadFloat3(&vBitangents[i0]);
			DirectX::XMVECTOR vbt1 = DirectX::XMLoadFloat3(&vBitangents[i1]);
			DirectX::XMVECTOR vbt2 = DirectX::XMLoadFloat3(&vBitangents[i2]);
			vbt0 += uv.r[1];
			vbt1 += uv.r[1];
			vbt2 += uv.r[1];
			DirectX::XMStoreFloat3(&vBitangents[i0], vbt0);
			DirectX::XMStoreFloat3(&vBitangents[i1], vbt1);
			DirectX::XMStoreFloat3(&vBitangents[i2], vbt1);
		}

		// orthogonalize normals and tangents
		for (UINT i = 0; i < NumberOfVertices; ++i) {
			const DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(XMLoadFloat3(&verts[i].n));
			DirectX::XMVECTOR vt = DirectX::XMLoadFloat3(&vTangents[i]);
			DirectX::XMVECTOR vbt = DirectX::XMLoadFloat3(&vBitangents[i]);

			// normalize tangents and orthogonalize TBN
			DirectX::XMVECTOR n0 = DirectX::XMVector3Normalize(vt - DirectX::XMVector3Dot(normal, vt) * normal);

			//calculate handedness
			const DirectX::XMVECTOR n1 = DirectX::XMVector3Cross(normal, vt);
			const float w = (DirectX::XMVector3Less(DirectX::XMVector3Dot(n1, vbt), NullVector)) ? -1.0f : 1.0f;

			n0 = DirectX::XMVectorSetW(n0, w);
			DirectX::XMStoreFloat3(&verts[i].tg, n0);
		}
	}


	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
};