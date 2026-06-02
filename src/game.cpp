#include "game.h"
#include <random>
#include <vector>
#include "graphics.h"
#include "candy.h"
#include "board.h"


const int SPEED_GAME = 30;

Game::Game() : m_board(), m_gen(std::random_device{}())
{
    m_x = 0;
    m_y = 0;
    m_speedCounter = 0;
    m_blockCandy = new Candy*[DEFAULT_BLOCKSIZE];
    for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
    {
        m_blockCandy[i] = nullptr;
    }
    landed = false;
    m_gameOver = false;
    m_score = 0;
}

Game::~Game()
{
    for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
    {
        delete m_blockCandy[i];
    }

    delete[] m_blockCandy;
}

void Game::update(const Controller& controller)
{
    if (m_gameOver)
        return;

    //Generar el bloque de 3 si no existe
    if (m_blockCandy[0] == nullptr)
    {
        //Random
        uniform_int_distribution<int> distributionCandyTypes(0, NUM_CANDYTYPES-1);

        std::vector<int> availableColumns;
        for (int x = 0; x < m_board.getWidth(); ++x)
        {
            if (m_board.getCell(x, 0) == nullptr)
            {
                availableColumns.push_back(x);
            }
        }

        if (availableColumns.size() == 0)
        {
            m_gameOver = true;
            return;
        }

        int availableCount = (int)availableColumns.size();
        uniform_int_distribution<int> distributionPosition(0, availableCount - 1);
        m_x = availableColumns[distributionPosition(m_gen)];
        m_y = -1;

        for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
        {
            if (m_blockCandy[i] != nullptr)
            {
                delete m_blockCandy[i];
            }
            int candyType = distributionCandyTypes(m_gen);
            CandyType type;
            switch (candyType)
            {
            case 0:
                type = CandyType::TYPE_RED;
                break;
            case 1:
                type = CandyType::TYPE_GREEN;
                break;
            case 2:
                type = CandyType::TYPE_BLUE;
                break;
            case 3:
                type = CandyType::TYPE_YELLOW;
                break;
            case 4:
                type = CandyType::TYPE_PURPLE;
                break;
            case 5:
                type = CandyType::TYPE_ORANGE;
                break;
            default:
                type = CandyType::TYPE_RED;
                break;

            }
            m_blockCandy[i] = new Candy(type);
        }

    }
    else
    {
        //Inputs
        if (controller.isUpPressed())
        {
            //NO se que hace
        }
        else if (controller.isDownPressed())
        {
            //Acelerar caída
            m_y++;
        }
        else if (controller.isLeftPressed())
        {
            //Mover bloque de caramelos a la izquierda
            if (m_x > 0 && m_board.getCell(m_x -1, m_y) == nullptr && m_board.getCell(m_x - 1, m_y - 1) == nullptr && m_board.getCell(m_x - 1, m_y - 2) == nullptr)
            {
                m_x--;
            }

        }
        else if (controller.isRightPressed())
        {
            //Mover bloque de caramelos a la
            if (m_x < m_board.getWidth() - 1 && m_board.getCell(m_x + 1, m_y) == nullptr && m_board.getCell(m_x + 1, m_y - 1) == nullptr && m_board.getCell(m_x + 1, m_y - 2) == nullptr)
            {
                m_x++;
            }
        }
        else if (controller.isKey1Pressed()) //Q
        {
            // se rota el orden de los caramelos dentro del bloque que cae (ABC → BCA → CAB → ABC).
            Candy* tmp = m_blockCandy[0];
            for (int i = 0; i < DEFAULT_BLOCKSIZE - 1; i++)
            {
                m_blockCandy[i] = m_blockCandy[i + 1];
            }
            m_blockCandy[DEFAULT_BLOCKSIZE - 1] = tmp;


        }
    }
    if (controller.isKey2Pressed()) //W
    {
        //Guardar estado
        m_board.dump("data/save.txt");
    }
    // else if (controller.isKey3Pressed() && m_toLoad) //E
    // {
    //     m_board.load(m_toLoad)
    // }

    // Estado Tablero
    // Decide whether the falling block should land. The previous logic only checked the
    // cell immediately below the block's bottom which allowed overlapping existing
    // candies higher in the column. We now check whether moving the block down by one
    // would cause any of its cells to overlap existing candies; if so, it must land
    // at the current position. If the block cannot be placed at the current position
    // because there is no free cell in the column, we mark game over.
    {
        int newY = m_y + 1;
        bool willCollide = false;
        for (int i = 0; i < DEFAULT_BLOCKSIZE; ++i)
        {
            int pos = newY - i;
            if (pos >= 0 && pos < m_board.getHeight())
            {
                if (m_board.getCell(m_x, pos) != nullptr)
                {
                    willCollide = true;
                    break;
                }
            }
        }

        if (newY >= m_board.getHeight() || willCollide)
        {
            if (willCollide && m_y < DEFAULT_BLOCKSIZE)
            {
                // No room to place any part of the block -> game over
                m_gameOver = true;
                // Clean up the falling block
                for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
                {
                    if (m_blockCandy[i] != nullptr)
                    {
                        delete m_blockCandy[i];
                        m_blockCandy[i] = nullptr;
                    }
                }
            }
            else
            {
                // Anchor the block at the current position (none of these cells should
                // contain candies because movement checks ensure the current position is safe)
                for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
                {
                    if (m_y - i >= 0 && m_y - i < m_board.getHeight())
                    {
                        m_board.setCell(m_blockCandy[i], m_x, m_y - i);

                        if (m_blockCandy[i] != nullptr)
                        {
                            delete m_blockCandy[i];
                            m_blockCandy[i] = nullptr;
                        }
                    }
                }
                landed = true;
            }
        }
        else
        {
            if (m_speedCounter >= SPEED_GAME)
            {
                m_y++;
                m_speedCounter = 0;
            }
            else
            {
                m_speedCounter++;
            }
        }
    }


    // Explosiones y tal
    // explodeAndDrop returns a vector<Candy*> (non-owning pointers). Caller must not delete these
    std::vector<Candy*> exploded = m_board.explodeAndDrop();
    scoreUpdate(exploded);
}

