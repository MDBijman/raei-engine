#include "Imports\Obj.h"

Mesh* Importers::Obj::load(std::string path)
{
	std::ifstream file(path);

	Mesh* m = new Mesh();
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	if (!file.is_open())
		throw new std::runtime_error("file not open");

	std::string line;
	std::string word;

	size_t maxchars = 8192;
	std::vector<char> buf(maxchars);
	while (file.peek() != -1) {
		file.getline(&buf[0], maxchars);

		int offset = 0;
		std::string linebuf(&buf[0]);

		// Vertex position
		if (linebuf[0] == 'v' && linebuf[1] == ' ') {
			offset += 2;
			temp_vertices.push_back(Parser::vec3f(linebuf, offset));
			continue;
		}

		// Vertex normal
		if (linebuf[0] == 'v' && linebuf[1] == 'n') {
			offset += 3;
			temp_normals.push_back(Parser::vec3f(linebuf, offset));
			continue;
		}

		// Vertex texture
		if (linebuf[0] == 'v' && linebuf[1] == 't') {
			offset += 3;
			temp_uvs.push_back(Parser::vec2f(linebuf, offset));
			continue;
		}

		// Face
		if (linebuf[0] == 'f') {
			offset += 2;
			int values[9];

			for (int i = 0; i < 9; i++)
				values[i] = Parser::parseInt(linebuf, offset) - 1;

			m->vertices.push_back(Vertex(temp_vertices.at(values[0]), temp_uvs.at(values[1]), temp_normals.at(values[2])));
			m->vertices.push_back(Vertex(temp_vertices.at(values[3]), temp_uvs.at(values[4]), temp_normals.at(values[5])));
			m->vertices.push_back(Vertex(temp_vertices.at(values[6]), temp_uvs.at(values[7]), temp_normals.at(values[8])));

			// TODO per vertex normals
			m->indices.push_back(m->vertices.size() - 3);
			m->indices.push_back(m->vertices.size() - 2);
			m->indices.push_back(m->vertices.size() - 1);
			continue;
		}
	}

	return m;
}