#include "board.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <tuple>
// using namespace std;

static Candy CANDY_RED(CandyType::TYPE_RED);
static Candy CANDY_GREEN(CandyType::TYPE_GREEN);
static Candy CANDY_BLUE(CandyType::TYPE_BLUE);
static Candy CANDY_YELLOW(CandyType::TYPE_YELLOW);
static Candy CANDY_PURPLE(CandyType::TYPE_PURPLE);
static Candy CANDY_ORANGE(CandyType::TYPE_ORANGE);
static Candy CANDY_ARR[6] = {
    CANDY_RED, CANDY_GREEN, CANDY_BLUE, CANDY_YELLOW, CANDY_PURPLE, CANDY_ORANGE};

Board::Board(int width, int height)
{
    m_width  = width;  // columnas
    m_height = height; // filas
    for (int i = 0; i < m_height; i++)
    {
        for (int j = 0; j < m_width; j++)
        {
            m_grid[i][j] = nullptr;
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
        return m_grid[y][x];
    return nullptr;
}

void Board::setCell(Candy* candy, int x, int y)
{
    if ((x >= 0 && x < m_width) && (y >= 0 && y < m_height))
        m_grid[y][x] = candy;
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

    bool found      = true;
    bool isPositive = true;
    int  i          = 1;
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
                    i          = 1; // reiniciar porque cambia a dirección negativa
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
                i          = 1; // reiniciar porque cambia a dirección negativa
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
    int  count   = 1;
    bool explode = false;

    Candy* actualCandy = getCell(x, y);
    if (actualCandy != nullptr)
    {
        CandyType actualType = actualCandy->getType();

        // Check
        int i = 1;

        // Check Vertical
        if (!explode)
            explode = searchCandy(x, y, count, 0, 1, actualType);

        // Check Horizontal
        count = 1;
        if (!explode)
            explode = searchCandy(x, y, count, 1, 0, actualType);

        // Diagonal x positiva /
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
    std::vector<Candy*>               exploded;
    std::vector<std::tuple<int, int>> toExplode = {};
    do
    {
        toExplode.clear();
        for (int i = 0; i < m_height; ++i)
        {
            for (int j = 0; j < m_width; ++j)
            {
                if (this->shouldExplode(i, j))
                {
                    toExplode.emplace_back(i, j);
                }
            }
        }
        for (auto&& [i, j] : toExplode)
        {
            exploded.push_back(getCell(i, j));
            m_grid[i][j] = nullptr;
        }
        for (int j = 0; j < m_width; ++j)
        {
            int cursor = m_height - 1;

            for (int i = m_height - 1; i >= 0; --i)
            {
                if (m_grid[i][j])
                {
                    m_grid[cursor][j] = m_grid[i][j];

                    if (cursor != i)
                        m_grid[i][j] = nullptr;

                    cursor--;
                }
            }
        }

    } while (!toExplode.empty());
    return exploded;
}

int display(Candy* candy)
{
    int ret;
    if (candy)
        ret = (int) (candy->getType());
    else
        ret = -1;
    return ret;
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here
    std::ofstream f;
    f.open(output_path);
    if (!f.is_open())
    {
        return false;
    }
    f << m_height << ' ' << m_width << std::endl;

    for (int i = 0; i < m_height; i++)
    {
        for (int j = 0; j < m_width; j++)
            f << display(m_grid[i][j]) << ' ';
        f << std::endl;
    }

    bool good = f.good();
    f.close();
    return good;
}

Candy* parse(int type)
{
    Candy* ret;
    if (type == -1 && type > 0 && type < 6)
        ret = nullptr;
    else
        ret = &CANDY_ARR[type];
    return ret;
}

bool Board::load(const std::string& input_path)
{
    // Implement your code here
    std::ifstream f;
    f.open(input_path);
    if (!f.is_open())
        return false;

    f >> m_height >> m_width;
    for (int i = 0; i < m_height; ++i)
    {
        for (int j = 0; j < m_width; ++j)
        {
            int x;
            f >> x;

            m_grid[i][j] = parse(x);
        }
    }

    bool good = f.good();
    f.close();
    return good;
}
