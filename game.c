#include "raylib.h"
#include<stdio.h>


// Defining Structures For Ball , Paddle and Theme
typedef struct {
    float x, y;
    float speedX, speedY;
    float radius;
} Ball;

typedef struct {
    float x, y;
    float speed;
    float width, height;
} Paddle;

typedef struct {
    Image image;
    Texture2D texture;
} Theme;


// Setting Difficulty Modes
typedef enum { EASY, HARD, INSANE } Difficulty;
Difficulty currentDifficulty = EASY;
bool difficultySelected = false;

// Speed adjustments for different difficulties
float easySpeedMultiplier = 1.0f;
float hardSpeedMultiplier = 1.5f;
float insaneSpeedMultiplier = 2.0f;

// Adding Some additional colors for game usage
Color RAYGOLD = { 255, 203, 0, 255 };
Color Rayyellow = { 255, 255, 0, 255 };

// The target score set by the player
int targetScore = 0; 
int rightPlayerScore = 0;
int leftPlayerScore = 0;

//  Functions Drawing Structures Ball , Paddle , Scores

void DrawBall(Ball* ball) {
    DrawCircle((int)ball->x, (int)ball->y, ball->radius, Rayyellow);
}
void DrawScores(int leftScore, int rightScore) {
    DrawText(TextFormat("Left: %i - Right: %i", leftScore, rightScore), 450, 10, 30, WHITE);
}
void DrawPaddle1(Paddle* paddle, float roundness) {
// Calculate the rounded rectangle parameters
    Rectangle rect = {
        paddle->x - paddle->width / 2,
        paddle->y - paddle->height / 2,
        paddle->width,
        paddle->height
    };

    // Draw the rounded rectangle
    DrawRectangleRounded(rect, roundness, 0, RAYGOLD);
}
void DrawPaddle2(Paddle* paddle, float roundness) {
   // Calculate the rounded rectangle parameters
    Rectangle rect = {
        paddle->x - paddle->width / 2,
        paddle->y - paddle->height / 2,
        paddle->width,
        paddle->height
    };

    // Draw the rounded rectangle
    DrawRectangleRounded(rect, roundness, 0, RED);
}

Rectangle GetPaddleRect(Paddle* paddle) {
    return (Rectangle) { 
        paddle->x - paddle->width / 2, 
        paddle->y - paddle->height / 2,
        paddle->width,
        paddle->height };
}

// Updating Paddle Position
void UpdatePaddle(Paddle* paddle, int upKey, int downKey, float screenHeight) {
    if (IsKeyDown(upKey)) {
        paddle->y -= paddle->speed * GetFrameTime();
        if (paddle->y - paddle->height / 2 < 0) {
            paddle->y = paddle->height / 2;
        }
    }
    if (IsKeyDown(downKey)) {
        paddle->y += paddle->speed * GetFrameTime();
        if (paddle->y + paddle->height / 2 > screenHeight) {
            paddle->y = screenHeight - paddle->height / 2;
        }
    }
}

// Setting Computer Update for Single Player
void AIUpdate(Paddle* paddle, Ball* ball, float screenHeight) {
    if (ball->speedX > 0) {
        if (paddle->y < ball->y) {
            paddle->y += paddle->speed * GetFrameTime();
            if (paddle->y + paddle->height / 2 > screenHeight) {
                paddle->y = screenHeight - paddle->height / 2;
            }
        }
        else if (paddle->y > ball->y) {
            paddle->y -= paddle->speed * GetFrameTime();
            if (paddle->y - paddle->height / 2 < 0) {
                paddle->y = paddle->height / 2;
            }
        }
    }
}

