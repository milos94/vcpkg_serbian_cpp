module;

#include <spdlog/spdlog.h>
#include <filesystem>

export module filestuff;

namespace fs = std::filesystem;

export std::optional<uintmax_t> get_file_size(std::string file_name)
{
	fs::path file_path = fs::current_path() / file_name;

	std::error_code error{};
	uintmax_t size = fs::file_size(file_path, error);
	if (error)
	{
		spdlog::error("Could not get a file size for {} because : {}", file_path.generic_string(), error.message());
		return std::nullopt;
	}
	else
	{
		spdlog::info("Size of the file {} is {}", file_path.generic_string(), size);
		return { size };
	}
}
