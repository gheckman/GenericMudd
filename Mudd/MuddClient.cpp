#include "MuddClient.hpp"

#include <deque>
#include <cstdlib>
#include <iostream>
#include <functional>

using boost::asio::async_read;
using boost::asio::buffer;
using std::bind;
using std::placeholders::_1;

MuddClient::MuddClient(io_service& ioService, tcp::resolver::iterator endpointIterator) :
    _ioService(ioService),
    _socket(ioService)
{
    async_connect(_socket, endpointIterator, bind(&MuddClient::HandleConnect, this, _1));
}

void MuddClient::Write(MessageBuffer& msgs)
{
    _writeMsg = msgs.Serialize();
    _ioService.post( [&]{DoWrite(_writeMsg);} );
}

void MuddClient::HandleConnect(const boost::system::error_code& error)
{
    if (!error)
    {
        // Reserve space for the header, then read it in
        _readMsg.resize(MessageBuffer::HEADER_LENGTH, '\0');
        async_read(_socket, buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH), bind(&MuddClient::HandleReadHeader, this, _1));
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
        async_read(_socket, buffer(_readMsg.data() + MessageBuffer::HEADER_LENGTH, payloadSize), bind(&MuddClient::HandleReadBody, this, _1));
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
        async_read(_socket, buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH), bind(&MuddClient::HandleReadHeader, this, _1));
    }
    else
    {
        DoClose();
    }
}

void MuddClient::DoWrite(const std::vector<char>& buf)
{
    async_write(_socket, buffer(_writeMsg.data(), _writeMsg.size()), bind(&MuddClient::HandleWrite, this, _1));
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