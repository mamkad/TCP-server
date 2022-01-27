#include "server.hpp"

#define COUNTOFARGS 6

// "0.0.0.0", "8000", "8001", 200, "log.txt"

int main(int argc, char** argv)
{
	if (argc != COUNTOFARGS) {
		cout << "wrong number of argument" << '\n';
		return 0;
	}

	try {
		function<uint16_t(uint16_t)> encrypt_func = [](uint16_t i) {return 2 * (2 * (i + 1) + 1); };
	
		server main_server(argv[1], argv[2], argv[3], std::stoi(argv[4]), argv[5], encrypt_func);

		return main_server.execute();

	} catch(exception const& e) {
		cout << e.what() << '\n';
	}

	return 0;
}
