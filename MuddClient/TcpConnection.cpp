#include "TcpConnection.h"

// Boost
#include <boost/bind.hpp>

using namespace boost::asio::placeholders;

TcpConnection::TcpConnection()
{}

TcpConnection::~TcpConnection()
{}

TcpConnection::pointer TcpConnection::Create(io_service & io_service)
{
    return pointer(new TcpConnection(io_service));
}

void TcpConnection::Start()
{
    // The data to be sent is stored in the class member _message as we need to keep the data valid until the asynchronous operation is complete.
    _message = "Hello World";

    // When initiating the asynchronous operation, and if using boost::bind, you must specify only the arguments that match the handler's parameter list.
    // In this program, both of the argument placeholders (asio::placeholders::error and asio::placeholders::bytes_transferred) could potentially have been removed, since they are not being used in handle_write().

    async_write(_socket, buffer(_message),
                boost::bind(&TcpConnection::HandleWrite, shared_from_this(),
                placeholders::error,
                placeholders::bytes_transferred));

    // Any further actions for this client connection are now the responsibility of handle_write().
}


TcpConnection::TcpConnection(io_service& io_service) : _socket(io_service)
{}

void TcpConnection::HandleWrite(const boost::system::error_code & ec, size_t len)
{}
