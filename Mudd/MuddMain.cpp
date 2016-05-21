#include "stdafx.h"

#include "CommandDecoder.hpp"
#include "Common.hpp"
#include "Console.hpp"
#include "ChatConsole.hpp"
#include "MuddClient.hpp"
#include "MuddServer.hpp"

#include <string>
#include <boost/thread/thread.hpp>

void GetServerClient(void);
void StartServer(void);
void StartClient(void);
void Test(void);

using namespace std::literals::string_literals;

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

    /*
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
    */
    Test();

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

    switch (c)
    {
        case '1': StartClient(); break;
        case '2': StartServer(); break;
        default: break;
    }

    console.Clear();
    console.SetCursor(0, 0);
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
        std::string host = "127.0.0.1";
        std::string port = "10616";

        boost::asio::io_service ioService;

        tcp::resolver resolver(ioService);
        tcp::resolver::query query(host, port);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        MuddClient client(ioService, iterator);

        boost::thread ioThread(boost::bind(&boost::asio::io_service::run, &ioService));

        std::string line;
        MessageBuffer msgs;
        while (getline(std::cin, line))
        {
            if (line != "")
            {
                auto msg = CommandDecoder::Decode(line);
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

void Test()
{
    // insert some tests
    ChatConsole chatConsole(1, 1, 114, 20);

    chatConsole.AddMessage(RoomType::GLOBAL, "DeathTails", "Test message type 1");
    chatConsole.AddMessage(RoomType::GLOBAL, "Trevor", "Test message type 2 abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");

    for (int i = 0; i < 25; ++i)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
        chatConsole.AddMessage(RoomType::GLOBAL, "DeathTails", "Test message type " + std::to_string(i));
    }

    getchar();
}
