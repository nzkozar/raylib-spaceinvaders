#include "game.hpp"
#include <iostream>
#include <string>

std::string FormatWithLeadingZeroes(int number, int width){
    std::string numberText = std::to_string(number);
    int leadingZeroes = width - numberText.length();
    return std::string(leadingZeroes,'0') + numberText;
}

//Constructor
Game::Game(){
    InitGame();
    
    font = LoadFontEx("Font/monogram.ttf",64,0,0);
    spaceshipImage = LoadTexture("Graphics/spaceship.png");
}

//Destructor
Game::~Game(){
    Alien::UnloadImages();
    UnloadFont(font);
}

void Game::Draw(){
    if(run)spaceship.Draw();
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

void Game::DrawUI(){
    DrawRectangleRoundedLinesEx({10,10,780,780},0.18f,20,3,UI_COLOR);
    DrawLineEx({25,730},{775,730},3,UI_COLOR);

    //Level / game state text
    if(run){
        DrawTextEx(font, "LEVEL 01", {570,740} ,34, 2, UI_COLOR);
    }else{
        DrawTextEx(font, "GAME OVER", {570,740}, 34, 2, UI_COLOR);
    }
    
    //Lives represented with spaceship images
    for(int i = 0; i < lives; i++){
        DrawTextureV(spaceshipImage,{50.0f * (i+1),745}, WHITE);
    }

    //Score text
    DrawTextEx(font, "SCORE", {50,15}, 34, 2, UI_COLOR);
    std::string scoreText = FormatWithLeadingZeroes(score,5);
    DrawTextEx(font, scoreText.c_str(), {50,45}, 34, 2, UI_COLOR);
}

void Game::Update(){
    if(run){
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
    
        CheckForCollisions();
    }else{
        if(IsKeyDown(KEY_ENTER)){
            Reset();
            InitGame();
        }
    }
}

void Game::HandleInput(){
    if(run){
        if(IsKeyDown(KEY_LEFT)){
            spaceship.MoveLeft();
        }else if(IsKeyDown(KEY_RIGHT)){
            spaceship.MoveRight();
        }
        
        if(IsKeyDown(KEY_SPACE)){
            spaceship.FireLaser();
        }
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
                float(GetScreenHeight()-200)
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
        if((alien.position.x + alien.GetImage().width) > GetScreenWidth() - 25){
            aliensDirection = -1;
            MoveDownAliens(4);
        }else if(alien.position.x < 25){
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

void Game::CheckForCollisions(){
    //Spaceship lasers
    for(Laser& laser: spaceship.lasers){
        //Aliens
        auto it = aliens.begin();
        while(it != aliens.end()){
            if(CheckCollisionRecs(it -> GetRect(), laser.GetRect())){
                score+= (it->type)*100;

                it = aliens.erase(it);
                laser.isActive = false;

            }else{
                ++it;
            }
        }

        //Obstacles
        for(Obstacle& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> GetRect(), laser.GetRect())){
                    it = obstacle.blocks.erase(it);
                    laser.isActive = false;
                }else{
                    ++it;
                }
            }
        }

        //Mystery ship
        if(CheckCollisionRecs(mysteryShip.GetRect(),laser.GetRect())){
            mysteryShip.alive = false;
            laser.isActive = false;
            score+=500;
        }
    }

    //Alien lasers
    for(Laser& laser: alienLasers){
        //Player ship
        if(CheckCollisionRecs(spaceship.GetRect(),laser.GetRect())){
            //TODO life count down
            laser.isActive = false;
            //Deduct score
            score-=1000;
            if(score<0)score=0;
            //Deduct lives
            lives--;
            if(lives == 0){
                GameOver();
            }
        }

        for(Obstacle& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> GetRect(), laser.GetRect())){
                    it = obstacle.blocks.erase(it);
                    laser.isActive = false;
                }else{
                    ++it;
                }
            }
        }
    }

    //Aliens
    for(Alien& alien: aliens){
        //Obstacles
        for(Obstacle& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> GetRect(),alien.GetRect())){
                    it = obstacle.blocks.erase(it);
                }else{
                    it ++;
                }
            }
        }

        //Spaceship
        if(CheckCollisionRecs(alien.GetRect(),spaceship.GetRect())){
            GameOver();
        }
    }
}

void Game::GameOver(){
    std::cout << "Game over" << std::endl;
    run = false;
}

void Game::Reset(){
    spaceship.Reset();

    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}

void Game::InitGame(){
    CreateObstacles();
    CreateAliens();
    aliensDirection = 1;
    lastAlienLaserFiredTime = 5;
    lives = 3;
    score = 0;
    run = true;

    //Mystery ship
    mysteryShipSpawnInterval = GetRandomValue(10,20);
    mysteryShipLastSpawnTime = 0.0;
}
