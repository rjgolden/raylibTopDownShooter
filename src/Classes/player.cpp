#include "player.h"

/*-------------------------------*/
Player::Player() : m_beamAnimationX(Assets::defaultBeamX, 8, 0.0f, 0.0f, false), 
                   m_beamAnimationY(Assets::defaultBeamY, 8, 0.0f, 0.0f, false) {

    // std::array derived from animation class
    m_animationTextures[0] = LoadTexture("src/resources/Animations/eyeball-Idle.png"); 
    m_animationTextures[1] = LoadTexture("src/resources/Animations/eyeballMoveLeft.png");
    m_animationTextures[2] = LoadTexture("src/resources/Animations/eyeballMoveRight.png");
    m_animationTextures[3] = LoadTexture("src/resources/Animations/eyeballMoveUp.png");
    m_animationTextures[4] = LoadTexture("src/resources/Animations/eyeball-attackleft.png");
    m_animationTextures[5] = LoadTexture("src/resources/Animations/eyeball-attackRight.png");
    m_animationTextures[6] = LoadTexture("src/resources/Animations/eyeball-attackDown.png");
    m_currentTexture = &m_animationTextures[0]; // default is idle

    // derived from animation
    m_frameCount = 8;
    m_defaultWidth = static_cast<float>(m_animationTextures[0].width) / static_cast<float>(m_frameCount);
    m_defaultHeight = static_cast<float>(m_animationTextures[0].height);

    // rectangles - derived from animation class
    m_animationRect = { 0.0f, 0.0f, m_defaultWidth, m_defaultHeight }; 
    m_hitboxRect = { 0.0f, 0.0f, m_defaultWidth, m_defaultHeight };

    // position - derived from animation class
    m_positionX = static_cast<float>(Global::halfScreenWidth);
    m_positionY = static_cast<float>(Global::halfScreenHeight); 

    m_defaultBeamSizeX = m_beamAnimationX.getDefaultDimensions();
    m_defaultBeamSizeY = m_beamAnimationY.getDefaultDimensions();

    m_defaultBeamSizeX.y -= 20.0f;
    m_defaultBeamSizeY.x -= 20.0f;

}

/*--------SPRITE-METHODS-----------*/
void Player::drawSprite() {
    DrawTexturePro(*m_currentTexture, m_animationRect, {static_cast<float>(m_positionX), static_cast<float>(m_positionY), static_cast<float>(m_animationTextures[0].width) / static_cast<float>(m_frameCount), static_cast<float>(m_animationTextures[0].height)}, {0.0f, 0.0f}, m_rotation, WHITE);
}

void Player::drawHitbox() {
    DrawEllipseLines(m_positionX + 16.5f, m_positionY + 32.0f, 8.0f, 12.0f, RED); 
}

void Player::setState(uint8_t newState){

    m_updateTime = 1.0f / m_animationTime;

    if (newState != m_currentState) {  

        m_currentState = newState;

        switch(m_currentState) {
            case LEFT: m_currentTexture = &m_animationTextures[1]; m_animationTime = 12.0f; break;  // left
            case RIGHT: m_currentTexture = &m_animationTextures[2]; m_animationTime = 12.0f; break;  // right
            case UP: m_currentTexture = &m_animationTextures[3]; m_animationTime = 12.0f; break;  // up
            case DOWN: m_currentTexture = &m_animationTextures[0]; m_animationTime = 12.0f; break;  // down
            case ATTACK_LEFT: m_currentTexture = &m_animationTextures[4]; m_animationTime = 12.0f; break;
            case ATTACK_RIGHT:m_currentTexture = &m_animationTextures[5]; m_animationTime = 12.0f; break;
            case ATTACK_DOWN: m_currentTexture = &m_animationTextures[6]; m_animationTime = 12.0f; break;
            default: m_currentTexture = &m_animationTextures[0]; m_animationTime = 12.0f; break; // idle 
        }
    }

}

/*-------------------------------*/
void Player::handleKeyboardMovement(){
    bool wasIdle = m_idle; 
    //m_direction = 0; uncomment if idle default is wanted
    
    //---------------//
    if (IsKeyDown(KEY_A)) {
        m_idle = false;
        m_state = LEFT; 
        m_positionX -= m_playerSpeed;
    }
    else if (IsKeyDown(KEY_D)){ 
        m_idle = false;
        m_state = RIGHT;
        m_positionX += m_playerSpeed;
    }

    if (IsKeyDown(KEY_W)){ 
        m_idle = false;
        m_state = UP;
        m_positionY -= m_playerSpeed;
    }
    else if (IsKeyDown(KEY_S)){ 
        m_idle = false;
        m_state = IDLE;
        m_positionY += m_playerSpeed;
    }

    // Reset animation if state changed
    if (wasIdle != m_idle) {
        m_currentFrame = 0;
        m_runningTime = 0.0f;
    }
    
}

