#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <gli/gli.hpp>

namespace fnt
{
	struct character
	{
		character(int id, int x, int y, int width, int height, int xoffset, int yoffset, int xadvance, int page, int chnl)
			: id(id), x(x), y(y), width(width), height(height), xoffset(xoffset), yoffset(yoffset), xadvance(xadvance),
			page(page), chnl(chnl) {}

		int id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl;
	};

	class file
	{
		int char_count;
		std::vector<character> characters;

	public:
		file(std::string filename)
		{
			std::ifstream file(filename);

			// info
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			// common
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			// page
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			// chars
			file.ignore(std::numeric_limits<std::streamsize>::max(), '=');
			file >> char_count;
			characters.reserve(char_count);
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			// character information
			for (int line_it = 0; line_it < char_count; line_it++)
			{
				int id = read_int_after_eq(file);
				int x = read_int_after_eq(file);
				int y = read_int_after_eq(file);
				int width = read_int_after_eq(file);
				int height = read_int_after_eq(file);
				int xoffset = read_int_after_eq(file);
				int yoffset = read_int_after_eq(file);
				int xadvance = read_int_after_eq(file);
				int page = read_int_after_eq(file);
				int chnl = read_int_after_eq(file);
				characters.push_back(character(id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl));

				file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << std::endl;
			}
		}

		character& get_character(char c)
		{
			int id = static_cast<int>(c);
			auto pos = std::find_if(characters.begin(), characters.end(), [id](auto& character) {
				return character.id == id;
			});

			if (pos == characters.end()) throw std::runtime_error("unknown");

			return *pos;
		}


	private:
		int read_int_after_eq(std::ifstream& stream)
		{
			stream.ignore(std::numeric_limits<std::streamsize>::max(), '=');

			int char_id;
			stream >> char_id;
			return char_id;
		}
	};
}