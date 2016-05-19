#include "stdafx.h"

#include "CommandDecoder.hpp"
#include "Console.hpp"
#include "MuddClient.hpp"
#include "MuddServer.hpp"

#include <boost/thread/thread.hpp>

void GetServerClient(void);
void StartServer(void);
void StartClient(void);
void Test(void);

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

    //GetServerClient();
    Test();

    return 0;
}

void GetServerClient(void)
{
    std::cout << "Start as server or client? <s/c>: ";

    bool done = false;
    do {
        std::string s;
        getline(std::cin, s);
        switch(s[0])
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
    } while(!done);
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
    auto sleep = [](int ms) { boost::this_thread::sleep(boost::posix_time::milliseconds(ms)); };

    Console console;

    int a = 26;
    for (auto key = console.ReadKey(); key != 3; key = console.ReadKey())
    {
        int x, y;
        console.GetCursor(x, y);
        switch(key)
        {
            case KeyCode::ARROW_UP:
            case KeyCode::KEYBOARD_UP:
                console.SetCursor(x, y - 1);
                break;
            case KeyCode::ARROW_LEFT:
            case KeyCode::KEYBOARD_LEFT:
                console.SetCursor(x - 1, y);
                break;
            case KeyCode::ARROW_DOWN:
            case KeyCode::KEYBOARD_DOWN:
                console.SetCursor(x, y + 1);
                break;
            case KeyCode::ARROW_RIGHT:
            case KeyCode::KEYBOARD_RIGHT:
                console.SetCursor(x + 1, y);
                break;
            case 8: // backspace
                console.SetCursor(x - 1, y);
                console.WriteChar(' ');
                break;
            default:
                console.WriteChar(key);
                console.SetCursor(x + 1, y);
                if (x == Console::WIDTH - 1)
                    console.SetCursor(0, y + 1);
                break;
        }
    }
}
