#include "Modules/IO/Logging.h"
#include <iostream>

namespace Logging
{
	void log(const std::string& message)
	{
		std::cout << message << "\n";
	}

	void flush()
	{
		std::cout.flush();
	}
}