/* TODO:
 * Doxygen
 * Get rid of _WIN32_WINNT error
 */

// System
#include <iostream>
#include <limits>
#include <string>

// Boost
#include <boost/array.hpp>
#include <boost/asio.hpp>

// Local
#include "MuddServer.h"
#include "MuddClient.h"

using namespace std;
using boost::asio::ip::tcp;

string MakeDaytimeString();
char GetChar(istream& is);
void SideSwitch();

int main(int argc, char* argv[])
{
    SideSwitch();
    return 0;
}

void SideSwitch()
{
    char c = '\0';

    // TODO: Input validation looks fairly repetitive. Make a function for it?
    // Something like GetInput(string input_func(void), bool validation_func(string), void correction_func(void))?
    while (true)
    {
        // Input
        cout << "Start as client or server? <c/s>: ";
        c = GetChar(cin);
        tolower(c);

        // Validation
        if (c == 's' || c == 'c') break;

        // User correction
        cout << "Please enter a single character" << endl;
        cout << "  c for client process" << endl;
        cout << "  s for server process" << endl;
    };

    if (c == 'c')
    {
        MuddClient c;
        c.Startup();
    }
    else
    {
        MuddServer s;
        s.Startup();
    }
}

char GetChar(istream& is)
{
    char c;
    is >> c;
    is.clear();
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return c;
}