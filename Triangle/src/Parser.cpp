#include "Parser.h"

#include <sstream>

glm::vec3 Importers::Parser::vec3f(std::string line, int& offset) {
	glm::vec3 ret;
	ret.x = parseFloat(line, offset);
	ret.y = parseFloat(line, offset);
	ret.z = parseFloat(line, offset);
	return ret;
}

glm::vec2 Importers::Parser::vec2f(std::string line, int& offset) {
	glm::vec2 ret;
	std::string word;
	ret.x = parseFloat(line, offset);
	ret.y = parseFloat(line, offset);
	return ret;
}

int Importers::Parser::parseInt(std::string line, int& offset) {
	int part = line.find_first_of(" /", offset);

	int i = stoi(line.substr(offset, part));
	offset = part + 1;

	return i;
}

float Importers::Parser::parseFloat(std::string line, int & offset)
{
	int part = line.find_first_of(" /", offset);
	float f = stof(line.substr(offset, part));
	offset = part + 1;
	return f;
}