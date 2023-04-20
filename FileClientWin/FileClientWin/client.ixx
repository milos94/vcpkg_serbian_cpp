module;

#include <spdlog/spdlog.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <string>
#include <fstream>

export module client;

namespace asio = boost::asio;
namespace sys = boost::system;
using boost::asio::ip::tcp;

asio::awaitable<void> send_file_to_server(tcp::socket socket, std::string const& file_name)
{
    try
    {
        std::string data = fmt::format("{}", file_name);
        size_t number_of_written_bytes = co_await socket.async_write_some(asio::buffer(data), asio::use_awaitable);
        spdlog::info("Sent '{}' to the server. Successfully wrote {} bytes", data, number_of_written_bytes);

        std::fstream file;
        file.open(file_name, std::fstream::in | std::fstream::binary);

        data.clear();
        data.resize(1000, '\0');

        while (file.read(data.data(), 1000))
        {
            number_of_written_bytes = co_await socket.async_write_some(asio::buffer(data), asio::use_awaitable);
            spdlog::debug("Wrote {} bytes to server", number_of_written_bytes);
        }

        spdlog::info("Finished sending the data");

        file.close();

        data.clear();
        number_of_written_bytes = co_await socket.async_write_some(asio::buffer(""), asio::use_awaitable);
        spdlog::info("Wrote the extra empty buffer {}", number_of_written_bytes);

        data.resize(1000, '\0');
        number_of_written_bytes = co_await socket.async_read_some(asio::buffer(data), asio::use_awaitable);
        spdlog::info("Response from server {}", data);
    }
    catch (sys::system_error const& e)
    {
        spdlog::error("Client failed with error {}", e.what());
    }
}

export void run_client(std::string const& ip, int port, std::string const& file_name)
{
    boost::asio::io_context io_context;

    tcp::socket socket{ io_context };

    auto endpont = tcp::endpoint(boost::asio::ip::make_address_v4(ip), port);

    spdlog::info("Connecting to {}:{}", ip, port);

    sys::error_code err{};
    sys::error_code error = socket.connect(endpont, err);

    if (!error)
    {
        spdlog::info("Successfully connected");
    }
    else
    {
        spdlog::error("Failed to connect with error {}", error.message());
    }

    asio::co_spawn(io_context, send_file_to_server(std::move(socket), file_name), asio::detached);

    io_context.run();
}