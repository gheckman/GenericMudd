//#define TESTING

#include "stdafx.h"

#include "BasicSerializer.hpp"
#include "CommandDecoder.hpp"
#include "CommonConst.hpp"
#include "Console.hpp"
#include "ChatConsole.hpp"
#include "MuddClient.hpp"
#include "MuddServer.hpp"

#include <string>
#include <boost/thread/thread.hpp>

using namespace std::literals::string_literals;

void GetServerClient(void);
void StartServer(void);
void StartClient(void);
void GetHostConnectionData(boost::asio::io_service& ioService, tcp::resolver::iterator& ioIterator);
void Test(void);

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

void GetServerClient(void)
{
    Console console;

    console.Clear();
    console.SetCursor(0, 0);

    std::cout << "1.) Start as Client" << std::endl;
    std::cout << "2.) Start as Server" << std::endl;

    auto c = console.ReadChar({'1', '2'});

    console.Clear();
    console.SetCursor(0, 0);

    switch (c)
    {
        case '1': StartClient(); break;
        case '2': StartServer(); break;
        default: break;
    }
}

void StartServer(void)
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

void StartClient(void)
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
        while (getline(std::cin, line))
        {
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
    Console console;
    std::string port("10616");

    tcp::resolver resolver(ioService);
    boost::system::error_code ec;

    console.SetCursor(0, 0);
    console.WriteString("Enter host ip: ");
    do
    {
        console.SetCursor(0, 1);
        console.ClearLine();
        auto hostIp = console.ReadLine();

        tcp::resolver::query query(hostIp, port);
        ioIterator = resolver.resolve(query, ec);
    } while (ec);

    console.SetCursor(0, 0);
    console.Clear();
}

void Test(void)
{

    getchar();
}
