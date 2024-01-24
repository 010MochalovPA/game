#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "TinyXML/tinyxml.h"
#include <sstream>
#include "Level.h"
#include "Player.h"
#include "Enemy.h"


int Object::GetPropertyInt(std::string name)
{
    return atoi(properties[name].c_str());
}

float Object::GetPropertyFloat(std::string name)
{
    return strtod(properties[name].c_str(), NULL);
}

std::string Object::GetPropertyString(std::string name)
{
    return properties[name];
}

Level::Level(std::string path)
{
    LoadFromFile(path);

    Object pl = GetObject("player");
    player = std::make_shared<Player>(*this, pl.rect.left, pl.rect.top);
    std::vector<Object> enemiesVector = GetObjects("enemy");

    for (Object en : enemiesVector) {
        enemies.push_back(std::make_shared<Enemy>(*this, en.rect.left, en.rect.top, 100.f));
    }
}

bool Level::LoadFromFile(std::string filename)
{
    TiXmlDocument levelFile(filename.c_str());

    // ��������� XML-�����
    if (!levelFile.LoadFile())
    {
        std::cout << "Loading level \"" << filename << "\" failed." << std::endl;
        return false;
    }

    // �������� � ����������� map
    TiXmlElement* map;
    map = levelFile.FirstChildElement("map");

    // ������ �����: <map version="1.0" orientation="orthogonal"
    // width="10" height="10" tilewidth="34" tileheight="34">
    width = atoi(map->Attribute("width"));
    height = atoi(map->Attribute("height"));
    tileWidth = atoi(map->Attribute("tilewidth"));
    tileHeight = atoi(map->Attribute("tileheight"));

    // ����� �������� �������� � ������������� ������� �����
    TiXmlElement* tilesetElement;
    tilesetElement = map->FirstChildElement("tileset");
    firstTileID = atoi(tilesetElement->Attribute("firstgid"));

    // source - ���� �� �������� � ���������� image
    TiXmlElement* image;
    image = tilesetElement->FirstChildElement("image");
    std::string imagepath = image->Attribute("source");
    // �������� ��������� �������
    sf::Image img;

    if (!img.loadFromFile(imagepath))
    {
        std::cout << "Failed to load tile sheet." << std::endl;
        return false;
    }


    img.createMaskFromColor(sf::Color(255, 255, 255));
    tilesetImage.loadFromImage(img);
    tilesetImage.setSmooth(false);

    // �������� ���������� �������� � ����� ��������
    int columns = tilesetImage.getSize().x / tileWidth;
    int rows = tilesetImage.getSize().y / tileHeight;

    // ������ �� ��������������� ����������� (TextureRect)
    std::vector<sf::Rect<int> > subRects;

    for (int y = 0; y < rows; y++)
        for (int x = 0; x < columns; x++)
        {
            sf::Rect<int> rect;

            rect.top = y * tileHeight;
            rect.height = tileHeight;
            rect.left = x * tileWidth;
            rect.width = tileWidth;

            subRects.push_back(rect);
        }

    // ������ �� ������
    TiXmlElement *layerElement;
    layerElement = map->FirstChildElement("layer");
    while(layerElement)
    {
        Layer layer;
		
		// ���� ������������ opacity, �� ������ ������������ ����, ����� �� ��������� �����������
        if (layerElement->Attribute("opacity") != NULL)
        {
            float opacity = strtod(layerElement->Attribute("opacity"), NULL);
            layer.opacity = 255 * opacity;
        }
        else
        {
            layer.opacity = 255;
        }

		// ��������� <data>
        TiXmlElement *layerDataElement;
        layerDataElement = layerElement->FirstChildElement("data");

        if(layerDataElement == NULL)
        {
            std::cout << "Bad map. No layer information found." << std::endl;
        }

		// ��������� <tile> - �������� ������ ������� ����
        TiXmlElement *tileElement;
        tileElement = layerDataElement->FirstChildElement("tile");

        if(tileElement == NULL)
        {
            std::cout << "Bad map. No tile information found." << std::endl;
            return false;
        }

        int x = 0;
        int y = 0;

        while(tileElement)
        {
            int tileGID = atoi(tileElement->Attribute("gid"));
            int subRectToUse = tileGID - firstTileID;

			// ������������� TextureRect ������� �����
            if (subRectToUse >= 0)
            {
                sf::Sprite sprite;
                sprite.setTexture(tilesetImage);
				sprite.setTextureRect(subRects[subRectToUse]);
                sprite.setPosition(x * tileWidth, y * tileHeight);
                sprite.setColor(sf::Color(255, 255, 255, layer.opacity));

                layer.tiles.push_back(sprite);
            }

            tileElement = tileElement->NextSiblingElement("tile");

            x++;
            if (x >= width)
            {
                x = 0;
                y++;
                if(y >= height)
                    y = 0;
            }
        }

        layers.push_back(layer);

        layerElement = layerElement->NextSiblingElement("layer");
    }

    // ������ � ���������
    TiXmlElement* objectGroupElement;

    // ���� ���� ���� ��������
    if (map->FirstChildElement("objectgroup") != NULL)
    {
        objectGroupElement = map->FirstChildElement("objectgroup");
        while (objectGroupElement)
        {
            // ��������� <object>
            TiXmlElement* objectElement;
            objectElement = objectGroupElement->FirstChildElement("object");

            while (objectElement)
            {
                // �������� ��� ������ - ���, ���, �������, etc
                std::string objectType;
                if (objectElement->Attribute("type") != NULL)
                {
                    objectType = objectElement->Attribute("type");
                }
                std::string objectName;
                if (objectElement->Attribute("name") != NULL)
                {
                    objectName = objectElement->Attribute("name");
                }
                float x = atof(objectElement->Attribute("x"));
                float y = atof(objectElement->Attribute("y"));

                float width, height;

                
                sf::Sprite sprite;
                sprite.setTexture(tilesetImage);
                sprite.setTextureRect(sf::Rect<int>(0, 0, 0, 0));
                sprite.setPosition(x, y);

                if (objectElement->Attribute("width") != NULL)
                {
                    width = atoi(objectElement->Attribute("width"));
                    height = atoi(objectElement->Attribute("height"));
                }
                else
                {
                    width = subRects[static_cast<std::vector<sf::Rect<int>, std::allocator<sf::Rect<int>>>::size_type>(atoi(objectElement->Attribute("gid"))) - firstTileID].width;
                    height = subRects[static_cast<std::vector<sf::Rect<int>, std::allocator<sf::Rect<int>>>::size_type>(atoi(objectElement->Attribute("gid"))) - firstTileID].height;
                    sprite.setTextureRect(subRects[static_cast<std::vector<sf::Rect<int>, std::allocator<sf::Rect<int>>>::size_type>(atoi(objectElement->Attribute("gid"))) - firstTileID]);
                }
                
                // ��������� �������
                Object object;
                object.name = objectName;
                object.type = objectType;
                object.sprite = sprite;

                sf::Rect <float> objectRect;
                objectRect.top = y;
                objectRect.left = x;
                objectRect.height = height;
                objectRect.width = width;
                object.rect = objectRect;

                // "����������" �������
                TiXmlElement* properties;
                properties = objectElement->FirstChildElement("properties");
                if (properties != NULL)
                {
                    TiXmlElement* prop;
                    prop = properties->FirstChildElement("property");
                    if (prop != NULL)
                    {
                        while (prop)
                        {
                            std::string propertyName = prop->Attribute("name");
                            std::string propertyValue = prop->Attribute("value");

                            object.properties[propertyName] = propertyValue;

                            prop = prop->NextSiblingElement("property");
                        }
                    }
                }

                objects.push_back(object);

                objectElement = objectElement->NextSiblingElement("object");
            }
            objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
        }
    }
    else
    {
        std::cout << "No object layers found..." << std::endl;
    }
    return true;
}

