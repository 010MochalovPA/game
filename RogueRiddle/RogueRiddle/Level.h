#pragma once

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Enemy.h"

class Player;
class Enemy;

struct Object
{
    int GetPropertyInt(std::string name);
    float GetPropertyFloat(std::string name);
    std::string GetPropertyString(std::string name);

    std::string name;
    std::string type;
    sf::Rect<float> rect;
    std::map<std::string, std::string> properties;

    sf::Sprite sprite;
};

struct Layer
{
    int opacity;
    std::vector<sf::Sprite> tiles;
};

class Level
{
public:
    Level(std::string path);
    bool LoadFromFile(std::string filename);
    Object GetObject(std::string name);
    std::vector<Object> GetObjects(std::string name);
    std::vector<Object> GetAllObjects();
    std::vector<std::shared_ptr<Enemy>> GetEnemiesInArea(const sf::FloatRect& area);
    sf::FloatRect GetPlayerRect();
    void Draw(sf::RenderWindow& window);
    sf::Vector2i GetTileSize();
    int width, height, tileWidth, tileHeight;
    int GetWidth();
    int GetHeight();
    std::shared_ptr<Player> GetPlayer();
    std::vector<std::shared_ptr<Enemy>> GetEnemies();
    sf::Vector2f getPlayerPosition() const;
    int getScore() const;
    void increaseScore(int points);
    bool isGameOver() const;
    void setGameOver(bool gameOver);
    bool areAllEnemiesDead() const;

private:
    int firstTileID;
    sf::Rect<float> drawingBounds;
    sf::Texture tilesetImage;
    std::vector<Object> objects;
    std::vector<Layer> layers;
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<Enemy>> enemies;
    int m_score;
    bool m_gameOver;
};