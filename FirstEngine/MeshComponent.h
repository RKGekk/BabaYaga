#pragma once

#include <DirectXMath.h>

#include "ActorComponent.h"
#include "tiny_obj_loader.h"
#include "Vertex.h"

class MeshComponent : public ActorComponent {

private:
	tinyobj::attrib_t m_attrib;
	std::vector<tinyobj::shape_t> m_shapes;
	std::vector<tinyobj::material_t> m_materials;
	std::string m_objResource;

	static void CalculateTangentsAndNormals(std::vector<Vertex>& verts, std::vector<unsigned short>& idx);

public:
	static const std::string g_Name;

	struct IndexedTriangleList {
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;
	};

	MeshComponent();

	int ShapesCount() {
		return m_shapes.size();
	}

	IndexedTriangleList GetTriangleList(int shapeid = 0);
	std::vector<tinyobj::material_t>& GetMaterials();

	virtual bool VInit(TiXmlElement* pData) override;
	virtual const std::string& VGetName() const override;
};