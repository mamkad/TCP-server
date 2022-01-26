#include "server.hpp"

int main()
{
	function<uint16_t(uint16_t)> encrypt_func = [](uint16_t i) {return 2 * (2 * (i + 1) + 1); };
	
	server main_server("0.0.0.0", "8000", "8001", 200, "log.txt", encrypt_func);
	return main_server.execute();
}