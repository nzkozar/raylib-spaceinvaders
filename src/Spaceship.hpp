#pragma once
#include <raylib.h>
#include "laser.hpp"
#include <vector>

class Spaceship{

    const int LASER_SPEED = -7;

    public:
        Spaceship();
        ~Spaceship();
        void Draw();
        void MoveLeft();
        void MoveRight();
        void FireLaser();
        std::vector<Laser> lasers;
    private:
        Texture2D image;
        Vector2 position;
        double lastFiredTime;
};