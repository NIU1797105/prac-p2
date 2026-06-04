#include "game.h"
// #include "private/sdl_wrapper/sound.h"
#include "sound_mgr.h"
#include <random>
#include <vector>
#include "graphics.h"
#include "candy.h"
#include "board.h"
#include <fstream>

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
    m_landed = false;
    m_pause = false;
    m_gameOver = false;
    m_score = 0;

    //Audio
    m_sound = SoundManager();
    m_sound.loadMusic("bg.ogg");
    m_sound_stars = m_sound.loadSound("stars.ogg", false);
    m_music_pause = m_sound.loadSound("beam.ogg", true);
    m_music_croco = m_sound.loadSound("crocodile_2.ogg", true);
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
            m_blockCandy[i] = Candy::parse_new(candyType);
        }

    }
    else
    {
        //Inputs
        if (controller.isUpPressed())
        {
            m_pause = !m_pause;
            if (m_pause)
            {
                m_sound.stopMusic();
                m_sound.playSound(m_music_pause);
            }
            else
            {
                m_sound.startMusic();
            }
        }
        if (!m_pause)
        {
            if (controller.isDownPressed())
            {
                //Acelerar caída
                int i = 0;
                bool trobat = false;

                while (m_board.getHeight() > i && !trobat)
                    trobat = nullptr != m_board.getCell(m_x, i++);

                m_y = i - (1 + static_cast<int>(trobat));
            }
            else if (controller.isLeftPressed())
            {
                //Mover bloque de caramelos a la izquierda

                if (m_x > 0 && m_board.getCell(m_x - 1, m_y) == nullptr
                    && m_board.getCell(m_x - 1, m_y - 1) == nullptr
                    && m_board.getCell(m_x - 1, m_y - 2) == nullptr)
                {
                    m_x--;
                }

            }
            else if (controller.isRightPressed())
            {
                //Mover bloque de caramelos a la
                if (m_x < m_board.getWidth() - 1
                    && m_board.getCell(m_x + 1, m_y) == nullptr
                    && m_board.getCell(m_x + 1, m_y - 1) == nullptr
                    && m_board.getCell(m_x + 1, m_y - 2) == nullptr)
                {
                    m_x++;
                }
            }
        }
        if (controller.isKey1Pressed()) //Q
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
        dump("data/save.txt");
    }
    if (controller.isKey3Pressed()) //E
    {
        load("data/save.txt");
    }

    // Estado Tablero
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
                m_landed = true;
            }
        }
        else
        {
            if (!m_pause)
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
    }


    // Explosiones y tal
    std::vector<Candy*> exploded = m_board.explodeAndDrop();
    scoreUpdate(exploded);
}

void Game::scoreUpdate(const std::vector<Candy*>& candies) {
    if (candies.size() > 0)
    {
        m_score += 1000 * (1 << (candies.size() - MINIM_EXPLOSIO));
        Sound_Play(m_sound_stars, SOUND_DO_NOT_RESTART_IF_ALREADY_PLAYING);
    }
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
    graphics.drawImage("img/bg_2.png", 0, 0);

    // Note: the following code exhibits the main graphic library features
    // Board: border [draw rectangles] and a single piece of candy
    const int board_size = 10;
    const int board_padding = 3;
    /*
    graphics.drawRectangle(
        CANDY_IMAGE_HEIGHT * board_padding, CANDY_IMAGE_HEIGHT * board_padding,
        CANDY_IMAGE_WIDTH * board_size,
        CANDY_IMAGE_HEIGHT * board_size,
        5, 150, 150, 150);
        */
    graphics.drawImage("img/grid.png", CANDY_IMAGE_HEIGHT * board_padding, CANDY_IMAGE_HEIGHT * board_padding);
    // Board: place a candy piece
    //Board
    for (int y = 0; y < m_board.getHeight(); y++)
    {
        for (int x = 0; x < m_board.getWidth(); x++)
        {
            Candy* candy = m_board.getCell(x, y);
            drawCandy(graphics, candy, x, y);

        }
    }
    graphics.drawImage("img/frame.png", CANDY_IMAGE_HEIGHT * board_padding, CANDY_IMAGE_HEIGHT * board_padding);

    //Bloque de caramelos
    for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
    {
        //Si está entre los rangos 0 y 9:
        if (m_y - i >= 0 && m_y - i < m_board.getHeight())
        {
            //Pintalo
            drawCandy(graphics, m_blockCandy[i], m_x, m_y - i);
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
    graphics.drawText("Score: " + std::to_string(m_score), 260, 30, 70, 125, 200, 125);

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
        }
        else
        {
            nFrame--;
        }
    }
    #endif
}


void Game::run()
{
    const int screen_width = 1080;
    const int screen_height = 750;
    const int bg_red = 255;
    const int bg_green = 255;
    const int bg_blue = 255;
    runGraphicGame(*this, screen_width, screen_height, bg_red, bg_green, bg_blue);


}

bool Game::dump(const std::string& output_path) const
{
    bool ret = false;
    ret = m_board.dump(output_path);

    if (!ret)
        return false;

    ofstream f;
    f.open(output_path, ofstream::app);
    if (!f.is_open())
        return false;
    f << m_y << " " << m_x << "\n";
    for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
    {
        if (m_blockCandy[i] == nullptr)
            f << -1 << "\n";
        else
            f << int(m_blockCandy[i]->getType()) << " ";
    }
    f << getScore() << " " << m_gameOver << "\n";
    ret = f.good();
    f.close();
    return ret;
}

bool Game::load(const std::string& input_path)
{
    bool ret = false;
    ret = m_board.load(input_path);
    
    std::ifstream f(input_path);
    if (!f.is_open())
        return false;


    int h, w;

    // saltar l oque ya se habia leido antes
    f >> h >> w;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
        {
            int tmp;
            f >> tmp;
        }

    //Ahora sí
    int y, x = 0;
    f >> y >> x;
    if (!f.good() && !f.eof())
        return false;
    if (y < -1 || x < 0 || y > 9 || x > 9)
        return false;

    m_x = x;
    m_y = y;

    for (int i = 0; i < DEFAULT_BLOCKSIZE; i++)
    {
        int type = -1;
        f >> type;
        m_blockCandy[i] = Candy::parse_new(type);
    }

    f >> m_score;
    f >> m_gameOver;
    ret = f.good();
    f.close();
    return ret;
}

bool Game::operator==(const Game& other) const
{
    bool ret = true;

    if (!(m_board == other.m_board))
        ret = false;

    if (ret && m_y != other.m_y)
        ret = false;
    if (ret && m_x != other.m_x)
        ret = false;
    if (ret && m_score != other.m_score)
        ret = false;


    for (int i = 0; i < DEFAULT_BLOCKSIZE && ret; i++)
    {
        Candy* a = m_blockCandy[i];
        Candy* b = other.m_blockCandy[i];

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
    
    return ret;
}