void Game::scoreUpdate(const std::vector<Candy*>& candies) {
    if (candies.size() > 0)
        m_score += 1000 * (1 << (candies.size() - MINIM_EXPLOSIO));
}

void drawCandy(GraphicManager& graphics, Candy* candy, int x, int y)
{
    if (candy != nullptr)
    {
        graphics.drawImage(Candy(candy->getType()).getResourceName(),
            CANDY_IMAGE_WIDTH * (3 + x),
            CANDY_IMAGE_HEIGHT * (3 + y));
    }
}
void Game::render(GraphicManager& graphics)
{
    // Implement your code here

    // Note: the following code exhibits the main graphic library features
    // Board: border [draw rectangles] and a single piece of candy
    const int board_size = 10;
    const int board_padding = 3;
    graphics.drawRectangle(
        CANDY_IMAGE_HEIGHT * board_padding, CANDY_IMAGE_HEIGHT * board_padding,
        CANDY_IMAGE_WIDTH * board_size,
        CANDY_IMAGE_HEIGHT * board_size,
        5, 150, 150, 150);
    // Board: place a candy piece
    //Board
    for (int y = 0; y < m_board.getHeight(); y++)
    {
        for (int x = 0; x < m_board.getWidth(); x++)
        {
            Candy* candy = m_board.getCell(x,y);
            drawCandy(graphics, candy, x, y);

        }
    }

    //Bloque de caramelos
    for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
    {
        //Si está entre los rangos 0 y 9:
        if (m_y - i >= 0 && m_y - i < m_board.getHeight())
        {
            //Pintalo
            drawCandy(graphics, m_blockCandy[i], m_x, m_y-i);

        }
    }

    /*
    graphics.drawImage(Candy(CandyType::TYPE_PURPLE).getResourceName(),
        CANDY_IMAGE_WIDTH * 3,
        CANDY_IMAGE_HEIGHT * 3);
        */
    // Title [draw images]
    graphics.drawImage("img/logo_small.png", 10, 10);
    // Score and footer [draw text]
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
                      "Buttons: [Q] [W] [E]  --  Exit [ESC]",
                      25, 700, 20, 100, 100, 100);
    graphics.drawText("Score: " + std::to_string(m_score), 220, 10, 70, 125, 200, 125);

    #ifndef GRADESCOPE
    if (m_gameOver)
    {
        static int nFrame = 0;
        static bool state = false;

        string str = "img/game_over_n" + std::to_string(nFrame) + ".png";
        graphics.drawImage(str, 0, 0);

        if (nFrame == 0 || nFrame == 10)
            state = !state;
        if (state) {
            nFrame++;
        } else {
            nFrame--;
        }
    }
    #endif
}

void Game::run()
{
    const int screen_width = 750;
    const int screen_height = 750;
    const int bg_red = 255;
    const int bg_green = 255;
    const int bg_blue = 255;
    runGraphicGame(*this, screen_width, screen_height, bg_red, bg_green, bg_blue);


}

bool Game::dump(const std::string& output_path) const
{
    // Implement your code here
    return false;
}

bool Game::load(const std::string& input_path)
{
    // Implement your code here
    return false;
}

bool Game::operator==(const Game& other) const
{
    // Implement your code here
    return false;
}
