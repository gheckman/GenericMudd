#include "MuddServer.h"

// System
#include <ctime>
#include <iostream>

// Boost
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;

MuddServer::MuddServer()
{}

MuddServer::~MuddServer()
{}

int MuddServer::Startup()
{
    try
    {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 10616));

        // TODO: Only allows a single client at a time. Allow multiple
        while (true)
        {
            tcp::socket socket(io_service);
            acceptor.accept(socket);

            while (true)
            {
                // Read from the client
                boost::array<char, 128> buf;
                boost::system::error_code error;
                size_t len = socket.read_some(boost::asio::buffer(buf), error);
                if (error) break;

                // Send back to the client
                string response;
                ParseMessage(buf.c_array(), len, response);
                boost::system::error_code ignored_error;
                boost::asio::write(socket, boost::asio::buffer(response), ignored_error);
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

string MuddServer::MakeDaytimeString() const
{
    time_t now = time(NULL);
    char buf[26] = { 0 };
    ctime_s(buf, 26, &now);
    return buf;
}

int MuddServer::ParseMessage(const string& message, int len, string& response) const
{
    cout << "Message received from client: " << endl;
    cout << message.substr(0, len) << endl << endl;

    if (len < 4) return -1;
    string header = message.substr(0, 4);

    // TODO: can't switch strings... some other method?
    if (header == "TIME")
        response = MakeDaytimeString();
    else if (header == "ECHO")
        response = message.substr(4, len - 4) + '\n';

    return 0;
}
