#pragma once

#include "CommonConst.hpp"

#include <vector>

class GameManager
{
    GameManager() : _usernames() { }

    int AddPlayer(std::string name);

    std::string GetPlayer(size_t id);

    private:
    std::vector<std::string> _usernames;
};