int main() {
    InitWindow(1200, 700, "THE PING PONG CHALLENGE");
    InitAudioDevice();
    SetWindowState(FLAG_VSYNC_HINT);
    // Adding Music for game
    Music music = LoadMusicStream("menu.wav");
    Music game = LoadMusicStream("game.wav");

// Adding Images
    //Menu
    Image menu = LoadImage("start menu.png");
    Texture2D Menu = LoadTextureFromImage(menu);

    Image single_button = LoadImage("sbtn.png");
    Texture2D Single_player = LoadTextureFromImage(single_button);

    Image multi_button = LoadImage("mbtn.png");
    Texture2D multi_player = LoadTextureFromImage(multi_button);
    // Background
    Image background = LoadImage("backimage.png");
    Texture2D Background = LoadTextureFromImage(background);
// Target
    Image target = LoadImage("TARGET.png");
    Texture2D Target = LoadTextureFromImage(target);
// Difficulty
    Image difficulty = LoadImage("difficultyb.png");
    Texture2D Difficulty = LoadTextureFromImage(difficulty);
// Theme
    Image themeb = LoadImage("Theme.png");
    Texture2D Themeb = LoadTextureFromImage(themeb);
// Theme Mars
    Image b7 = LoadImage("b7.png");
    Texture2D pic1 = LoadTextureFromImage(b7);
    float scale = 0.45;
    int newWidth = (int)(b7.width * scale);
    int newHeight = (int)(b7.height * 0.5);
    ImageResize(&b7, newWidth, newHeight);
    Texture2D B2 = LoadTextureFromImage(b7);

//Theme Desert
    Image b5 = LoadImage("b8.png");
    Texture2D pic3 = LoadTextureFromImage(b5);
    newWidth = (int)(b5.width * scale);
    newHeight = (int)(b5.height * 0.5);
    ImageResize(&b5, newWidth, newHeight);
    Texture2D B1 = LoadTextureFromImage(b5);


// Theme Moon
    Image b6 = LoadImage("b6.png");
    Texture2D pic2 = LoadTextureFromImage(b6);
    scale = 0.45;
    newWidth = (int)(b6.width * scale);
    newHeight = (int)(b6.height * 0.5);
    ImageResize(&b6, newWidth, newHeight);
    Texture2D B3 = LoadTextureFromImage(b6);

// Load a sound
    Sound fxWav = LoadSound("buttonplay.wav");
    SetSoundVolume(fxWav, 0.3f);
// INSTUCTIONS
    Image instructions = LoadImage("INSTRUCTIONS.png");
    Texture2D Instructions = LoadTextureFromImage(instructions);

// Theme Setting as per structures
    Theme theme1 = { b7, pic1 };
    Theme theme2 = { b6, pic2 };
    Theme theme3 = { b5, pic3 };
    Theme* currentTheme = &theme1;
 
// End Buttons
    Image end = LoadImage("end b.png");
    Texture2D End = LoadTextureFromImage(end);

// Setting Initial Position for Paddles and Ball
    Ball ball = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 300, 300, 8 };
    Paddle leftPaddle = { 30, GetScreenHeight() / 2, 500, 25, 120 };
    Paddle rightPaddle = { GetScreenWidth() - 30, GetScreenHeight() / 2, 500, 25, 120 };

// Setting Booleans for Each step of the Game
    const char* winnerText = NULL;
    bool singlePlayer = false;
    bool gameStarted = false;
    bool settingTargetScore = false;
    bool themeSelected = false;
    bool isGameReady = false;
    bool showPostGameMenu = false;
    Rectangle playAgainBtn = { 430, 340, 340, 85 };
    Rectangle mainMenuBtn = { 430, 510, 360, 85 };

// Button dimensions
    Rectangle singlePlayerBtn = { 420, 400, 300, 80 };
    Rectangle multiPlayerBtn = { 420, 550, 300, 80 };
    Rectangle theme1Btn = { 60, 480, 200, 50 };
    Rectangle theme2Btn = { 480, 480, 200, 50 };
    Rectangle theme3Btn = { 880, 480, 200, 50 };
    float roundness = 0.5f;

