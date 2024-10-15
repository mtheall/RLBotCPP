#include <rlbot/BotManager.h>

#include <rlbot/Bot.h>

#include "BotManagerImpl.h"
#include "Log.h"
#include "SockAddr.h"
#include "Socket.h"

#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <poll.h>
#endif

#include <atomic>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <tuple>
#include <unordered_map>
using namespace std::chrono_literals;

using namespace rlbot;
using namespace rlbot::detail;

namespace
{
std::unique_ptr<Bot> spawnNothing (int, int, std::string) noexcept
{
	return {};
}
}

///////////////////////////////////////////////////////////////////////////
BotManagerBase::~BotManagerBase () noexcept
{
	m_impl->join ();
}

BotManagerBase::BotManagerBase (
    std::unique_ptr<Bot> (&spawn_) (int, int, std::string) noexcept) noexcept
    : m_impl (std::make_unique<detail::BotManagerImpl> (spawn_))
{
}

bool BotManagerBase::run (char const *const host_,
    char const *const port_,
    char const *const agentId_) noexcept
{
	auto const agentId = std::getenv ("RLBOT_AGENT_ID");
	if (!agentId || std::strlen (agentId) == 0)
	{
		error ("No agent id provided\n");
		return false;
	}

	if (!m_impl->run (host_, port_))
		return false;

	rlbot::flat::ConnectionSettingsT cs{
	    //
	    .agent_id               = agentId_,
	    .wants_ball_predictions = true,
	    .wants_comms            = true,
	    .close_after_match      = true,
	    //
	};

	m_impl->enqueueMessage (cs);

	return true;
}

void rlbot::BotManagerBase::terminate () noexcept
{
	m_impl->terminate ();
}

void rlbot::BotManagerBase::startMatch (rlbot::flat::MatchSettingsT const &matchSettings_) noexcept
{
	m_impl->enqueueMessage (matchSettings_);
}

bool rlbot::BotManagerBase::startMatch (char const *const host_,
    char const *const port_,
    rlbot::flat::MatchSettingsT const &matchSettings_) noexcept
{
	auto agent = std::make_unique<detail::BotManagerImpl> (spawnNothing);
	if (!agent->run (host_, port_))
		return false;

	agent->enqueueMessage (matchSettings_);

	agent->waitForWriterIdle ();
	agent->terminate ();

	return true;
}