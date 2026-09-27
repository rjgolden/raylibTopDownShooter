#include "enemy.h"

/*-------------------------------*/
Enemy::Enemy(Texture2D texture, uint8_t frameCount){

    // load textures
    m_texture = texture; 
    m_enemyHurt = Assets::enemyHurt; 

    m_defaultWidth = static_cast<float>(m_texture.width) / static_cast<float>(frameCount);
    m_defaultHeight = static_cast<float>(m_texture.height);

    // rectangles 
    m_animationRect = { 0.0f, 0.0f, m_defaultWidth, m_defaultHeight }; 
    m_hitboxRect = { 0.0f, 0.0f, m_defaultWidth, m_defaultHeight };
    m_healthBarRect = { 0.0f, 0.0f, m_defaultWidth, 10.0f };

    m_frameCount = frameCount;
    
    // position and speed - dervied from animation class
    m_positionX = GetRandomValue(0, Global::screenWidth); 
    m_positionY = GetRandomValue(0, Global::screenHeight); 

}

Enemy::Enemy() {}

/*-------------------------------*/
void Enemy::chasePlayer(Vector2 position){
    float dx = position.x - m_positionX;
    float dy = position.y - m_positionY;
    float length = std::sqrt(dx*dx + dy*dy);
    if (length > 0) {
        dx /= length;
        dy /= length;
    }
    m_positionX += dx * m_enemySpeed;
    m_positionY += dy * m_enemySpeed;
}

void Enemy::takeDamage(float damage) {
    m_hurtFrameActive = true; // activate hurt frame
    m_enemyHealth -= damage;
    if (m_enemyHealth < 0.0f) {
        m_enemyHealth = 0.0f; // prevent negative health
    }
}

void Enemy::drawHealthBar(){
    // draw background
    DrawRectangle(m_healthBarRect.x, m_healthBarRect.y, m_healthBarRect.width, m_healthBarRect.height, GRAY);
    
    // calculate health bar width based on current health
    float healthWidth = (m_enemyHealth / 100.0f) * m_healthBarRect.width;
    
    // draw health bar
    DrawRectangle(m_healthBarRect.x, m_healthBarRect.y, healthWidth, m_healthBarRect.height, RED);
    DrawRectangleLines(m_healthBarRect.x, m_healthBarRect.y, m_healthBarRect.width, m_healthBarRect.height, BLACK);
    
}

void Enemy::drawHurtFrame() {
    DrawTexture(m_enemyHurt, m_positionX, m_positionY, WHITE);
}

void Enemy::drawCircle(){
    DrawCircleLines(m_positionX + 16.0f, m_positionY + 16.0f, circleRadius, RED);
}

void Enemy::updateSprite(){
    m_healthBarRect.x = m_positionX; 
    m_healthBarRect.y = m_positionY - 13.0f;
    circlePos.x = m_positionX + 16.0f;
    circlePos.y= m_positionY + 16.0f;
    if(m_hurtFrameActive) {
        drawHurtFrame();
        m_hurtFrameActive = false; // reset hurt frame after drawing
    }
    else{
        animateSprite();
        drawSprite();
        //drawCircle();
    }
    drawHealthBar();
}
    
/*-------------------------------*/
float Enemy::getHealth() {
    return m_enemyHealth;
}  

Vector2 Enemy::getCirclePos() {
    return circlePos;
}  

 void Enemy::setPositionX(float pos, float deltaTime = 0.0f){
    if(deltaTime > 0.0f) m_positionX += pos * deltaTime; 
    else m_positionX = pos;
 };

  void Enemy::setPositionY(float pos, float deltaTime = 0.0f){
    if(deltaTime > 0.0f) m_positionY += pos * deltaTime; 
    else m_positionY = pos;
  };

void Enemy::setHealth(float health) {
    m_enemyHealth = health;
}

void Enemy::setEnemySpeed(float speed){
    m_enemySpeed = speed;
}

void Enemy::setPositionRandom(){
    m_positionX = static_cast<float>(GetRandomValue(0, Global::screenWidth)) + Global::screenWidth;
    m_positionY = static_cast<float>(GetRandomValue(0, Global::screenHeight)) + Global::screenHeight;
}

