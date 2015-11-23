#include "TcpServer.h"

// Boost
#include <boost/bind.hpp>

// Local
#include "TcpConnection.h"

TcpServer::TcpServer(io_service& ioService, int port) : _acceptor(ioService, tcp::endpoint(tcp::v4(), port))
{
    StartAccept();
}

TcpServer::~TcpServer()
{}

void TcpServer::StartAccept()
{
    TcpConnection::pointer newConnection =
        TcpConnection::Create(_acceptor.get_io_service());

    _acceptor.async_accept(newConnection->Socket(),
                           boost::bind(&TcpServer::HandleAccept, this, newConnection,
                           placeholders::error));
}

void TcpServer::HandleAccept(TcpConnection::pointer newConnection, const boost::system::error_code& error)
{
    if (!error)
    {
        newConnection->Start();
        StartAccept();
    }
}