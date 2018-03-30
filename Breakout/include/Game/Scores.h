#pragma once
#include <string>
#include <array>

namespace game
{
	class scores
	{
		std::string location;
		bool should_save = false;

		std::array<int, 10> sorted_scores;

	public:
		scores(const std::string& loc);
		~scores();

		void add_score(int score);
		const std::array<int, 10>& get_scores();

		void save();
	};
}