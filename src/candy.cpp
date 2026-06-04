#include "candy.h"

#include <cstdlib>
#include <iostream>
#include <string>

Candy::Candy(CandyType candyType) : m_type(candyType)
{
    m_type = candyType;
}

Candy::Candy()
{
    m_type = CandyType::COUNT;
}

Candy* Candy::parse_new(int type)
{
    if (type == -1)
        return nullptr;
    if (type >= 0 && type < static_cast<int>(CandyType::COUNT))
        return new Candy(static_cast<CandyType>(type));
    return nullptr;
}

CandyType Candy::getType() const
{
    return m_type;
}

std::string Candy::getResourceName() const
{
    const std::string candy_resource_prefix = "img/candy/";
    std::string       candy_resource_name;

    switch (m_type)
    {
        case CandyType::TYPE_RED:
            candy_resource_name = candy_resource_prefix + "red.png";
            break;
        case CandyType::TYPE_GREEN:
            candy_resource_name = candy_resource_prefix + "green.png";
            break;
        case CandyType::TYPE_BLUE:
            candy_resource_name = candy_resource_prefix + "blue.png";
            break;
        case CandyType::TYPE_YELLOW:
            candy_resource_name = candy_resource_prefix + "yellow.png";
            break;
        case CandyType::TYPE_PURPLE:
            candy_resource_name = candy_resource_prefix + "purple.png";
            break;
        case CandyType::TYPE_ORANGE:
            candy_resource_name = candy_resource_prefix + "orange.png";
            break;
        default:
            std::cout << "Invalid candy type. aborting" << std::endl;
            abort();
    }

    return candy_resource_name;
}
