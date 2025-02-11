#pragma once
#include "Spaceship.hpp"
#include "obstacle.hpp"
#include "alien.hpp"
#include "mysteryship.hpp"

class Game{
    public:
        Game(); //Constructor
        ~Game(); //Destructor
        void Draw();
        void Update();
        void HandleInput();
    private:
        void DeleteInactiveLasers();
        void CreateObstacles();
        void CreateAliens();
        void MoveAliens();
        void MoveDownAliens(int distance);
        void AlienShootLaser();
        Spaceship spaceship;
        std::vector<Obstacle> obstacles;
        std::vector<Alien> aliens;
        std::vector<Laser> alienLasers;
        int aliensDirection;
        constexpr static float alienLaserShootInterval = 0.45;
        float lastAlienLaserFiredTime;
        MysteryShip mysteryShip;
        float mysteryShipSpawnInterval;
        float mysteryShipLastSpawnTime;
};
