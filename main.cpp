#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
using namespace std;

// Defining colors using RGBA (Red, Green, Blue, Alpha)
Color skyBlue = {0, 191, 255, 255}; 
Color black = {0, 0, 0, 255}; 
int cellSize = 30;
int cellCount = 25;
double lastUpdateTime = 0; // Timer to control snake movement intervals
int offset = 75; // Margin from window edge to the game board

// Checks if a certain interval has passed to trigger an event (like updating the snake)
bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

// To check if a specific element (Vector2) exists in the deque
bool ElementInDeque(Vector2 element, deque<Vector2> deque){
    for (unsigned int i = 0; i<deque.size(); i++){
        if (Vector2Equals(deque[i], element)){
            return true;
        }
    }
    return false;
}

class Snake{
    public:
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}}; // The body of the snake is stored as a deque of Vector2 positions
    Vector2 direction = {1,0}; // Initial direction
    bool addSegment = false; 

    void Draw(){
        for (unsigned int i = 0; i < body.size(); i++){
            float x = body[i].x;
            float y = body[i].y;
            // Each segment of the snake is a rounded rectangle
            Rectangle segment = Rectangle{offset+x*cellSize, offset+y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.7, 6, black);
        }
    }
    void Update(){ // Move the snake in the current direction and handle growth
        body.push_front(Vector2Add(body[0], direction)); // Add new head based on direction
        if (addSegment == true){ // If the snake ate food, don't remove tail
            addSegment = false; 
        }
        else{
            body.pop_back();
        }
    }
    void reset(){ // Resets the snake to initial state after game over
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1,0};
    }
};

class Food{
    public:
    Vector2 position;
    Texture2D texture;
    Image image;

    Food(deque<Vector2> snakeBody){ // Constructor
        image = LoadImage("Graphics/ball(1).png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }
    ~Food(){ //Destructor
        UnloadTexture(texture);
    }
    void Draw(){ 
        DrawTexture(texture, offset+position.x*cellSize, offset+position.y*cellSize, WHITE);
    }
    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x,y};
    }
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody){ // Generates a position that doesn't overlap with the snake's body
        Vector2 position = GenerateRandomCell();
        while(ElementInDeque(position, snakeBody)){
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game{
    public:
    Snake snake;
    Food food = Food(snake.body); // Food object initialized with snake's body
    bool running = true;
    int score = 0;

    void Draw(){
        food.Draw();
        snake.Draw();
    }
    void Update(){ // Update game logic only when running
        if (running){
            snake.Update();
            eatFood();
            checkCollision();
            collisionTail();
        }
    }
    void eatFood(){
        if (Vector2Equals(snake.body[0], food.position)){
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
        }
    }
    void checkCollision(){ 
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        gameOver();
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        gameOver();
    }
    void gameOver(){
        snake.reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
    }
    void collisionTail(){
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        gameOver();
    }
};

int main () {
    InitWindow(2*offset+cellCount*cellSize, 2*offset+cellCount*cellSize, "SNAKE GAME IN C++"); // Set up game window size
    SetTargetFPS(60);
    Game game;
    
    while(WindowShouldClose() == false){
        BeginDrawing();
        if (eventTriggered(0.15)){ // Updates game state every 0.15 seconds (controls snake speed)
            game.Update();
        }
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1, 0};
            game.running = true;
        }
        // Draw background and game border
        ClearBackground(skyBlue);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellCount*cellSize+10, (float)cellCount*cellSize+10}, 5, black);
        // Draw title and score
        DrawText("Snake Game", offset-5, 20, 40, black);
        DrawText(TextFormat("%i",game.score), offset-5, offset+cellCount*cellSize+10, 40, black);
        // Draw all game elements (snake + food)
        game.Draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}