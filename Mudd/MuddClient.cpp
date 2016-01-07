//#include <boost/thread/thread.hpp>

#include "MuddClient.hpp"

MuddClient::MuddClient(boost::asio::io_service& ioService, tcp::resolver::iterator endpointIterator) :
    _ioService(ioService),
    _socket(ioService)
{
    auto handleConnect = [&](const boost::system::error_code& ec, tcp::resolver::iterator) { HandleConnect(ec); };
    boost::asio::async_connect(_socket, endpointIterator, handleConnect);
}

void MuddClient::Write(MessageBuffer& msgs)
{
    _writeMsg = msgs.Serialize();
    _ioService.post([&] { DoWrite(_writeMsg); });
}

void MuddClient::HandleConnect(const boost::system::error_code& error)
{
    if (!error)
    {
        // Reserve space for the header, then read it in
        _readMsg.resize(MessageBuffer::HEADER_LENGTH, '\0');
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH),
                                boost::bind(&MuddClient::HandleReadHeader, this, boost::asio::placeholders::error));
    }
}

void MuddClient::HandleReadHeader(const boost::system::error_code& error)
{
    if (!error)
    {
        // Get the payload size from the header
        auto payloadSize = MessageBuffer::PayloadSize(_readMsg.data());

        // reserve enough space for the header and the payload, then read in the payload
        _readMsg.resize(MessageBuffer::HEADER_LENGTH + payloadSize, '\0');
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_readMsg.data() + MessageBuffer::HEADER_LENGTH, payloadSize),
                                boost::bind(&MuddClient::HandleReadBody, this, boost::asio::placeholders::error));
    }
    else
    {
        DoClose();
    }
}

void MuddClient::HandleReadBody(const boost::system::error_code& error)
{
    if (!error)
    {
        MessageBuffer msgs;
        msgs.Deserialize(_readMsg.data());

        ProcessMessageBuffer(msgs);

        // Reserve space for the header, then read it in
        _readMsg.resize(MessageBuffer::HEADER_LENGTH, '\0');
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH),
                                boost::bind(&MuddClient::HandleReadHeader, this, boost::asio::placeholders::error));
    }
    else
    {
        DoClose();
    }
}

void MuddClient::DoWrite(const std::vector<char>& buf)
{
    boost::asio::async_write(_socket,
                                boost::asio::buffer(_writeMsg.data(), _writeMsg.size()),
                                boost::bind(&MuddClient::HandleWrite, this, boost::asio::placeholders::error));
}

void MuddClient::ProcessMessageBuffer(MessageBuffer& msgs)
{
    auto msg_up = msgs.Pop();
    while (msg_up)
    {
        if (msg_up->Header() == ChatMessage::HEADER)
        {
            auto msg = dynamic_cast<ChatMessage*>(msg_up.get());
            ProcessMessage(*msg);
        }
        msg_up = msgs.Pop();
    }
}

void MuddClient::ProcessMessage(const ChatMessage& chat)
{
    auto s = chat.Chat();
    std::cout << s << std::endl;
}