#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"
#include "board.h"




Game::Game() : m_board(), m_gen(random_device{}())
{
    m_x = 0;
    m_y = 0;
    m_blockCandy = new Candy*[DEFAULT_BLOCKSIZE];
    for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
    {
        m_blockCandy[i] = nullptr;
    }
    landed = false;
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
    if (!landed)
    {
        uniform_int_distribution<int> distributionPosition(0, DEFAULT_BOARD_HEIGHT-1);
        uniform_int_distribution<int> distributionCandyTypes(0, NUM_CANDYTYPES-1);
        m_x = distributionPosition(m_gen);
        m_y = -1;
         
        for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
        {
            if (m_blockCandy[i] != nullptr)
            {
                delete m_blockCandy[i];
            }
            int candyType = distributionCandyTypes(m_gen);
            CandyType type = CandyType::COUNT;
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
            if (m_x > 0)
            {
                m_x--;
            }

        }
        else if (controller.isRightPressed())
        {
            //Mover bloque de caramelos a la derecha
            if (m_x < m_board.getWidth()-1)
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
    }
    else if (controller.isKey3Pressed()) //E
    {
        //Leer estado, asumo¿¿
    }

    // Estado Tablero
    ///REVISAR Y CAMBIAR
    if (m_y == m_board.getHeight() - 1 || m_board.getCell(m_x,m_y+1) != nullptr)
    {
        //Ha llegado al límite del tablero
        for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
        {
            if (m_y + i < m_board.getHeight())
            { 
                //Anclar el bloque a su posicion en el board
                m_board.setCell(m_blockCandy[i], m_x, m_y + i);

                //Reiniciar el bloque de caramelos
                if (m_blockCandy[i] != nullptr)
                {
                    delete m_blockCandy[i];
                    m_blockCandy[i] = nullptr;
                }
            }
            
        }
        landed = true;
    }
    else
    {
        m_y++;
    }

    
    // Explosiones y tal
}
bool fallCandy(Board board, int i, int j)
{
    
    return false;

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

    

    graphics.drawImage(Candy(CandyType::TYPE_PURPLE).getResourceName(),
        CANDY_IMAGE_WIDTH * 3,
        CANDY_IMAGE_HEIGHT * 3);
    // Title [draw images]
    graphics.drawImage("img/logo_small.png", 10, 10);
    // Score and footer [draw text]
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
                      "Buttons: [Q] [W] [E]  --  Exit [ESC]",
                      25, 700, 20, 100, 100, 100);
    graphics.drawText("Score: ", 450, 10, 70, 125, 200, 125);
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