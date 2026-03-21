#include "board.h"
#include <memory>
#include <iostream>
using namespace std;

Board::Board(int width, int height)
{
    m_width = width; // columnas
    m_height = height; // filas
    for (int i = 0; i < m_height; i++)
    {
        for (int j = 0; j < m_width; j++)
        {
            grid[i][j] = nullptr;
            
        }
    }
}

Board::~Board()
{
    // Implement your code here
}


Candy* Board::getCell(int x, int y) const
{
    if ((x >= 0 && x < m_width) && (y >= 0 && y < m_height))
        return grid[y][x];
    return nullptr;
}

void Board::setCell(Candy* candy, int x, int y)
{
    if ((x >= 0 && x < m_width) && (y >= 0 && y < m_height))
        grid[y][x] = candy;
}


int Board::getWidth() const
{
    return m_width;
}


int Board::getHeight() const
{
    return m_height;
}

bool Board::searchCandy(int x, int y, int& count, int difX, int difY, CandyType actualType) const
{
    bool explode = false;

    bool found = true;
    bool isPositive = true;
    int i = 1;
    while (count < 3 && found)
    {
        
        Candy* c = nullptr;
        if (isPositive)
            c = getCell(x + difX * i, y + difY * i);
        else
            c = getCell(x - difX * i, y - difY * i);

        if (c != nullptr)
        {
            
            CandyType type = c->getType();
            if (actualType == type)
            {
                count++;
                i++;
            }
            else
            {
                if (isPositive)
                {
                    isPositive = false;
                    i = 1; //reiniciar porque cambia a dirección negativa
                }
                else
                    found = false;
            }
        }
        else
        {
            if (isPositive)
            {
                isPositive = false;
                i = 1; //reiniciar porque cambia a dirección negativa
            }
            else
                found = false;
        }

    }

    if (count >= 3)
        explode = true;

    return explode;
}

bool Board::shouldExplode(int x, int y) const
{
    int count = 1;
    bool explode = false;

    Candy* actualCandy = getCell(x,y); 
    if (actualCandy != nullptr)
    {
        CandyType actualType = actualCandy->getType();

        //Check
        int i = 1;

        //Check Vertical
        if (!explode)
            explode = searchCandy(x, y, count, 0, 1, actualType);

        //Check Horizontal
        count = 1;
        if (!explode)
            explode = searchCandy(x, y, count, 1, 0, actualType);


        //Diagonal x positiva /
        count = 1;
        if (!explode)
            explode = searchCandy(x, y, count, 1, 1, actualType);

        //Diagonal x negativa \
        
        count = 1;
        if (!explode)
            explode = searchCandy(x, y, count, -1, 1, actualType);
    }
    return explode;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here
    return {};
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here
    return false;
}

bool Board::load(const std::string& input_path)
{
    // Implement your code here
    return false;
}
