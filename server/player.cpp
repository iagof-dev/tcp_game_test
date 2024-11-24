#include <iostream>
#include <string>

class player{
    std::string name;
    int points;

    void setName(std::string value){
        name = value;
    }
    std::string getName(){
        return name;
    }

    void setPoints(int value){
        points = value;
    }

    int getPoints(){
        return points;
    }

};