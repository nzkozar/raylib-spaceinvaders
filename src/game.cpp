#include "game.hpp"
#include <iostream>

//Constructor
Game::Game(){
    CreateObstacles();
    CreateAliens();
    aliensDirection = 1;
    lastAlienLaserFiredTime = 5;
    
    //Mystery ship
    mysteryShipSpawnInterval = GetRandomValue(10,20);
    mysteryShipLastSpawnTime = 0.0;
}

//Destructor
Game::~Game(){
    Alien::UnloadImages();
}

void Game::Draw(){
    spaceship.Draw();
    for(Laser& laser: spaceship.lasers){
        laser.Draw();
    }

    for(Obstacle& obstacle: obstacles){
        obstacle.Draw();
    }

    for(Alien& alien: aliens){
        alien.Draw();
    }

    for(Laser& alienLaser: alienLasers){
        alienLaser.Draw();
    }

    mysteryShip.Draw();
}

void Game::Update(){
    for(Laser& laser: spaceship.lasers){
        laser.Update();
    }

    for(Laser& laser: alienLasers){
        laser.Update();
    }

    DeleteInactiveLasers();
    MoveAliens();
    AlienShootLaser();

    //Mystery ship
    if(GetTime() - mysteryShipLastSpawnTime > mysteryShipSpawnInterval){
        mysteryShip.Spawn();
        mysteryShipLastSpawnTime = GetTime();
        mysteryShipSpawnInterval = GetRandomValue(10,20);
    }

    mysteryShip.Update();
}

void Game::HandleInput(){
    if(IsKeyDown(KEY_LEFT)){
        spaceship.MoveLeft();
    }else if(IsKeyDown(KEY_RIGHT)){
        spaceship.MoveRight();
    }
    
    if(IsKeyDown(KEY_SPACE)){
        spaceship.FireLaser();
    }
}

void Game::DeleteInactiveLasers(){
    for(auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();){
        if(!it -> isActive){
            it = spaceship.lasers.erase(it);
        }else{
            ++ it;
        }
    }

    for(auto it = alienLasers.begin(); it != alienLasers.end();){
        if(!it -> isActive){
            it = alienLasers.erase(it);
        }else{
            ++ it;
        }
    }
}

void Game::CreateObstacles(){
    unsigned int obstacleWidth = Obstacle::grid[0].size() * Block::SIDE_LENGTH;
    float gapWidth = int(GetScreenWidth() - (4 * obstacleWidth))/5;

    for(int i=0;i<4;++i){
        float offsetX = (i + 1) * gapWidth + i * obstacleWidth;
        Obstacle obstacle = Obstacle({
                offsetX, 
                float(GetScreenHeight()-100)
            });
        obstacles.push_back(obstacle);
    }
}

void Game::CreateAliens(){
    for(int row = 0; row < 5; row++){
        for(int col = 0;col<11;col++){
            int alienType = 1;
            if(row == 0){
                alienType = 3;
            }else if(row < 3){
                alienType = 2;
            }

            float x = col * 55 + 75;
            float y = row * 55 + 110;
            aliens.push_back(Alien(alienType,{x,y}));
        }
    }
}

void Game::MoveAliens(){
    for(Alien& alien: aliens){
        if((alien.position.x + alien.GetImage().width) > GetScreenWidth()){
            aliensDirection = -1;
            MoveDownAliens(4);
        }else if(alien.position.x < 0){
            aliensDirection = 1;
            MoveDownAliens(4);
        }

        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance){
    for(Alien& alien: aliens){
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser(){
    float timeDiff = GetTime() - lastAlienLaserFiredTime;
    if(timeDiff >= alienLaserShootInterval && !aliens.empty()){
        int randomIndex = GetRandomValue(0,aliens.size()-1);
        Alien& alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.GetImage().width/2,alien.position.y},4));
        lastAlienLaserFiredTime = GetTime();
    }
}
