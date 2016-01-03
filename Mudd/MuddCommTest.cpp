#include "MuddServer.hpp"
#include "MuddClient.hpp"
#include "CommandDecoder.hpp"

void StartServer();
void StartClient();

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));
    getchar() == 's' ? StartServer() : StartClient();
    return 0;
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
                msgs.Push(CommandDecoder::Decode(line));
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