// Playing the Loaded Music
    PlayMusicStream(music);
    PlayMusicStream(game);
    int speedx = 0;
    int speedy = 0;
    while (!WindowShouldClose()) {
 // Menu to select game mode
        if (!gameStarted) {
            Vector2 mousepoint = GetMousePosition();
            UpdateMusicStream(music);
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(Menu, 0,0, WHITE);
            DrawTexture(multi_player, 0, 0, WHITE);
            DrawTexture(Single_player, 0, 0, WHITE);           
            EndDrawing();

            // Check for button clicks
            if (CheckCollisionPointRec(GetMousePosition(), singlePlayerBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);           
                singlePlayer = true;
                gameStarted = true;
                
            }
            if (CheckCollisionPointRec(GetMousePosition(), multiPlayerBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);
                singlePlayer = false;
                gameStarted = true;

            }
        }
        else if (!settingTargetScore && !themeSelected && !isGameReady) {
            // New screen for target score input
            UpdateMusicStream(music);
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(Target, 0, 0, WHITE);
            //DrawText("Set Target Score:", 340, 40, 60, YELLOW);
            DrawRectangle(520, 310, 170, 80, BLUE);
            //DrawText("UP[+]", 540, 200, 30, YELLOW);
            DrawText(TextFormat("%i", targetScore), 570, 320, 70, WHITE);
            //DrawText("DOWN[-]", 520, 310, 30, RED);
            //DrawText("Press [ENTER] when done", 390, 450, 35, GREEN);
            EndDrawing();

            // Input logic
            if (IsKeyPressed(KEY_UP)) {
                PlaySound(fxWav);
                targetScore++; // Increment target score
            }
            if (IsKeyPressed(KEY_DOWN) && targetScore > 0) {
                PlaySound(fxWav);
                targetScore--; // Decrement target score
            }
            if (IsKeyPressed(KEY_ENTER)) {
                PlaySound(fxWav);
                settingTargetScore = true; // Confirm target score and proceed to game
            }
        }
        else if (settingTargetScore && !themeSelected && !isGameReady) {
            // New Screen for Theme selection
            BeginDrawing();
            UpdateMusicStream(music);
            ClearBackground(BLACK);
            DrawTexture(Themeb, 0, 0, WHITE);
            DrawTexture(B2, 0, 200, WHITE);
            DrawTexture(B3, 390, 200, WHITE);
            DrawTexture(B1, 800, 200, WHITE);

            //DrawText("SELECT THEME:", 310, 50, 80, YELLOW);
            Color darkOrange = { 204, 102, 0, 255 };
            DrawRectangleRec(theme1Btn, darkOrange);
            DrawRectangleRec(theme2Btn, DARKPURPLE);
            DrawRectangleRec(theme3Btn, DARKBLUE);
            Color lightBlue = { 173, 216, 230, 255 };

            DrawText("MARS", theme1Btn.x + 50, theme1Btn.y + 15, 30, YELLOW);
            DrawText("MOON", theme2Btn.x + 60, theme2Btn.y + 15, 30, lightBlue);
            DrawText("DESERT", theme3Btn.x + 40, theme2Btn.y + 15, 30, PURPLE);
            EndDrawing();
            // Selecting Theme Mars
            if (CheckCollisionPointRec(GetMousePosition(), theme1Btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);
                currentTheme = &theme1;
                themeSelected = true;
            }
            // Selecting Theme Moon
            if (CheckCollisionPointRec(GetMousePosition(), theme2Btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);
                currentTheme = &theme2;
                themeSelected = true;
            }
            // Selecting Theme Desert
            if (CheckCollisionPointRec(GetMousePosition(), theme3Btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);
                currentTheme = &theme3;
                themeSelected = true;
            }
        }


        else if (settingTargetScore && themeSelected && !difficultySelected) {
            // New screen for difficulty selection
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(Difficulty, 0, 0, WHITE);
            UpdateMusicStream(music);
           

            // Define buttons for difficulty levels
            Rectangle easyBtn = { 400, 230, 400, 90 };
            Rectangle hardBtn = { 400, 400, 400, 90 };
            Rectangle insaneBtn = { 400, 560, 400, 90 };

            EndDrawing();

            // Check for button clicks for difficulty selection
            if (CheckCollisionPointRec(GetMousePosition(), easyBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);
                currentDifficulty = EASY;
                difficultySelected = true;
            }
            if (CheckCollisionPointRec(GetMousePosition(), hardBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);
                currentDifficulty = HARD;
                difficultySelected = true;
            }
            if (CheckCollisionPointRec(GetMousePosition(), insaneBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                PlaySound(fxWav);
                currentDifficulty = INSANE;
                difficultySelected = true;
            }
        }

        else if (settingTargetScore && themeSelected && !isGameReady) {
            // New: Show the selected theme screen with an option to start the game
            switch (currentDifficulty) {
            case EASY:
                ball.speedX =speedx= 300 * easySpeedMultiplier;
                ball.speedY =speedy= 300 * easySpeedMultiplier;
                rightPaddle.speed = 500 * easySpeedMultiplier;               
                leftPaddle.speed = 500 * easySpeedMultiplier;
                
                break;
            case HARD:
                ball.speedX =speedx= 300 * hardSpeedMultiplier;
                ball.speedY =speedy= 300 * hardSpeedMultiplier;
                rightPaddle.speed = 500 * hardSpeedMultiplier;               
                leftPaddle.speed = 500 * hardSpeedMultiplier;
                
                break;
            case INSANE:
                ball.speedX =speedx= 300 * insaneSpeedMultiplier;
                ball.speedY =speedy=300 * insaneSpeedMultiplier;
                rightPaddle.speed = 500 * insaneSpeedMultiplier;             
                leftPaddle.speed = 500 * insaneSpeedMultiplier;
                
                break;
            }
            UpdateMusicStream(game);
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(currentTheme->texture, 0, 0, WHITE);
        //ENTER
            DrawTexture(Instructions, 0, 0, WHITE);
            EndDrawing();

            // Wait for the player to press [ENTER] to start the game
            if (IsKeyPressed(KEY_ENTER)) {
                isGameReady = true; // Set the flag to true to indicate the game should now start
            }
        }

        else if (settingTargetScore && themeSelected && isGameReady) {
            UpdateMusicStream(game);
            ball.x += (ball.speedX * GetFrameTime());
            ball.y += ball.speedY * GetFrameTime();

            if (ball.y - ball.radius < 0 || ball.y + ball.radius > GetScreenHeight()) {
                ball.speedY *= -1;
                if (ball.y - ball.radius < 0) ball.y = ball.radius;
                if (ball.y + ball.radius > GetScreenHeight()) ball.y = GetScreenHeight() - ball.radius;
            }

            if (singlePlayer) {
                AIUpdate(&rightPaddle, &ball, GetScreenHeight());
            }
            else {
                UpdatePaddle(&rightPaddle, KEY_UP, KEY_DOWN, GetScreenHeight());
            }
            UpdatePaddle(&leftPaddle, KEY_W, KEY_S, GetScreenHeight());
            if (CheckCollisionCircleRec((Vector2) { ball.x, ball.y }, ball.radius, GetPaddleRect(&leftPaddle)) && ball.speedX < 0) {
                ball.speedX *= -1.1f;
                ball.speedY = (ball.y - leftPaddle.y) / (leftPaddle.height / 2) * ball.speedX;
            }
            if (CheckCollisionCircleRec((Vector2) { ball.x, ball.y }, ball.radius, GetPaddleRect(&rightPaddle)) && ball.speedX > 0) {
                ball.speedX *= -1.1f;
                ball.speedY = (ball.y - rightPaddle.y) / (rightPaddle.height / 2) * -ball.speedX;
            }
            if (ball.x < 0) {
                ball.speedX = 0;
                ball.speedY = 0;
                if (IsKeyPressed(KEY_SPACE)) {
                    rightPlayerScore = rightPlayerScore + 1;
                    ball.x = GetScreenWidth() / 2;
                    ball.y = GetScreenHeight() / 2;
                    ball.speedX = speedx;
                    ball.speedY = speedy;
                    if (rightPlayerScore >= targetScore) {
                        winnerText = "Right Player Wins!";

                    }
                }
            }

            if (ball.x > GetScreenWidth()) {
                ball.speedX = 0;
                ball.speedY = 0;
                if (IsKeyPressed(KEY_SPACE)) {
                    ball.x = GetScreenWidth() / 2;
                    ball.y = GetScreenHeight() / 2;
                    ball.speedX = speedx;
                    ball.speedY = speedy;
                    leftPlayerScore = leftPlayerScore + 1;
                    if (leftPlayerScore >= targetScore) {
                        winnerText = "left Player Wins!";

                    }
                }
            }



            BeginDrawing();

            DrawTexture(currentTheme->texture, 0, 0, WHITE);
            DrawScores(leftPlayerScore, rightPlayerScore);
            if (!winnerText) {
                DrawBall(&ball);
            }
            DrawPaddle1(&leftPaddle, 0.7);
            DrawPaddle2(&rightPaddle, 0.7);
            DrawText(winnerText, GetScreenWidth() / 2 - MeasureText(winnerText, 60) / 2, 240, 60, YELLOW);
            if (winnerText) {
            DrawTexture(End,0,0,WHITE); // Draw post-game menu buttons
                //DrawRectangleRounded(playAgainBtn, roundness, 0, GREEN);
                //DrawText("Play Again", playAgainBtn.x + 40, playAgainBtn.y + 15, 30, WHITE);
                //DrawRectangleRounded(mainMenuBtn, roundness, 0, RED);
                //DrawText("Main Menu", mainMenuBtn.x + 40, mainMenuBtn.y + 15, 30, WHITE);
            }
            DrawFPS(10, 10);
            EndDrawing();



            // Check for button clicks 
            if (CheckCollisionPointRec(GetMousePosition(), playAgainBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Reset game state for new game
                rightPlayerScore = 0;
                leftPlayerScore = 0;
                ball.x = GetScreenWidth() / 2;
                ball.y = GetScreenHeight() / 2;
                ball.speedX = speedx;
                ball.speedY = speedy;
                winnerText = NULL;
                showPostGameMenu = false;
                gameStarted = true; // or false, depending on your game logic
            }
            if (CheckCollisionPointRec(GetMousePosition(), mainMenuBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Return to main menu
                gameStarted = false;
                settingTargetScore = false;
                winnerText = false;
                rightPlayerScore = 0;
                leftPlayerScore = 0;
                themeSelected = false;
                difficultySelected = false;
                isGameReady = false;
                showPostGameMenu = false;
            }
        }
        
    }

    UnloadTexture(Menu);
    UnloadTexture(Difficulty);
    UnloadTexture(Background);
    UnloadTexture(Target);
    UnloadTexture(Themeb);
    UnloadTexture(Instructions);
    UnloadTexture(End);
    UnloadTexture(Single_player);
    UnloadTexture(multi_player);
    UnloadTexture(pic3);
    UnloadTexture(pic1);
    UnloadTexture(pic2);
    UnloadImage(menu);
    UnloadImage(b5);
    UnloadImage(b6);
    UnloadImage(b7);
    UnloadImage(themeb);
    UnloadImage(background);
    UnloadImage(instructions);
    UnloadImage(end);
    UnloadImage(multi_button);
    UnloadImage(single_button);
    UnloadImage(target);
    UnloadImage(difficulty);
    UnloadSound(fxWav);
    UnloadMusicStream(music);
    UnloadMusicStream(game);
    CloseAudioDevice();
    CloseWindow();
    return 0;

}