void Player::handleControllerMovement(){
    
    m_axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    m_axisY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        m_idle = false;
        m_state = LEFT; 
        m_positionX -= m_playerSpeed;
    }
    else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        m_idle = false;
        m_state = RIGHT;
        m_positionX += m_playerSpeed;
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        m_idle = false;
        m_state = UP;
        m_positionY -= m_playerSpeed;
    }
    else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        m_idle = false;
        m_state = IDLE;
        m_positionY += m_playerSpeed;
    }

    //right controller movement
    if (std::fabs(m_axisX) > 0.1f) {
        m_idle = false;
        m_positionX += m_axisX * m_playerSpeed;
            if(m_axisX > 0.3f){
            m_state = RIGHT;
        }
        else if(m_axisX < -0.3f) {
            m_state = LEFT;
        }
        
    }
    if (std::fabs(m_axisY) > 0.1f) {
        m_idle = false;
        m_positionY  += m_axisY * m_playerSpeed;
        if(m_axisY > 0.3f){
            m_state = IDLE;
        }
        else if(m_axisY < -0.3f) {
            m_state = UP;
        }
    }
}

void Player::handleKeyboardDash(){

    if(IsKeyPressed(KEY_SPACE) && !m_isDashing){
        m_isDashing = true;
        m_dashTimer = m_dashTime;
    }

    float deltaTime = GetFrameTime();

    if (m_isDashing) {

        float move = m_dashSpeed * deltaTime;

        // UP & DIAGONAL UPs
        if(IsKeyDown(KEY_W)) m_positionY -= move;
        else if(IsKeyDown(KEY_W) && IsKeyDown(KEY_D)) { m_positionY -= move; m_positionX += move; }
        else if(IsKeyDown(KEY_W) && IsKeyDown(KEY_A)) { m_positionY -= move; m_positionX -= move; }

        // DOWN & DIAGONAL DOWNS
        if(IsKeyDown(KEY_S)) m_positionY += move;
        else if(IsKeyDown(KEY_S) && IsKeyDown(KEY_D)) { m_positionY += move; m_positionX += move; }
        else if(IsKeyDown(KEY_S) && IsKeyDown(KEY_A)) { m_positionY += move; m_positionX -= move; }

        // LEFT & RIGHT
        if(IsKeyDown(KEY_A)) m_positionX -= move;
        if(IsKeyDown(KEY_D)) m_positionX += move;

        m_dashTimer -= deltaTime;
        if (m_dashTimer <= 0.0f) {
            m_isDashing = false;
        }
    }

    m_hitboxRect.x = m_positionX; 
    m_hitboxRect.y = m_positionY;
}

void Player::handleControllerDash(){

    if(IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) && !m_isDashing){
        m_isDashing = true;
        m_dashTimer = m_dashTime;
    }

    float deltaTime = GetFrameTime();

    if (m_isDashing) {

        float move = m_dashSpeed * deltaTime;

        // UP & DIAGONAL UPs
        if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) m_positionY -= move;
        else if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) { m_positionY -= move; m_positionX += move; }
        else if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) { m_positionY -= move; m_positionX -= move; }

        // DOWN & DIAGONAL DOWNS
        if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) m_positionY += move;
        else if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) { m_positionY += move; m_positionX += move; }
        else if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) { m_positionY += move; m_positionX -= move; }

        // LEFT & RIGHT
        if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) m_positionX -= move;
        if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) m_positionX += move;

        // right controller movement
        if (std::fabs(m_axisX) > 0.1f) {
            if(m_axisX > 0.3f){
                m_positionX  +=  move;
            }
            else if(m_axisX < -0.3f) {
                m_positionX  -= move;
            }
            
        }
        if (std::fabs(m_axisY) > 0.1f) {
            if(m_axisY > 0.3f){
                m_positionY  +=  move;
            }
            else if(m_axisY < -0.3f) {
                m_positionY  -= move;
            }
        }


        m_dashTimer -= deltaTime;
        if (m_dashTimer <= 0.0f) {
            m_isDashing = false;
        }
    }

    m_hitboxRect.x = m_positionX; 
    m_hitboxRect.y = m_positionY;
}

