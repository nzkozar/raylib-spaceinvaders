#include "game.hpp"
#include <iostream>

Game::Game(){
    CreateObstacles();    
}

Game::~Game(){}

void Game::Draw(){
    spaceship.Draw();
    for(Laser& laser: spaceship.lasers){
        laser.Draw();
    }

    for(Obstacle& obstacle: obstacles){
        obstacle.Draw();
    }
}

void Game::Update(){
    for(Laser& laser: spaceship.lasers){
        laser.Update();
    }
    DeleteInactiveLasers();
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
}

std::vector<Obstacle> Game::CreateObstacles(){
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

    return obstacles;
}
