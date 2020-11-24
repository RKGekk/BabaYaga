#include "MeshComponent.h"

const std::string MeshComponent::g_Name = "MeshComponent";

MeshComponent::MeshComponent() {}

void MeshComponent::CalculateTangentsAndNormals(std::vector<Vertex>& verts, std::vector<unsigned short>& idx) {

	using namespace DirectX;

	// Computing Tangent Space Basis Vectors for an Arbitrary Mesh
	DirectX::XMFLOAT3 zero(0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR NullVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR W_Null = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR signature = DirectX::XMVectorSet(1.0f, -1.0f, -1.0f, 1.0f);
	DirectX::XMMATRIX st(NullVector, NullVector, NullVector, NullVector);

	const unsigned int NumberOfVertices = (unsigned int)verts.size();
	std::vector<DirectX::XMFLOAT3> vTangents;
	std::vector<DirectX::XMFLOAT3> vBitangents;
	//std::vector<XMVECTOR> vTangents(NumberOfVertices);
	//std::vector<XMVECTOR> vBitangents(NumberOfVertices);

	vTangents.resize(NumberOfVertices, zero);
	vBitangents.resize(NumberOfVertices, zero);

	const unsigned int NumberOfIndices = (unsigned int)idx.size();
	for (unsigned int i = 0; i < NumberOfIndices; ++i) {
		const unsigned int i0 = idx[i];
		const unsigned int i1 = idx[++i];
		const unsigned int i2 = idx[++i];

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

		const unsigned int* const pSrc = reinterpret_cast<const unsigned int* const>(&s);
		unsigned int* pDst = reinterpret_cast<unsigned int*>(&st.r[0]);
		pDst[0] = pSrc[3];  // _00 = sw
		pDst[1] = pSrc[2];  // _01 = -sz
		pDst = reinterpret_cast<unsigned int*>(&st.r[1]);
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
	for (unsigned int i = 0; i < NumberOfVertices; ++i) {
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

MeshComponent::IndexedTriangleList MeshComponent::GetTriangleList(int shapeid) {
	IndexedTriangleList tl;

	// these will be filled by obj loading function
	using namespace tinyobj;

	const mesh_t& mesh = m_shapes[shapeid].mesh;
	tl.vertices.reserve(mesh.indices.size());
	tl.indices.reserve(mesh.indices.size());
	size_t ff = 0;

	for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {

		for (size_t vn = 0; vn < 3u; vn++) {
			const index_t idx = mesh.indices[f * 3u + vn];
			//tl.indices.push_back(size_t(f * 3u + vn));
			tl.indices.push_back(ff++);

			Vertex v{
				// write pos into the vertices
				DirectX::XMFLOAT4(
					m_attrib.vertices[3 * idx.vertex_index + 0],
					m_attrib.vertices[3 * idx.vertex_index + 1],
					m_attrib.vertices[3 * idx.vertex_index + 2],
					1.0f
				),
				// write normals into the vertices
				DirectX::XMFLOAT3(
					m_attrib.normals[3 * idx.normal_index + 0],
					m_attrib.normals[3 * idx.normal_index + 1],
					m_attrib.normals[3 * idx.normal_index + 2]
				),
				// write uv into the vertices
				DirectX::XMFLOAT2(
					m_attrib.texcoords[2 * idx.texcoord_index + 0],
					m_attrib.texcoords[2 * idx.texcoord_index + 1]
				)
			};

			tl.vertices.push_back(v);
		}
	}

	CalculateTangentsAndNormals(tl.vertices, tl.indices);

	return tl;
}

bool MeshComponent::VInit(TiXmlElement* pData) {

	TiXmlElement* pObjElement = pData->FirstChildElement("Obj");
	std::string fileName(pObjElement->GetText());
	if (fileName.empty()) {
		return false;
	}
	m_objResource = fileName;
	if(fileName == "NoObj") {
		return true;
	}
	std::string err;
	return LoadObj(&m_attrib, &m_shapes, &m_materials, &err, fileName.c_str());
}

const std::string& MeshComponent::VGetName() const {
	return g_Name;
}