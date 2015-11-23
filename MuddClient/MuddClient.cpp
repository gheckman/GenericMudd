#include "MuddClient.h"

// System
#include <iostream>

// Boost
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

MuddClient::MuddClient()
{}

MuddClient::~MuddClient()
{}

int MuddClient::Startup()
{
    try
    {
        // Get the IPv4 address
        string ip;
        boost::system::error_code ec;

        while (true)
        {
            // Input
            ec = GetIp(ip);

            // Validation
            if (!ec) break;

            // User correction
            cout << "IP is not in the correct format." << endl;
            cout << "IPv4 format - 255.255.255.255" << endl;
        }

        // Connect to things or something
        io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(ip, "10616");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::socket socket(io_service);
        connect(socket, endpoint_iterator);

        while (true)
        {
            bool exit = false;
            // TODO: encapsulate switch functionality in separate parser class?
            string message;
            switch (GetMessageType())
            {
                case EXIT: exit = true;
                    break;
                case TIME: message = TimeMessage();
                    break;
                case ECHO: message = EchoMessage();
                    break;
            }
            if (exit) break;

            // Send to the server
            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);

            // Read the response
            boost::array<char, 128> buf;
            boost::system::error_code error;
            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            std::cout.write(buf.data(), len);
        }
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}

boost::system::error_code MuddClient::GetIp(string& ip) const
{
    cout << "Enter IPv4 address" << endl;
    cout << "> ";
    getline(cin, ip);

    boost::system::error_code ec;
    boost::asio::ip::address::from_string(ip, ec);
    return ec;
}

MuddClient::MessageType MuddClient::GetMessageType() const
{
    string choice;

    while (true)
    {
        // Input
        cout << "1) EXIT" << endl;
        cout << "2) TIME" << endl;
        cout << "3) ECHO" << endl;
        cout << "> ";
        getline(cin, choice);

        // Validation
        if (choice[0] >= '1' && choice[0] <= '3') break;

        // User correction
        cout << "Enter a valid choice you wank stain." << endl;
    };

    MessageType msg;

    // This is retarded. Don't do this
    switch(choice[0])
    {
        case '1': msg = EXIT; break;
        case '2': msg = TIME; break;
        case '3': msg = ECHO; break;
    }

    return msg;
}

string MuddClient::TimeMessage() const
{
    const string header = "TIME";
    return header;
}

string MuddClient::EchoMessage() const
{
    const string header = "ECHO";
    string echo;

    cout << "Enter the message you would like echo'ed: " << endl;
    cout << "> ";
    getline(cin, echo);

    return header + echo;
}
