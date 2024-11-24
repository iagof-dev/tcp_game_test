#include <iostream>
#include <headers/player.h>
#include <ctime>

class session{
    bool isActive;
    std::string roomName;
    int state;
    
    int max_players;
    int players_connected;

    time_t total_time;
    time_t total_time_isActiveplaying;

    int rounds;

    // content
    
};