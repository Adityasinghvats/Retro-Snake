// SnakeGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <deque>
#include <iostream>
#include <raylib.h>
#include<raymath.h>
using namespace std;

Color green = Color{ 173, 204, 96, 255 };
Color darkGreen = Color{ 43, 51, 24, 255 };

float cellSize = 30;
float cellcount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool elementInDeque(Vector2 element, deque<Vector2> body)
{
    for (int i = 0;i < body.size();i++)
    {
        if (Vector2Equals(body[i], element))
        {
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake {
public:
    deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1,0 };
   
    bool addSegment = false;

    void Draw()
    {
        for (int i = 0;i < body.size();i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, cellSize, cellSize };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));

        if (addSegment == true) 
        {
            addSegment = false;
        }
        else {
            body.pop_back();
        }
    }
    void Reset()
    {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = { 1,0 };
    }
};

class Food {
public:
    Texture2D texture;
    Vector2 position;

    Food(deque<Vector2> snakeBody) 
    {
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void Draw()
    {
        //DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, darkGreen);
        DrawTexture(texture,offset + position.x * cellSize,offset + position.y * cellSize,WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellcount - 1);
        float y = GetRandomValue(0, cellcount - 1);
        return Vector2{ x,y };
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) 
    {
        Vector2 pos = GenerateRandomCell();
        while (elementInDeque(pos, snakeBody))
        {
            pos = GenerateRandomCell();
        }
        return pos;
    }
};

class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw() {
        food.Draw();
        snake.Draw();
    }

    void Update() {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }
    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellcount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellcount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }
    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (elementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

};

int main()
{

    InitWindow(2*offset + cellSize * cellcount, 2*offset + cellSize * cellcount, "Retro Snake");
    SetTargetFPS(60);

    Game game = Game();

    //GameLoop
    while (WindowShouldClose() == false) 
    {
        BeginDrawing();

        if (eventTriggered(0.2)) {
            //update only after 200ms not 60 times per/second
            game.Update();

        }
        //move up and check to make sure cannot change direction immediatly
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
            game.snake.direction = { 0, -1 };
            game.running = true;
        }
        //move down
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = { 0, 1 };
            game.running = true;

        }
        //move left
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = { -1, 0 };
            game.running = true;

        }
        //move left
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
            game.snake.direction = { 1, 0 };
            game.running = true;

        }

        //Drawing 
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellcount + 10, (float)cellSize * cellcount + 10 }, 5, darkGreen);
        DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellcount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
