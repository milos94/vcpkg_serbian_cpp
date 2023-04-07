import client;
import filestuff;
import std.core;

int main()
{
    std::string ip{"192.168.1.103"};
    int port{55555};
    std::string file_name{ "Veliki_Kazan.jpg" };
    auto res = get_file_size(file_name);
    if (res.has_value())
    {
        run_client(ip, port, file_name, res.value());
    }
    return 0;
}