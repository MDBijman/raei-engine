#include "Modules\Importers\Obj.h"

#include <sstream>

namespace Importers
{
	namespace Obj
	{
		Graphics::Data::Mesh* load(const std::string& path)
		{
			std::ifstream file(path);

			Graphics::Data::Mesh* m = new Graphics::Data::Mesh();
			std::vector<glm::vec3> temp_vertices = { glm::vec3(0, 0, 0) };
			std::vector<glm::vec2> temp_uvs = { glm::vec2(0, 0) };
			std::vector<glm::vec3> temp_normals = { glm::vec3(0, 0, 0) };

			if (!file.is_open()) throw new std::runtime_error("file not open");

			std::stringstream fileStream;
			fileStream << file.rdbuf();

			std::string line;
			while (std::getline(fileStream, line)) {

				// Vertex position
				if (line[0] == 'v' && line[1] == ' ') {
					float values[3];

					std::stringstream lineWithoutIdentifier(line.substr(2));
					std::string vertex;
					int index = 0;
					while (std::getline(lineWithoutIdentifier, vertex, ' '))
					{
						values[index] = stof(vertex);
						index++;
					}

					temp_vertices.push_back(glm::vec3(values[0], values[1], values[2]));
					continue;
				}

				// Vertex normal
				if (line[0] == 'v' && line[1] == 'n') {
					float values[3];

					std::stringstream lineWithoutIdentifier(line.substr(3));
					std::string normal;
					int index = 0;
					while (std::getline(lineWithoutIdentifier, normal, ' '))
					{
						values[index] = stof(normal);
						index++;
					}

					temp_normals.push_back(glm::vec3(values[0], values[1], values[2]));
					continue;
				}

				// Vertex texture
				if (line[0] == 'v' && line[1] == 't') {
					float values[2];

					std::stringstream lineWithoutIdentifier(line.substr(3));
					std::string uv;
					int index = 0;
					while (std::getline(lineWithoutIdentifier, uv, ' '))
					{
						values[index] = stof(uv);
						index++;
					}

					temp_uvs.push_back(glm::vec2(values[0], values[1]));
					continue;
				}

				// Face
				if (line[0] == 'f') {
					int values[9];

					std::stringstream lineWithoutIdentifier(line.substr(2));

					int index = 0;
					std::string vertex;
					while (std::getline(lineWithoutIdentifier, vertex, ' '))
					{
						std::stringstream segment(vertex);
						std::string attribute;
						while (std::getline(segment, attribute, '/'))
						{
							if (attribute.length() > 0)
								values[index] = stoi(attribute);
							else
								values[index] = 0;
							index++;
						}
					}

					m->vertices.getData().push_back(Vertex(temp_vertices.at(values[0]), temp_uvs.at(values[1]), temp_normals.at(values[2])));
					m->vertices.getData().push_back(Vertex(temp_vertices.at(values[3]), temp_uvs.at(values[4]), temp_normals.at(values[5])));
					m->vertices.getData().push_back(Vertex(temp_vertices.at(values[6]), temp_uvs.at(values[7]), temp_normals.at(values[8])));

					// TODO per vertex normals
					m->indices.getData().push_back(m->vertices.getData().size() - 3);
					m->indices.getData().push_back(m->vertices.getData().size() - 2);
					m->indices.getData().push_back(m->vertices.getData().size() - 1);
					continue;
				}
			}

			return m;
		}
	}
}
