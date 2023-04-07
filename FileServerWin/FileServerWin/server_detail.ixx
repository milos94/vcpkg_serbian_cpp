module;

#include <spdlog/spdlog.h>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <array>
#include <fstream>

export module server:detail;

namespace asio = boost::asio;
namespace sys = boost::system;
using boost::asio::ip::tcp;

asio::awaitable<void> session(tcp::socket socket)
{
    try
    {
        std::string data;
        data.resize(1000, '\0');
        std::size_t n = co_await socket.async_read_some(asio::buffer(data), asio::use_awaitable);
        spdlog::info("Recieved '{}' from the client. Total {} bytes", data, n);

        std::fstream file;
        file.open(data, std::fstream::out | std::fstream::binary);

        while (true)
        {
            if (n == 0)
            {
                break;
            }
            n = co_await socket.async_read_some(asio::buffer(data), asio::use_awaitable);
            spdlog::info("Got {} bytes from the client", n);
            file.write(data.data(), n);

        }

        file.close();

        n = co_await socket.async_write_some(asio::buffer("We did it"), asio::use_awaitable);
    }
    catch (sys::system_error const& e)
    {
        if (e.code() == asio::error::eof)
            spdlog::info("Session done");
        else
            spdlog::error("Session had error {}", e.what());
    }
}

asio::awaitable<void> listener(asio::io_context& context, unsigned short port)
{
    tcp::acceptor acceptor(context, { tcp::v4(), port });

    try
    {
        for (;;)
        {
            tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
            spdlog::info("Got new connection.");
            asio::co_spawn(context, session(std::move(socket)), asio::detached);
        }
    }
    catch (sys::system_error const& e)
    {
        spdlog::error("Listener had error {}", e.what());
    }
}