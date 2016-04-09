#pragma once
#include <vector>
#include <memory>
#include <Triangle.h>

class Mesh
{
public:
	std::vector<Triangle> triangles;
};

class GraphicsAssetModule
{
public:
	GraphicsAssetModule()
	{
		Vertex v1(glm::vec3(1.0f, 1.0f, 0.0f));// },{ 1.0f, 0.0f, 0.0f } },
		Vertex v2(glm::vec3(-1.0f, 1.0f, 0.0f));//{ 0.0f, 1.0f, 0.0f } },
		Vertex v3(glm::vec3(0.0f, -1.0f, 0.0f));//{ 0.0f, 0.0f, 1.0f } },

		Mesh* m = new Mesh();
		m->triangles.push_back(Triangle(v1, v2, v3));
		addMesh(m);
	}

	void addMesh(Mesh* m)
	{
		meshes.push_back(std::unique_ptr<Mesh>(m));
	}

private:
	std::vector<std::unique_ptr<Mesh>> meshes;

};