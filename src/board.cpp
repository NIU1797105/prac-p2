#include "board.h"

#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

int display(Candy* candy)
{
    if (!candy)
        return -1;
    return static_cast<int>(candy->getType());
}

Candy* parse(int type)
{
    if (type == -1)
        return nullptr;
    if (type >= 0 && type < static_cast<int>(CandyType::COUNT))
        return new Candy(static_cast<CandyType>(type));
    return nullptr;
}

Board::Board(int width, int height)
    : m_height(height), m_width(width), m_grid(nullptr)
{
    if (m_height <= 0 || m_width <= 0)
    {
        m_height = 0;
        m_width = 0;
        return;
    }

    m_grid = new Candy**[m_height];
    for (int y = 0; y < m_height; ++y)
    {
        m_grid[y] = new Candy*[m_width];
        for (int x = 0; x < m_width; ++x)
            m_grid[y][x] = nullptr;
    }
}

Board::~Board()
{
    if (!m_grid)
        return;

    for (int y = 0; y < m_height; ++y)
    {
        if (!m_grid[y])
            continue;
        for (int x = 0; x < m_width; ++x)
        {
            delete m_grid[y][x];
        }
        delete[] m_grid[y];
    }
    delete[] m_grid;
    m_grid = nullptr;
}

Candy* Board::getCell(int x, int y) const
{
    if (!m_grid)
        return nullptr;
    if ((x >= 0 && x < m_width) && (y >= 0 && y < m_height))
        return m_grid[y][x];
    return nullptr;
}

void Board::setCell(Candy* candy, int x, int y)
{
    if (m_grid && (x >= 0 && x < m_width) && (y >= 0 && y < m_height))
    {
        if (m_grid[y][x] != nullptr)
        {
            delete m_grid[y][x];
            m_grid[y][x] = nullptr;
        }

        if (candy != nullptr)
            m_grid[y][x] = new Candy(candy->getType());
        else
            m_grid[y][x] = nullptr;
    }
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
    bool found = true;
    bool isPositive = true;
    int i = 1;
    while (count < MINIM_EXPLOSIO && found)
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
                    i = 1; // reiniciar porque cambia a direcci�n negativa
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
                i = 1; // reiniciar porque cambia a direcci�n negativa
            }
            else
                found = false;
        }
    }

    return (count >= MINIM_EXPLOSIO);
}

bool Board::operator==(const Board& other) const
{
    bool ret = true;
    if (m_height != other.m_height)
        ret = false;
    if (ret && m_width != other.m_width)
        ret = false;


    for (int y = 0; y < m_height && ret; y++)
    {
        for (int x = 0; x < m_width && ret; x++)
        {
            Candy* a = m_grid[y][x];
            Candy* b = other.m_grid[y][x];
                
            if (a == nullptr && b != nullptr)
                ret = false;

            if (b == nullptr && a != nullptr)
                ret = false;

            if (a != nullptr && b != nullptr)
            {
                if (a->getType() != b->getType())
                    ret = false;
            }
        }
    }
    
    return ret;
}

bool Board::shouldExplode(int x, int y) const
{
    int count = 1;
    bool explode = false;

    Candy* actualCandy = getCell(x, y);
    if (actualCandy != nullptr)
    {
        CandyType actualType = actualCandy->getType();

        // Vertical
        if (!explode)
            explode = searchCandy(x, y, count, 0, 1, actualType);

        // Horizontal
        count = 1;
        if (!explode)
            explode = searchCandy(x, y, count, 1, 0, actualType);

        // Diagonal (/)
        count = 1;
        if (!explode)
            explode = searchCandy(x, y, count, 1, 1, actualType);

        // Diagonal (\)
        count = 1;
        if (!explode)
            explode = searchCandy(x, y, count, -1, 1, actualType);
    }
    return explode;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    std::vector<Candy*> exploded;
    std::vector<std::tuple<int,int>> toExplode;

    if (!m_grid)
        return exploded;

    do
    {
        toExplode.clear();
        for (int y = 0; y < m_height; ++y)
        {
            for (int x = 0; x < m_width; ++x)
            {
                if (shouldExplode(x, y))
                    toExplode.emplace_back(x, y);
            }
        }

        if (toExplode.empty())
            break;

        for (auto &p : toExplode)
        {
            int x = std::get<0>(p);
            int y = std::get<1>(p);
            if (m_grid[y][x])
            {
                exploded.push_back(m_grid[y][x]);
                m_grid[y][x] = nullptr;
            }
        }

        // Drop candies column by column
        for (int x = 0; x < m_width; ++x)
        {
            int cursor = m_height - 1;
            for (int y = m_height - 1; y >= 0; --y)
            {
                if (m_grid[y][x])
                {
                    if (cursor != y)
                    {
                        m_grid[cursor][x] = m_grid[y][x];
                        m_grid[y][x] = nullptr;
                    }
                    cursor--;
                }
            }
            for (int y = cursor; y >= 0; --y)
                m_grid[y][x] = nullptr;
        }

    } while (!toExplode.empty());

    return exploded;
}

bool Board::dump(const std::string& output_path) const
{
    std::ofstream f(output_path);
    if (!f.is_open())
        return false;

    f << m_height << ' ' << m_width << "\n";
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
            f << display(m_grid[y][x]) << ' ';
        f << '\n';
    }
    bool good = f.good();
    f.close();
    return good;
}

bool Board::load(const std::string& input_path)
{
    std::ifstream f(input_path);
    if (!f.is_open())
        return false;

    int h = 0, w = 0;
    f >> h >> w;
    if (!f.good() && !f.eof())
        return false;
    if (h <= 0 || w <= 0)
        return false;

    if (m_grid)
    {
        for (int y = 0; y < m_height; ++y)
        {
            if (!m_grid[y])
                continue;
            for (int x = 0; x < m_width; ++x)
                delete m_grid[y][x];
            delete[] m_grid[y];
        }
        delete[] m_grid;
        m_grid = nullptr;
    }

    m_height = h;
    m_width = w;

    m_grid = new Candy**[m_height];
    for (int y = 0; y < m_height; ++y)
    {
        m_grid[y] = new Candy*[m_width];
        for (int x = 0; x < m_width; ++x)
        {
            int t;
            if (!(f >> t))
            {
                for (int yy = 0; yy <= y; ++yy)
                {
                    if (!m_grid[yy]) continue;
                    for (int xx = 0; xx < m_width; ++xx)
                        delete m_grid[yy][xx];
                    delete[] m_grid[yy];
                }
                delete[] m_grid;
                m_grid = nullptr;
                return false;
            }
            m_grid[y][x] = parse(t);
        }
    }

    bool good = f.good();
    f.close();
    return good;
}
