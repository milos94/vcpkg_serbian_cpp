module;

#include <spdlog/spdlog.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <array>
#include <string>
#include <optional>
#include <fstream>

export module client;

namespace asio = boost::asio;
namespace sys = boost::system;
using boost::asio::ip::tcp;

asio::awaitable<void> send_file_to_server(std::shared_ptr<tcp::socket> socket, std::string const& file_name, uintmax_t file_size)
{
    try
    {
        std::string data = fmt::format("{} {}", file_name, file_size);
        size_t number_of_written_bytes = co_await socket->async_write_some(asio::buffer(data), asio::use_awaitable);
        spdlog::info("Sent '{}' to the server. Successfully wrote {} bytes", data, number_of_written_bytes);

        std::fstream file;
        file.open(file_name, std::fstream::in | std::fstream::binary);

        data.clear();
        data.resize(1000, '\0');
        for (size_t i = 0; i < (file_size / 1000) + 1; ++i)
        {
            if (file.read(data.data(), 1000))
            {
                number_of_written_bytes = co_await socket->async_write_some(asio::buffer(data), asio::use_awaitable);
                spdlog::info("Wrote {} bytes to server", number_of_written_bytes);
            }
        }

        spdlog::info("Finished sending the data");

        file.close();

        data.clear();
        number_of_written_bytes = co_await socket->async_write_some(asio::buffer(data), asio::use_awaitable);
        spdlog::info("Wrote the extra empty buffer {}", number_of_written_bytes);

        data.resize(1000, '\0');
        number_of_written_bytes = co_await socket->async_read_some(asio::buffer(data), asio::use_awaitable);
        spdlog::info("Response from server {}", data);
    }
    catch (sys::system_error const& e)
    {
        spdlog::error("Client failed with error {}", e.what());
    }
}

std::shared_ptr<tcp::socket> connect(std::string const & ip, int port, std::string const& file_name, uintmax_t file_size)
{
    boost::asio::io_context io_context;

    std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(io_context);
    
    auto endpont = tcp::endpoint(boost::asio::ip::make_address_v4(ip), port);
    
    spdlog::info("Connecting to {}:{}", ip, port);
    
    sys::error_code err{};

    auto error = socket->connect(endpont,err);

    if (!error)
    {
        spdlog::info("Successfully connected");
    }
    else
    {
        spdlog::info("Failed to connect with error {}", error.message());
        return nullptr;
    }
    
    asio::co_spawn(io_context, send_file_to_server(socket, file_name, file_size), asio::detached);

    io_context.run();

    return socket;
}

export void run_client(std::string ip, int port, std::string const & file_name, uintmax_t file_size)
{
    auto socket = connect(ip, port, file_name, file_size);

}