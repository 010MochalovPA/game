#include "Level.h"
#include "Player.h"
#include "Enemy.h"

namespace DefaultEnemyConstants
{
    constexpr float MAX_SPEED = 80.f;
    constexpr float ACCELERATION = 7.f;
    constexpr int HEALTH = 100;
    constexpr int COOLDOWN_TIME = 1.5f;
    constexpr int VISION_DISTANCE_X = 150.0f;
    constexpr int VISION_DISTANCE_Y = 50.0f;
    constexpr int STOP_DISTANCE = 20.0f;
}

Enemy::Enemy(Level& level, float x, float y, float patrolDistance) : m_level(&level), m_healthBar()
{
    if (!m_font.loadFromFile("./assets/arial.ttf"))
    {
        std::cerr << "Failed to load font!" << std::endl;
    }

	m_position = { x, y };
    m_texture.loadFromFile("./assets/sprite-sheet-enemy.png");
    m_animationManager = AnimationManager();
    m_animationManager.create("run", m_texture, 6, 0, 20, 32, 8, 14.f, 32);
    m_animationManager.create("stay", m_texture, 6, 32, 20, 32, 6, 10.f, 32);
    m_animationManager.create("attack", m_texture, 6, 64, 32, 32, 1, 1.f, 32);
    m_state = State::STAY;
    m_maxSpeed = DefaultEnemyConstants::MAX_SPEED;
    m_acceleration = DefaultEnemyConstants::ACCELERATION;
    m_patrolDistance = patrolDistance;
    m_patrolStartX = x - patrolDistance / 2;
    m_patrolEndX = x + patrolDistance / 2;
    m_isPatrolling = true;
    m_health = DefaultEnemyConstants::HEALTH;
    m_isOnCooldown = false;
    m_cooldownTime = DefaultEnemyConstants::COOLDOWN_TIME;
    m_currentCooldownTime = 0.0f;
    m_isDead = false;
    m_attackCooldown = sf::seconds(1.5f);
}

void Enemy::updateDamageTexts(float time)
{
    for (auto& damageText : m_damageTexts)
    {
        damageText.text.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(
            damageText.initialAlpha * (damageText.duration / 1.0f))));

        damageText.text.move(0, -50 * time);
        damageText.duration -= time;

        if (damageText.duration <= 0.0f)
        {
            m_damageTexts.erase(std::remove_if(m_damageTexts.begin(), m_damageTexts.end(),
                [](const DamageText& dt) { return dt.duration <= 0.0f; }),
                m_damageTexts.end());
        }
    }
}

bool Enemy::collisionX(float velocityX)
{
    std::vector<Object> objects = m_level->GetObjects("enemy_solid");

    for (const auto& object : objects)
    {
        sf::FloatRect enemyRect(m_position.x + velocityX, m_position.y, m_width, m_height);
        sf::FloatRect obstacleRect(object.rect.left, object.rect.top, object.rect.width, object.rect.height);

        if (enemyRect.intersects(obstacleRect))
        {
            m_velocity.x = 0;
            return true;
        }
    }
    return false;
}

