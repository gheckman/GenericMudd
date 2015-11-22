#if defined(_WIN32) and not defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501
#endif

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std;

    time_t now = time(NULL);
    char buf[26] = {0};
    ctime_s(buf, 26, &now);
    return buf;
}

int main()
{
    try
    {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 13));
        for (;;)
        {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            std::string message = make_daytime_string();

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}