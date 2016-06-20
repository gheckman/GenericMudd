//#define TESTING

#include "stdafx.h"

// 
#include "BasicSerializer.hpp"
#include "CommandDecoder.hpp"
#include "CommonConst.hpp"
#include "MuddClient.hpp"
#include "MuddServer.hpp"

//Standard
#include <iostream>
#include <string>

//Boost
#include <boost/thread/thread.hpp>

//Curses
#include <curses.h>

using namespace std::literals::string_literals;

void GetServerClient();
void StartServer();
void StartClient();
void GetHostConnectionData(boost::asio::io_service& ioService, tcp::resolver::iterator& ioIterator);
void Test();

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

#ifndef TESTING
    if (argc > 1)
    {
        if (argv[1] == "--server"s || argv[1] == "-s"s)
            StartServer();
        else if (argv[1] == "--client"s || argv[1] == "-c"s)
            StartClient();
        else
            GetServerClient();
    }
    else
        GetServerClient();
#else
    Test();
#endif

    return 0;
}

void Test() { }

void GetServerClient()
{
    std::cout << "Start as server or client? <s/c>: ";

    bool done = false;
    do
    {
        std::string s;
        getline(std::cin, s);
        switch (s[0])
        {
            case 'c':
                std::cout << "Starting as client: " << std::endl;
                StartClient();
                done = true;
                break;
            case 's':
                std::cout << "Starting as server: " << std::endl;
                StartServer();
                done = true;
                break;
        }
    } while (!done);
}

void StartServer()
{
    try
    {
        std::string port = "10616";
        boost::asio::io_service ioService;
        MuddServer_list servers;
        tcp::endpoint endpoint(tcp::v4(), atoi(port.c_str()));
        MuddServer_ptr server(new MuddServer(ioService, endpoint));
        servers.push_back(server);

        ioService.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void StartClient()
{
    try
    {
        boost::asio::io_service ioService;
        tcp::resolver::iterator ioIterator;
        GetHostConnectionData(ioService, ioIterator);
        MuddClient client(ioService, ioIterator);

        boost::thread ioThread(boost::bind(&boost::asio::io_service::run, &ioService));

        std::string line;
        MessageBuffer msgs;
        CommandDecoder decoder(0);

        while(true)
        {
            std::getline(std::cin, line);
            if (line != "")
            {
                auto msg = decoder.Decode(line);
                msgs.Push(msg);
                client.Write(msgs);
                msgs.Clear();
            }
        }

        client.Close();
        ioThread.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}


void GetHostConnectionData(boost::asio::io_service& ioService, tcp::resolver::iterator& ioIterator)
{
    std::string hostIp;
    std::string port("10616");

    tcp::resolver resolver(ioService);
    boost::system::error_code ec;

    do
    {
        std::cin.clear();
        std::getline(std::cin, hostIp);
        tcp::resolver::query query(hostIp, port);
        ioIterator = resolver.resolve(query, ec);
    } while (ec);
}
