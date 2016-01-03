//
// MuddClient.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "Message.hpp"

using boost::asio::ip::tcp;

class MuddClient
{
    public:
    MuddClient(boost::asio::io_service& ioService, tcp::resolver::iterator endpointIterator) :
        _ioService(ioService),
        _socket(ioService)
    {
        boost::asio::async_connect(_socket, endpointIterator,
                                   boost::bind(&MuddClient::HandleConnect, this, boost::asio::placeholders::error));
    }

    void Write(MessageBuffer& msgs)
    {
        _writeMsg = msgs.Serialize();
        _ioService.post( [&]{DoWrite(_writeMsg);} );
    }

    void Close()
    {
        _ioService.post( [&]{DoClose();} );
    }

    private:
    void HandleConnect(const boost::system::error_code& error)
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

    void HandleReadHeader(const boost::system::error_code& error)
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

    void HandleReadBody(const boost::system::error_code& error)
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

    void DoWrite(const std::vector<char>& buf)
    {
        boost::asio::async_write(_socket,
                                 boost::asio::buffer(_writeMsg.data(), _writeMsg.size()),
                                 boost::bind(&MuddClient::HandleWrite, this, boost::asio::placeholders::error));
    }

    void HandleWrite(const boost::system::error_code& error)
    {
        if (error)
            DoClose();
    }

    void ProcessMessageBuffer(MessageBuffer& msgs)
    {
        auto msg_up = msgs.Pop();
        while (msg_up)
        {
            if (msg_up->Header() == ChatMessage::HEADER())
            {
                auto msg = dynamic_cast<ChatMessage*>(msg_up.get());
                ProcessMessage(*msg);
            }
            msg_up = msgs.Pop();
        }
    }

    void ProcessMessage(const ChatMessage& chat)
    {
        auto s = chat.Chat();
        cout << s << endl;
    }

    void DoClose()
    {
        _socket.close();
    }

    boost::asio::io_service& _ioService;
    tcp::socket _socket;
    std::vector<char> _readMsg;
    std::vector<char> _writeMsg;
};
