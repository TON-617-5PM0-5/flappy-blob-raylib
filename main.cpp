#include "raylib.h"
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
// Flappy bird





const int windowSizeX = 800;
const int windowSizeY = 800;

std::random_device rd; 
std::mt19937 gen(rd()); 
std::uniform_real_distribution<float> heightDist(-300, 300);
std::uniform_real_distribution<float> widthDist(40, 100);



struct Bird {
    Vector2 position;
    float impact;
    float gravity;
    float jumpForce;
    float radius;
};

struct Pipe {
    float x;
    float width;
    float windowTop;
    float windowBottom;
};



void drawStartMenu(){
    std::string text = "press Space to start";
    ClearBackground(WHITE);
    int textWidth = MeasureText(text.data(), 32);
    DrawText("press Space to start", (GetScreenWidth() - textWidth) / 2, GetScreenHeight()/2, 32, Color{0,0,0, static_cast<unsigned char>(128 + 64 * sin(GetTime()*5))});
}

bool IsBirdOnScreen(Bird &bird){
    if (bird.position.y < 0 || bird.position.y > GetScreenHeight()){
        return false;
    }
    return true;
}

void drawBird(Bird &bird){
    if (bird.impact < 0){
        bird.impact = 0;
    }else if(bird.impact > 150){
        bird.impact = 149;
    }

    bird.position.y -= (bird.impact - bird.gravity) * GetFrameTime()*10; 
    
    bird.impact = bird.impact - GetFrameTime()*50;

     if (bird.impact < 0){
        bird.impact = 0;
    }else if(bird.impact > 150){
        bird.impact = 149;
    }

    
    DrawCircle(bird.position.x, bird.position.y, bird.radius, BLACK);
}

void setUpPipe(Pipe& pipe) {
        std::cout<< "setup\n";
        int randomY = heightDist(gen); 
        pipe.width = widthDist(gen);
        pipe.windowBottom = 480+randomY;
        pipe.windowTop = 320+randomY;
        pipe.x = GetScreenWidth()+100;
}

void drawPipe(Pipe& pipe){
    DrawRectangle(pipe.x, 0, pipe.width, pipe.windowTop, BLACK); // Top one 
    DrawRectangle(pipe.x-20, pipe.windowTop-40, pipe.width+40, 40, GRAY);
    
    DrawRectangle(pipe.x, pipe.windowBottom, pipe.width, GetScreenHeight()-pipe.windowBottom, BLACK); // Bottom one
    DrawRectangle(pipe.x-20, pipe.windowBottom, pipe.width+40, 40, GRAY);
    if (pipe.x > -100) {
        pipe.x -= 200 * GetFrameTime();
    }
}

bool checkCollisions(Bird& bird, Pipe& pipe) {
    Vector2 birdCenter = { bird.position.x, bird.position.y};

    Rectangle topPipeBox = { 
        pipe.x, 
        0.0f, 
        pipe.width, 
        pipe.windowTop 
    };

    Rectangle bottomPipeBox = { 
        pipe.x, 
        pipe.windowBottom, 
        pipe.width, 
        2000.0f
    };

    bool hitTop = CheckCollisionCircleRec(birdCenter, bird.radius, topPipeBox);
    bool hitBottom = CheckCollisionCircleRec(birdCenter, bird.radius, bottomPipeBox);

    return hitTop || hitBottom;
}


int main(void) {

    bool InMenu = true;
    float MenuDebounce = 0;
    Bird player;

    float PipeCooldown = 0;
    std::vector<Pipe*> pipes;

    InitWindow(windowSizeX, windowSizeY, "Flappy blob");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            if (InMenu) {
                drawStartMenu();
                if (MenuDebounce <= 0) {
                    
                    if (IsKeyReleased(KEY_SPACE)) {
                        InMenu = false;
                        
                        player.gravity = 115;
                        player.impact = 150;
                        player.jumpForce = 150;
                        player.position = Vector2{static_cast<float>(GetScreenWidth()/4), 400.00};
                        player.radius = 20;

                        for (int i = 0; i < pipes.size(); ++i){
                            delete pipes.at(i);
                        }
                        pipes.clear();
                    }
                }
                else {
                    MenuDebounce -= GetFrameTime();
                }
            } else {
                if (IsKeyPressed(KEY_SPACE)){ 
                    player.impact += player.jumpForce;
                }
                if (!IsBirdOnScreen(player)){
                    InMenu = true;
                    std::cout << "menuon\n";
                    MenuDebounce = 3;
                }
                if (PipeCooldown < 0){
                    
                    PipeCooldown = 2;
                    Pipe* pipe = new Pipe;
                    setUpPipe(*pipe);
                    pipes.push_back(pipe);
                    
                }
                auto i = pipes.begin();
                for (; i != pipes.end();){
                    if((**i).x > -80) {
                        drawPipe(**i);
                        if (checkCollisions(player, **i)) {
                            InMenu = true;
                            std::cout << "menuon\n";
                            MenuDebounce = 3;
                        }
                        ++i;
                    }
                    else{
                        delete *i;
                        i = pipes.erase(i);
                    }
                }

                std::cout << GetFrameTime()<< "\n";
                PipeCooldown -= GetFrameTime();
                drawBird(player);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}