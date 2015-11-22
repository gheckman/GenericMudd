#pragma once

// System
#include <string> 

// Boost
#include <boost/system/error_code.hpp> 

using std::string;

class MuddClient
{
  public:
    enum MessageType { EXIT, TIME, ECHO };

    MuddClient();
    ~MuddClient();

    // Brief   Starts the damn thing
    // Return  error code
    int Startup();
  protected:
  private:
    // Brief   Prompts for an ip
    // [out]   ip              contains the IPv4 address
    // Return  Error if the ip is not a correct format
    boost::system::error_code GetIp(string& ip) const;

    MessageType GetMessageType() const;


    string TimeMessage() const;


    string EchoMessage() const;
};

