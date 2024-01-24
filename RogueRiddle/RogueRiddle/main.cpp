#include <SFML/Graphics.hpp>
#include "AnimationManager.h"
#include "Player.h"
#include "Level.h"
#include <iostream>
#include "Enemy.h"

using namespace sf;
void mainGame(sf::Clock& clock, float accumulatedTime, float fixedTimeStep,
    sf::RenderWindow& window, std::shared_ptr<Player> player, std::vector<std::shared_ptr<Enemy>> enemies, Level& level);
void endGame(sf::RenderWindow& window, std::shared_ptr<Player> player, const Level& level, sf::Text& scoreText);

int main()
{
    RenderWindow window(VideoMode(1024, 768), "RogueRiddle", Style::Default);

    View view(FloatRect(0, 0, 1024, 768));
    view.zoom(0.25f);
    window.setVerticalSyncEnabled(true);
    sf::Clock clock;
    
    Level level = Level("./assets/level.tmx");
    
    std::shared_ptr<Player> player = level.GetPlayer();
    std::vector<std::shared_ptr<Enemy>> enemies = level.GetEnemies();
    const float fixedTimeStep = 0.016f;
    float accumulatedTime = 0.0f;

    sf::Font font;
    if (!font.loadFromFile("./assets/arial.ttf")) {
        return 1;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
 
    while (window.isOpen())
    {
        if (level.areAllEnemiesDead())
        {
            text.setString("Victory");
            endGame(window, player, level, text);
        }
        else if (level.isGameOver())
        {
            text.setString("Game Over");
            endGame(window, player, level, text);
        }
        else
        {
            mainGame(clock, accumulatedTime, fixedTimeStep, window, player, enemies, level);
        }
        
    }
    return 0;
}

void mainGame(sf::Clock& clock, float accumulatedTime, float fixedTimeStep,
    sf::RenderWindow& window, std::shared_ptr<Player> player, std::vector<std::shared_ptr<Enemy>> enemies, Level& level)
{
    float deltaTime = clock.restart().asSeconds();
    accumulatedTime += deltaTime;
    Event event;
    static bool upIsPressed = false;
    static bool upSpacePressed = false;

    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
    }

    while (accumulatedTime >= fixedTimeStep)
    {
        if (Keyboard::isKeyPressed(Keyboard::Up) && !upIsPressed)
        {
            upIsPressed = true;
            player->jump();
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::Up && upIsPressed)
            {
                upIsPressed = false;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            player->moveLeft(fixedTimeStep);
        }

        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            player->moveRight(fixedTimeStep);
        }

        if (!Keyboard::isKeyPressed(Keyboard::Left) && !Keyboard::isKeyPressed(Keyboard::Right))
        {
            player->stay(fixedTimeStep);
        }

        if (Keyboard::isKeyPressed(Keyboard::Space) && !upSpacePressed)
        {
            upSpacePressed = true;
            player->meleeAttack();
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::Space && upSpacePressed)
            {
                upSpacePressed = false;
            }
        }

        player->update(fixedTimeStep);

        for (std::shared_ptr<Enemy> enemy : enemies) {
            if (!enemy->isDead()) {
                enemy->update(fixedTimeStep);

            }
        }

        accumulatedTime -= fixedTimeStep;
    }

    if (level.areAllEnemiesDead()) {
        level.setGameOver(true);
    }

    window.setView(player->getView());
    window.clear();
    level.Draw(window);
    player->drawScore(window, level.getScore());

    for (std::shared_ptr<Enemy> enemy : enemies)
    {
        if (!enemy->isDead()) {
            enemy->draw(window);
        }
    }

    player->draw(window);
    window.display();
}

void endGame(sf::RenderWindow& window, std::shared_ptr<Player> player, const Level& level, sf::Text& text)
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
    }

    window.clear();
    text.setPosition(player->getView().getCenter().x - (text.getGlobalBounds().width / 2), player->getView().getCenter().y - 20);
    window.draw(text);
    window.setView(player->getView());
    window.display();
}