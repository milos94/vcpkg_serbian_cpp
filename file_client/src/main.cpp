#include <string>
#include <string_view>
#include <charconv>

#include <spdlog/spdlog.h>

#include "client/client.hpp"

int main(int argc, char ** argv)
{
    if(argc < 4)
    {
        spdlog::error("Not enough arguments, four required IP ADDRESS, PORT NUMBER and FILE NAME");
    }
    std::string ip{argv[1]};
    std::string file_name{argv[3]};
    
    int port_number{};
    std::string_view port{argv[2]};
	
    auto [ptr, ec] { std::from_chars(port.data(), port.data() + port.size(), port_number) };

	if (ec == std::errc())
	{
		spdlog::info("Port number is {}", port_number);
		run_client(ip, port_number, file_name);
	}
	else if (ec == std::errc::invalid_argument)
	{
		spdlog::error("That isn't a number.");
	}
	else if (ec == std::errc::result_out_of_range)
	{
		spdlog::error("This number is larger than an int.");
	}

    return 0;
}