Object Level::GetObject(std::string name)
{
    for (int i = 0; i < objects.size(); i++)
        if (objects[i].name == name)
            return objects[i];
}

std::vector<Object> Level::GetObjects(std::string name)
{
    std::vector<Object> vec;
    for (int i = 0; i < objects.size(); i++)
        if (objects[i].name == name) {

            vec.push_back(objects[i]);
           
        }

    return vec;
}

std::vector<Object> Level::GetAllObjects()
{
    return objects;
};

sf::Vector2i Level::GetTileSize()
{
    return sf::Vector2i(tileWidth, tileHeight);
}

int Level::GetWidth()
{
    return width;
}

int Level::GetHeight()
{
    return height;
}

std::shared_ptr<Player> Level::GetPlayer()
{
    return player;
}

std::vector<std::shared_ptr<Enemy>> Level::GetEnemies()
{
    return enemies;
}

sf::Vector2f Level::getPlayerPosition() const {
    return { player->getX(), player->getY() };
}

int Level::getScore() const {
    return m_score;
}

void Level::increaseScore(int points) {
    m_score += points;
}

std::vector<std::shared_ptr<Enemy>> Level::GetEnemiesInArea(const sf::FloatRect& area)
{
    std::vector<std::shared_ptr<Enemy>> enemiesInArea;
    
    for (std::shared_ptr<Enemy> enemy : enemies)
    {
        sf::FloatRect enemyRect(enemy->getX(), enemy->getY(), enemy->getWidth(), enemy->getHeight());

        if (enemyRect.intersects(area))
        {
            enemiesInArea.push_back(enemy);
        }
    }
    
    return enemiesInArea;
}

sf::FloatRect Level::GetPlayerRect()
{
    return sf::FloatRect(player->getX(), player->getY(), player->getWidth(), player->getHeight());
}

void Level::Draw(sf::RenderWindow& window)
{
    for (int layer = 0; layer < layers.size(); layer++)
    {
        for (int tile = 0; tile < layers[layer].tiles.size(); tile++)
        {
            window.draw(layers[layer].tiles[tile]);
        }  
    }
}

bool Level::isGameOver() const
{
    return m_gameOver;
}

void Level::setGameOver(bool gameOver)
{
    m_gameOver = gameOver;
}

bool Level::areAllEnemiesDead() const
{
    for (const auto& enemy : enemies) {
        if (!enemy->isDead()) {
            return false;
        }
    }
    return true;
}