void Player::handleKeyboardAttack() {

     //many magic numbers here - will fix in future
    static Music m_attackSound = LoadMusicStream("src/resources/Sounds/laser.wav");
    m_attackSound.looping = true;
    SetMusicVolume(m_attackSound, 0.05f);

    static Vector2 attackLeftPos = {0.0f, 10.0f};
    static Vector2 attackRightPos = {0.0f, 10.0f};
    static Vector2 attackUpPos = {11.0f, 0.0f};
    static Vector2 attackDownPos = {11.0f, 0.0f};
    m_beamAnimationX.setPosition({m_positionX + 16.0f, m_positionY + 32.0f});
    m_beamAnimationY.setPosition({m_positionX + 16.0f, m_positionY + 32.0f});

    if(IsKeyDown(KEY_LEFT)){
        m_idle = false;
        m_state = ATTACK_LEFT; 
        m_beamAnimationX.setPosition(Vector2({m_positionX - 120.0f, m_positionY + 12.0f}));
        m_beamAnimationX.setHitboxOffset(attackLeftPos);
        m_beamAnimationX.setHitboxDimensions(m_defaultBeamSizeX);
        m_beamAnimationX.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else if(IsKeyDown(KEY_RIGHT)){
        m_idle = false;
        m_state = ATTACK_RIGHT;
        m_beamAnimationX.setPosition(Vector2({m_positionX + 24.0f, m_positionY + 12.0f}));
        m_beamAnimationX.setHitboxOffset(attackRightPos);
        m_beamAnimationX.setHitboxDimensions(m_defaultBeamSizeX);
        m_beamAnimationX.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else if(IsKeyDown(KEY_UP)){
        m_idle = false;
        m_state = UP; 
        m_beamAnimationY.setPosition(Vector2({m_positionX, m_positionY - 96.0f}));
        m_beamAnimationY.setHitboxOffset(attackUpPos);
        m_beamAnimationY.setHitboxDimensions(m_defaultBeamSizeY);
        m_beamAnimationY.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else if(IsKeyDown(KEY_DOWN)){
        m_idle = false;
        m_state = ATTACK_DOWN; 
        m_beamAnimationY.setPosition(Vector2({m_positionX, m_positionY + 32.0f}));
        m_beamAnimationY.setHitboxOffset(attackDownPos);
        m_beamAnimationY.setHitboxDimensions(m_defaultBeamSizeY);
        drawSprite(); 
        m_beamAnimationY.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else {
        if (IsMusicStreamPlaying(m_attackSound)) PauseMusicStream(m_attackSound);
        if (m_state == ATTACK_RIGHT) m_state = RIGHT;
        if (m_state == ATTACK_LEFT) m_state = LEFT;
        if (m_state == ATTACK_DOWN) m_state = DOWN;
    }
}

void Player::handleControllerAttack() {
    
    static Music m_attackSound = LoadMusicStream("src/resources/Sounds/laser.wav");
    m_attackSound.looping = true;
    SetMusicVolume(m_attackSound, 0.05f);
    
    static Vector2 attackLeftPos = {0.0f, 10.0f};
    static Vector2 attackRightPos = {0.0f, 10.0f};
    static Vector2 attackUpPos = {11.0f, 0.0f};
    static Vector2 attackDownPos = {11.0f, 0.0f};
    m_beamAnimationX.setPosition({m_positionX + 16.0f, m_positionY + 32.0f});
    m_beamAnimationY.setPosition({m_positionX + 16.0f, m_positionY + 32.0f});

    if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)){
        m_idle = false;
        m_state = ATTACK_LEFT; 
        m_beamAnimationX.setPosition(Vector2({m_positionX - 120.0f, m_positionY + 12.0f}));
        m_beamAnimationX.setHitboxOffset(attackLeftPos);
        m_beamAnimationX.setHitboxDimensions(m_defaultBeamSizeX);
        m_beamAnimationX.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)){
        m_idle = false;
        m_state = ATTACK_RIGHT;
        m_beamAnimationX.setPosition(Vector2({m_positionX + 24.0f, m_positionY + 12.0f}));
        m_beamAnimationX.setHitboxOffset(attackRightPos);
        m_beamAnimationX.setHitboxDimensions(m_defaultBeamSizeX);
        m_beamAnimationX.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)){
        m_idle = false;
        m_state = UP; 
        m_beamAnimationY.setPosition(Vector2({m_positionX, m_positionY - 96.0f}));
        m_beamAnimationY.setHitboxOffset(attackUpPos);
        m_beamAnimationY.setHitboxDimensions(m_defaultBeamSizeY);
        m_beamAnimationY.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else if(IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
        m_idle = false;
        m_state = ATTACK_DOWN; 
        m_beamAnimationY.setPosition(Vector2({m_positionX, m_positionY + 32.0f}));
        m_beamAnimationY.setHitboxOffset(attackDownPos);
        m_beamAnimationY.setHitboxDimensions(m_defaultBeamSizeY);
        drawSprite(); 
        m_beamAnimationY.updateSprite();
        if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
        UpdateMusicStream(m_attackSound); 
    }
    else {
        if (IsMusicStreamPlaying(m_attackSound)) PauseMusicStream(m_attackSound);
        if (m_state == ATTACK_RIGHT) m_state = RIGHT;
        if (m_state == ATTACK_LEFT) m_state = LEFT;
        if (m_state == ATTACK_DOWN) m_state = DOWN;
    }

    //right stick movement - same function as d-pad

    m_axisXR = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
    m_axisYR = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);

    if ((std::fabs(m_axisXR) > 0.1f) || (std::fabs(m_axisYR) > 0.1f)) {
        if(m_axisXR < -0.3f){
            m_idle = false;
            m_state = ATTACK_LEFT; 
            m_beamAnimationX.setPosition(Vector2({m_positionX - 120.0f, m_positionY + 12.0f}));
            m_beamAnimationX.setHitboxOffset(attackLeftPos);
            m_beamAnimationX.setHitboxDimensions(m_defaultBeamSizeX);
            m_beamAnimationX.updateSprite();
            if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
            UpdateMusicStream(m_attackSound); 
        }
        else if(m_axisXR > 0.3f) {
            m_idle = false;
            m_state = ATTACK_RIGHT;
            m_beamAnimationX.setPosition(Vector2({m_positionX + 24.0f, m_positionY + 12.0f}));
            m_beamAnimationX.setHitboxOffset(attackRightPos);
            m_beamAnimationX.setHitboxDimensions(m_defaultBeamSizeX);
            m_beamAnimationX.updateSprite();
            if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
            UpdateMusicStream(m_attackSound); 
        }
        else if(m_axisYR < -0.3f){
            m_idle = false;
            m_state = UP; 
            m_beamAnimationY.setPosition(Vector2({m_positionX, m_positionY - 96.0f}));
            m_beamAnimationY.setHitboxOffset(attackUpPos);
            m_beamAnimationY.setHitboxDimensions(m_defaultBeamSizeY);
            m_beamAnimationY.updateSprite();
            if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
            UpdateMusicStream(m_attackSound); 
        }
        else if(m_axisYR > 0.3f) {
            m_idle = false;
            m_state = ATTACK_DOWN; 
            m_beamAnimationY.setPosition(Vector2({m_positionX, m_positionY + 32.0f}));
            m_beamAnimationY.setHitboxOffset(attackDownPos);
            m_beamAnimationY.setHitboxDimensions(m_defaultBeamSizeY);
            drawSprite(); 
            m_beamAnimationY.updateSprite();
            if (!IsMusicStreamPlaying(m_attackSound)) PlayMusicStream(m_attackSound);
            UpdateMusicStream(m_attackSound);
        }
        else {
            if (IsMusicStreamPlaying(m_attackSound)) PauseMusicStream(m_attackSound);
            if (m_state == ATTACK_RIGHT) m_state = RIGHT;
            if (m_state == ATTACK_LEFT) m_state = LEFT;
            if (m_state == ATTACK_DOWN) m_state = DOWN;
        }
    }
    
   
}

void Player::updateSprite() {

    m_beamAnimationX.setHitboxDimensions({0.0f, 0.0f});
    m_beamAnimationY.setHitboxDimensions({0.0f, 0.0f});

    handleKeyboardMovement();
    handleKeyboardDash();
    handleKeyboardAttack();
    if(IsGamepadAvailable(0)){
        handleControllerMovement();
        handleControllerDash();
        handleControllerAttack();
    }
    setState(m_state);  
    animateSprite();
    if(!(IsKeyDown(KEY_DOWN) && !(m_currentState == ATTACK_DOWN))) {
        drawSprite(); 
    } 
    else {  
        //drawHitbox();
    }

}

/*-------------GETTERS------------*/
float Player::getPlayerSpeed(){
    return m_playerSpeed;
}

Rectangle Player::getBeamAttackRect() {
    if(m_state == UP  || m_state == ATTACK_DOWN) return m_beamAnimationY.getHitboxRect();
    return m_beamAnimationX.getHitboxRect();
}