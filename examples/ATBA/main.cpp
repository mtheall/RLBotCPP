#include "ATBA.h"

#include <rlbot/BotManager.h>

#include <cstdio>
#include <cstdlib>

int main (int argc_, char *argv_[])
{
	auto const agentId = std::getenv ("RLBOT_AGENT_ID");
	if (!agentId || std::strlen (agentId) == 0)
	{
		std::fprintf (stderr, "Missing environment variable RLBOT_AGENT_ID\n");
		return EXIT_FAILURE;
	}

	auto const serverPort = [] () -> char const * {
		auto const env = std::getenv ("RLBOT_SERVER_PORT");
		if (env)
			return env;
		return "23234";
	}();

	auto const host = argc_ > 1 ? argv_[1] : "127.0.0.1";
	auto const port = argc_ > 2 ? argv_[2] : serverPort;

	rlbot::BotManager<ATBA> manager{true};
	if (!manager.run (host, port, agentId, true))
	{
		std::fprintf (stderr, "Usage: %s [addr] [port]\n", argv_[0]);
		return EXIT_FAILURE;
	}
}