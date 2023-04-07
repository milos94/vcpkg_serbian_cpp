module;

#include <spdlog/spdlog.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

export module server;

import :detail;

namespace asio = boost::asio;

export void run_server()
{
    try
    {
        spdlog::info("Starting up server. Listening on port 55555");
        
        asio::io_context context;

        asio::signal_set signals(context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { context.stop(); });
        auto listen = listener(context, 55555);
        
        asio::co_spawn(context, std::move(listen), asio::detached);

        context.run();
        spdlog::info("Server done");
    }
    catch (std::exception& e)
    {
        spdlog::error("Server failure: {}" , e.what());
    }
}
