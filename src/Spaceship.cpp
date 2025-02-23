#include "Spaceship.hpp"
#define SPACESHIP_SPEED 7

Spaceship::Spaceship(){
    image = LoadTexture("Graphics/spaceship.png");
    InitPosition();
    lastFiredTime = 0.0;
}

Spaceship::~Spaceship(){
    UnloadTexture(image);
}

void Spaceship::Draw(){
    DrawTextureV(image,position,WHITE);
}

void Spaceship::MoveLeft(){
    position.x -= SPACESHIP_SPEED;
    if(position.x<25){
        position.x = 25;
    }
}

void Spaceship::MoveRight(){
    position.x += SPACESHIP_SPEED;
    if(position.x>GetScreenWidth()-image.width - 25){
        position.x = GetScreenWidth()-image.width - 25;
    }
}

void Spaceship::FireLaser(){
    if(GetTime()-lastFiredTime >= 0.35){
        lasers.push_back(Laser({position.x + image.width/2 - Laser::WIDTH/2,position.y},LASER_SPEED));
        lastFiredTime = GetTime();
    }
}

Rectangle Spaceship::GetRect(){
    return {position.x,position.y,float(image.width),float(image.height)};
}

void Spaceship::Reset(){
    InitPosition();
    lasers.clear();
}

void Spaceship::InitPosition(){
    position.x = (GetScreenWidth() - image.width)/2;
    position.y = GetScreenHeight() - image.height - 100;
}
