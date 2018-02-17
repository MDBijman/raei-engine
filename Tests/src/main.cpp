#define CATCH_CONFIG_RUNNER
#define CATCH_CPP14_OR_GREATER
#include <catch/catch.hpp>

int main(int argc, char* argv[]) {

	int result = Catch::Session().run(argc, argv);

	std::cin.get();
	return result;
}