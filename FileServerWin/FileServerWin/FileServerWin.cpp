// FileServerWin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <spdlog/spdlog.h>

import server;
import std.core;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		spdlog::error("Not enough arguments, port is required");
		return 1;
	}

	std::string_view port{ argv[1] };
	int port_number{};
	auto [ptr, ec] { std::from_chars(port.data(), port.data() + port.size(), port_number) };

	if (ec == std::errc())
	{
		spdlog::info("Port number is {}", port_number);
		run_server(port_number);
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