void Enemy::update(float time)
{
    updateDamageTexts(time);

    if (m_isOnCooldown)
    {
        m_currentCooldownTime -= time;

        if (m_currentCooldownTime <= 0.0f)
        {
            m_isOnCooldown = false;
        }
        else
        {
            m_animationManager.set("stay");
            m_animationManager.flip(m_direction == Direction::LEFT);
            m_animationManager.tick(time);
            return;
        }
    }

    m_animationManager.flip(m_direction == Direction::LEFT);

    if (m_isPatrolling)
    {
        m_direction = Direction::RIGHT;
        m_state = State::RUN;
        m_isPatrolling = false;
    }
    
    sf::Vector2f playerPosition = m_level->getPlayerPosition();
    sf::Vector2f directionToPlayer = playerPosition - m_position;
    float distanceToPlayer = std::sqrt(directionToPlayer.x * directionToPlayer.x + directionToPlayer.y * directionToPlayer.y);

    m_velocity.x = (m_direction == Direction::LEFT) ? -m_maxSpeed : m_maxSpeed;
    m_velocity.x += m_acceleration * (m_direction == Direction::LEFT ? -1.f : 1.f);
    m_state = State::RUN;
    
    m_attackTimer += time;
    float attackInterval = 1.2f;

    if (std::abs(directionToPlayer.x) < DefaultEnemyConstants::VISION_DISTANCE_X 
        && std::abs(directionToPlayer.y) < DefaultEnemyConstants::VISION_DISTANCE_Y)
    {
        m_direction = (playerPosition.x < m_position.x) ? Direction::LEFT : Direction::RIGHT;

        if (distanceToPlayer < DefaultEnemyConstants::STOP_DISTANCE)
        {
            m_velocity.x = 0.0f;
            m_state = State::STAY;
        }

        if (m_attackTimer >= attackInterval)
        {
            meleeAttack();
            attackTimer = 0.0f;
        }
    }
    else
    {
        if (m_position.x < m_patrolStartX || m_position.x > m_patrolEndX)
        {
            m_direction = (m_position.x < m_patrolStartX) ? Direction::RIGHT : Direction::LEFT;
        }
    }

    checkCollision(m_velocity.x * time);
    setAnimation();
    m_animationManager.tick(time);
}

void Enemy::checkCollision(float velocityX)
{
    if (!collisionX(velocityX))
    {
        m_position.x += velocityX;
        m_state = State::RUN;
    }
    else
    {
        m_state = State::STAY;
    }
}

void Enemy::setAnimation()
{
    if (m_state == State::STAY)
    {
        m_animationManager.set("stay");
    }

    if (m_state == State::RUN)
    {
        m_animationManager.set("run");
    }
}

void Enemy::draw(sf::RenderWindow& window)
{
	m_healthBar.draw(window, m_position.x, m_position.y);
	m_animationManager.draw(window, m_position.x, m_position.y);

    for (const auto& damageText : m_damageTexts)
    {
        window.draw(damageText.text);
    }
}

bool Enemy::isDead()
{
    return m_isDead;
}

void Enemy::takeDamage(int damage, bool damageFromRight)
{
    m_health = m_health - damage;
    m_healthBar.setHealth(m_health);

    float knockbackDistance = 10.0f;
    float newX = m_position.x;

    if (!collisionX((damageFromRight) ? knockbackDistance : -knockbackDistance))
    {
        newX += (damageFromRight) ? knockbackDistance : -knockbackDistance;
    }

    m_position.x = newX;
    m_currentCooldownTime = 1.0f;
    m_isOnCooldown = true;

    DamageText damageText = createDamageText(damage);

    m_damageTexts.push_back(damageText);

    if (m_health <= 0)
    {
        m_isDead = true;
        m_level->increaseScore(10);
    }
}

DamageText Enemy::createDamageText(int damage)
{
    DamageText damageText;
    damageText.text.setFont(m_font);
    damageText.text.setCharacterSize(12);
    damageText.text.setFillColor(sf::Color(255, 255, 255, 255));
    damageText.text.setString(std::to_string(damage));
    damageText.text.setPosition(m_position.x, m_position.y - 20);
    damageText.duration = 0.5f;
    damageText.initialAlpha = 255;

    return damageText
}

float Enemy::getX()
{
    return m_position.x;
}

float Enemy::getY()
{
    return m_position.y;
}

float Enemy::getWidth()
{
    return m_width;
}

float Enemy::getHeight()
{
    return m_height;
}

void Enemy::meleeAttack()
{
    m_state = State::ATTACK;
    sf::FloatRect attackArea;
    attackArea.left = m_position.x + (m_direction == Direction::LEFT ? -15.f : 15.f);
    attackArea.top = m_position.y + 8;
    attackArea.width = 18;
    attackArea.height = 16;

    sf::FloatRect playerRect = m_level->GetPlayerRect();
    if (attackArea.intersects(playerRect)) {
        m_level->GetPlayer()->takeDamage(15 + rand() % 10, m_direction == Direction::RIGHT);
    }
}