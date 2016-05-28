#include "GameManager.hpp"

#include <algorithm>

int GameManager::AddPlayer(std::string name)
{
    if (std::any_of( _usernames.begin(), _usernames.end(), [&](const std::string& username) { return username == name; } ))
        return -1;
    _usernames.push_back(name);
    return _usernames.size() - 1;
}

std::string GameManager::GetPlayer(size_t id)
{
    std::string username;
    if (_usernames.size() < id)
        username = _usernames[id];
    return username;
}
