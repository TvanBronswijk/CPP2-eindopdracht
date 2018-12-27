#include "server.hpp"

int main(int argc, const char * argv[])
{
	return Server("machiavelli", "machiavelli> ", 1080).run();
}

