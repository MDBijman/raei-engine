#include "stdafx.h"
#include "Game/Scores.h"

#include <fstream>

namespace game
{
	scores::scores(const std::string& loc) : location(loc)
	{
		std::ifstream file(location);

		for (int i = 0; i < 10; i++) sorted_scores[i] = 0;

		int score;
		while (file >> score)
			add_score(score);
	}

	scores::~scores()
	{
		if (should_save)
			save();
	}

	void scores::add_score(int score)
	{
		should_save = true;

		for (int i = 0; i < 10; i++)
		{
			if (score > sorted_scores[i])
			{
				int prev = sorted_scores[i];
				for (int j = 9; j > i; j--)
				{
					sorted_scores[j] = sorted_scores[j - 1];
				}

				sorted_scores[i] = score;
				break;
			}
		}
	}

	const std::array<int, 10>& scores::get_scores()
	{
		return this->sorted_scores;
	}

	void scores::save()
	{
		std::ofstream file(location, std::ofstream::trunc);
		for (auto score : sorted_scores) file << score << "\n";
	}
}