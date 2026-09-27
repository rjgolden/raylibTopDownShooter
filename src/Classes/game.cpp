#include "game.h"
#include "particles.h"
#include <iostream>
#include <vector>

void Game::drawLight(Vector2 position, float radius, Color color)
{
    DrawCircleGradient(
        static_cast<int>(position.x) + 16,
        static_cast<int>(position.y) + 32,
        radius,
        color,
        BLANK
    );
}

void Game::checkEnemyCollisions(std::array<Enemy, 10>& enemies, float deltaTime) {
    // Outer loop selects the first enemy (i)
    for (int i = 0; i < 10; i++) {
        // Inner loop selects the second enemy (j), starting right after 'i'
        for (int j = i + 1; j < 10; j++) {
            
            // Check collision using raylib's circle-to-circle function
            if (CheckCollisionCircles(enemies[i].getCirclePos(), 20.0f, 
                                      enemies[j].getCirclePos(), 20.0f)) {
                
                enemies[i].setPositionX(enemies[i].getPositionX() * -.05f, deltaTime);
                enemies[i].setPositionY(enemies[i].getPositionY() * -.05f, deltaTime);
               
            }
        }
    }
}

void Game::runGame(){

    // setup
    Utilities::init();
    Assets::loadAssets();

    // animations
    Animation fireAnimation(Assets::fireAnimation, 6, m_centerX, m_centerY, true);
    Animation coinAnimation(Assets::coinAnimation, 8, m_centerX, m_centerY, false);

    // player
    Player playerAnimation;
    
    // enemies
    std::array<Enemy, 10> enemies;
    for(int i{0}; i<10; i++){
        enemies[i] = (Enemy(Assets::enemy, 6));
    }


    // camera
    GameCamera gameCamera;
    gameCamera.camera.zoom = 1.0f;
    gameCamera.camera.offset = {Global::halfScreenWidth, Global::halfScreenHeight};

    // sound stuff
    SoundSystem initSoundSystem;  
    Sound coin = LoadSound("src/resources/Sounds/coin.mp3");

    // music
    Music music = LoadMusicStream("src/resources/Sounds/fireSound.mp3");
    PlayMusicStream(music);
    SetMusicVolume(music, 0.01f);
    music.looping = true;

    // particles
    Particles particles;

    // temp
    int enemiesKilled{0};

    // light map
    RenderTexture2D lightMap = LoadRenderTexture(Global::screenWidth, Global::screenHeight);
    SetTextureFilter(lightMap.texture, TEXTURE_FILTER_POINT);
    RenderTexture2D target = LoadRenderTexture(Global::screenWidth, Global::screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); 

    while (!WindowShouldClose())
    {   

        UpdateMusicStream(music);
        m_deltaTime = GetFrameTime();

        // sets fullscreen
        if(IsKeyPressed(KEY_F)) Utilities::toggleFullscreenWindow();
        
        // moves camera to origin
        if(IsKeyDown(KEY_C)) {
            gameCamera.camera.target.x += (m_centerX - gameCamera.camera.target.x) * (Global::cameraLerp * m_deltaTime);
            gameCamera.camera.target.y += (m_centerY - gameCamera.camera.target.y) * (Global::cameraLerp * m_deltaTime);
        }
        else{
            gameCamera.camera.target.x += ((playerAnimation.getPositionX() + 16.0f) - gameCamera.camera.target.x) * (playerAnimation.getPlayerSpeed() * m_deltaTime);
            gameCamera.camera.target.y += ((playerAnimation.getPositionY() + 32.0f) - gameCamera.camera.target.y) * (playerAnimation.getPlayerSpeed() * m_deltaTime);
        }

        // places coin
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 worldPos = GetScreenToWorld2D({static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY())}, gameCamera.camera);
            Vector2 position = { worldPos.x - fireAnimation.getWidth()/2.0f, worldPos.y - fireAnimation.getHeight()/2.0f };
            coinAnimation.setPosition(position);
            PlaySound(coin);
        }

        // enemy logic
        for(Enemy &enemy : enemies){
            enemy.setEnemySpeed(2.0f);
            if(CheckCollisionRecs(playerAnimation.getBeamAttackRect(), enemy.getHitboxRect())) {
                enemy.takeDamage(0.25f); 
                enemy.setEnemySpeed(0.5f);
            }
            if(enemy.getHealth() <= 0.0f){
                enemiesKilled++;
                particles.explode(enemy.getPosition(), RED, rand() % particles.m_particleTextures.size());
                enemy.setHealth(100.0f); 
                enemy.setPositionRandom();
            }
            checkEnemyCollisions(enemies, m_deltaTime);
        }
        
        // where light is drawn to screen
        BeginTextureMode(lightMap);  
            ClearBackground(GRAY); // screen tint (black for complete dark, etc.)
            BeginBlendMode(BLEND_ADDITIVE); 
                Vector2 screenPos = GetWorldToScreen2D(playerAnimation.getPosition(), gameCamera.camera);
                drawLight(screenPos, 150, Color{255, 240, 200, 255});
                Vector2 screenPos2 = GetWorldToScreen2D(fireAnimation.getPosition(), gameCamera.camera);
                float flicker = 140.0 + sin(GetTime() * 10.0) * 10.0;
                drawLight(screenPos2, flicker, ORANGE);
            EndBlendMode();
        EndTextureMode(); 

        // virtual screen
        BeginTextureMode(target);  
            ClearBackground(BLACK);
            // 2D Mode - where game is drawn to virtual screen   
            BeginMode2D(gameCamera.camera); 
                Utilities::drawBackground();
                fireAnimation.updateSprite();
                coinAnimation.updateSprite();
                playerAnimation.updateSprite();
                for(Enemy &enemy : enemies){
                    enemy.updateSprite();
                    //enemy.chasePlayer(playerAnimation.getPosition());
                }
                particles.updateParticles();
            EndMode2D();

            BeginBlendMode(BLEND_MULTIPLIED); 
                DrawTextureRec(lightMap.texture, Rectangle{0.0f, 0.0f, static_cast<float>(lightMap.texture.width), -static_cast<float>(lightMap.texture.height)}, Vector2{0, 0}, RAYWHITE);
            EndBlendMode(); 
        EndTextureMode();

        BeginDrawing();
            
            ClearBackground(BLACK);
            m_scale = std::max(1.0f, std::min(static_cast<float>(GetRenderWidth())  / static_cast<float>(Global::screenWidth),
                                              static_cast<float>(GetRenderHeight()) / static_cast<float>(Global::screenHeight)));           
            float offsetX = (static_cast<float>(GetRenderWidth()) - static_cast<float>(Global::screenWidth) * m_scale) / 2.0f;
            float offsetY = (static_cast<float>(GetRenderHeight()) - static_cast<float>(Global::screenHeight) * m_scale) / 2.0f;
            
            DrawTexturePro(
                target.texture,
                { 0, 0, static_cast<float>(target.texture.width), -static_cast<float>(target.texture.height) },
                { offsetX, offsetY, static_cast<float>(Global::screenWidth) * m_scale, static_cast<float>(Global::screenHeight) * m_scale },
                { 0, 0 },
                0.0f,
                WHITE
            );
            // UI (temp)
            std::string enemiesKilledStr = std::to_string(enemiesKilled);
            const char* enemiesKilledCStr = enemiesKilledStr.c_str();
            DrawText("Enemies Killed: ", 20 * m_scale, 20, 20 * m_scale, YELLOW);
            DrawText(enemiesKilledCStr, 180 * m_scale, 20, 20 * m_scale, YELLOW);
        EndDrawing();  
    }  
    CloseWindow();
}