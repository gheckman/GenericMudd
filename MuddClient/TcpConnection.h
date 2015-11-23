#pragma once

// System
#include <string>

// Boost
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
    public:
    typedef boost::shared_ptr<TcpConnection> pointer;

    TcpConnection();
    ~TcpConnection();

    static pointer Create(io_service& io_service);

    tcp::socket& Socket()
    {
        return _socket;
    }

    // In the function start(), we call asio::async_write() to serve the data to the client.
    // Note that we are using asio::async_write(), rather than asio::ip::tcp::socket::async_write_some(), to ensure that the entire block of data is sent.
    void Start();

    private:
    tcp::socket _socket;
    std::string _message;

    TcpConnection(io_service& io_service);
    void HandleWrite(const boost::system::error_code& ec, size_t len);
};

