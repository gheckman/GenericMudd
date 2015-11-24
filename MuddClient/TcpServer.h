#pragma once

// System
#include <string>

// Boost
#include <boost/asio.hpp>

// Local
#include "TcpConnection.h"

using namespace boost::asio;

class TcpServer
{
  public:

    //The constructor initialises an acceptor to listen on TCP port 10616.
    TcpServer(io_service & ioService, int port = 10616);

    ~TcpServer();

  private:
  private:
    tcp::acceptor _acceptor;

    //The function StartAccept() creates a socket and initiates an asynchronous accept operation to wait for a new connection.
    void StartAccept();
        
    // The function HandleAccept() is called when the asynchronous accept operation initiated by StartAccept() finishes.
    // It services the client request, and then calls StartAccept() to initiate the next accept operation.
    void HandleAccept(TcpConnection::pointer newConnection, const boost::system::error_code& error);
};

