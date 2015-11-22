#pragma once

// System
#include <string>

using std::string;

class MuddServer
{
  public:
    MuddServer();
    ~MuddServer();

    int Startup();
  protected:
  private:
    string MakeDaytimeString() const;
    int ParseMessage(const string& msg, int len, string& response) const;
};

