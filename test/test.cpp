#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"
#include "test.h"

#include <filesystem>
#include <iostream>
#include <vector>

bool test()
{
    std::cout << "\nRunning test suite:" << std::endl;
    bool allPassed = true;
    #define RUN(test) if (allPassed && !(allPassed = allPassed && (test)())) \
                            ::std::cout << "Failure: `test::" #test << "()`" << ::std::endl;

    RUN(boardTest);
    RUN(constructorTest);
    RUN(shouldExplodeTest);
    RUN(candyParseTest);
    RUN(gameTest);

    if (allPassed)
        std::cout << "All tests passed." << std::endl;

    return allPassed;
}

bool boardTest()
{
    bool ret = true;
    const int SIZE = 10;

    // Test board 2D container
    Candy c(CandyType::TYPE_ORANGE);
    Candy c2(CandyType::TYPE_RED);
    Board b(SIZE, SIZE);
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            b.setCell(&c, i, j);
        }
        b.setCell(&c2, i, i);
    }
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            ret = ret && (b.getCell(i, j)->getType() == (i == j ? c2 : c).getType());

    // Dump and load board
    ret = ret && b.dump(getDataDirPath() + "dump_board.txt");

    Board b2(SIZE, SIZE);
    ret = ret && b2.load(getDataDirPath() + "dump_board.txt");

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            ret = ret && (b.getCell(i, j)->getType() == b2.getCell(i, j)->getType());

    std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    ret = ret && b == b;

    return ret;
}

bool constructorTest()
{
    bool  ok = true;
    Board b(5, 5);

    // Width y height
    if (b.getWidth() != 5)
    {
        std::cout << "Error: getWidth() devuelve " << b.getWidth() << ", esperado 5" << std::endl;
        ok = false;
    }
    if (b.getHeight() != 5)
    {
        std::cout << "Error: getHeight() devuelve " << b.getHeight() << ", esperado 5" << std::endl;
        ok = false;
    }

    // Inicializacion
    for (int y = 0; y < b.getHeight(); y++)
    {
        for (int x = 0; x < b.getWidth(); x++)
        {
            if (b.getCell(x, y) != nullptr)
            {
                std::cout << "Error: celda (" << x << "," << y << ") no es nullptr al inicializar"
                          << std::endl;
                ok = false;
            }
        }
    }

    // setCell y getCell
    Candy c1(CandyType::TYPE_RED);
    Candy c2(CandyType::TYPE_BLUE);
    b.setCell(&c1, 0, 0);
    b.setCell(&c2, 4, 4);

    if (*b.getCell(0, 0) != c1)
    {
        std::cout << "Error: getCell(0,0) no devuelve el candy esperado: " << b.getCell(0, 0)
                  << std::endl;
        ok = false;
    }
    if (*b.getCell(4, 4) != c2)
    {
        std::cout << "Error: getCell(4,4) no devuelve el candy esperado: " << b.getCell(4, 4)
                  << std::endl;
        ok = false;
    }
    return ok;

    // setCell fuera de rango
    b.setCell(&c1, -1, 0);
    b.setCell(&c1, 0, 5);
    b.setCell(&c1, 5, 5);
    if (b.getCell(-1, 0) != nullptr || b.getCell(0, 5) != nullptr || b.getCell(5, 5) != nullptr)
    {
        std::cout << "Error: setCell fuera de rango modificó algo\n";
        ok = false;
    }
    return ok;
}

bool shouldExplodeTest()
{
    bool  ok = true;
    Board b(5, 5);

    // --- Horizontal ---
    Candy cRed(CandyType::TYPE_RED);
    b.setCell(&cRed, 0, 0);
    b.setCell(&cRed, 1, 0);
    b.setCell(&cRed, 2, 0);

    if (!b.shouldExplode(1, 0))
    {
        std::cout << "Error: horizontal no detectado en (1,0)" << std::endl;
        ok = false;
    }
    // --- Vertical ---
    Candy cBlue(CandyType::TYPE_BLUE);
    b.setCell(&cBlue, 0, 1);
    b.setCell(&cBlue, 0, 2);
    b.setCell(&cBlue, 0, 3);

    if (!b.shouldExplode(0, 1))
    {
        std::cout << "Error: vertical no detectado en (0,1)" << std::endl;
        ok = false;
    }
    // --- Diagonal / ---
    Candy cGreen(CandyType::TYPE_GREEN);
    b.setCell(&cGreen, 0, 4);
    b.setCell(&cGreen, 1, 3);
    b.setCell(&cGreen, 2, 2);

    if (!b.shouldExplode(2, 2))
    {
        std::cout << "Error: diagonal / no detectada en (2,2)" << std::endl;
        ok = false;
    }
    // --- Diagonal \ ---
    Candy cYellow(CandyType::TYPE_YELLOW);
    b.setCell(&cYellow, 0, 0);
    b.setCell(&cYellow, 1, 1);
    b.setCell(&cYellow, 2, 2);

    if (!b.shouldExplode(1, 1))
    {
        std::cout << "Error: diagonal \\ no detectada en (1,1)" << std::endl;
        ok = false;
    }
    // --- No explosion ---
    Candy cOrange(CandyType::TYPE_ORANGE);
    b.setCell(&cOrange, 4, 4);

    if (b.shouldExplode(4, 4))
    {
        std::cout << "Error: shouldExplode devolvió true para candy aislado (4,4)" << std::endl;
        ok = false;
    }
    return ok;
}

bool explodeAndDropTest()
{
    bool ok = true;

    // --- Horizontal ---
    {
        Board b(5, 5);
        Candy cRed(CandyType::TYPE_RED);
        Candy cBlue(CandyType::TYPE_BLUE);

        b.setCell(&cRed, 4, 0);
        b.setCell(&cRed, 4, 1);
        b.setCell(&cRed, 4, 2);
        b.setCell(&cBlue, 2, 1);

        std::vector<Candy*> exploded = b.explodeAndDrop();

        if (exploded.size() != 3)
        {
            std::cout << "Error: debería haber explotado 3 candies, explotó " << exploded.size()
                      << std::endl;
            ok = false;
        }

        if (b.getCell(4, 0) != nullptr || b.getCell(4, 1) != nullptr || b.getCell(4, 2) != nullptr)
        {
            std::cout << "Error: fila inferior debería estar vacía después de explotar"
                      << std::endl;
            ok = false;
        }

        if (b.getCell(4, 1) != &cBlue)
        {
            std::cout << "Error: candy azul no cayó correctamente a (4,1)" << std::endl;
            ok = false;
        }
    }

    // --- En cadena ---
    {
        Board b(5, 5);
        Candy cRed(CandyType::TYPE_RED);
        Candy cBlue(CandyType::TYPE_BLUE);

        b.setCell(&cRed, 4, 0);
        b.setCell(&cRed, 4, 1);
        b.setCell(&cRed, 4, 2);
        b.setCell(&cBlue, 3, 1);
        b.setCell(&cBlue, 2, 1);
        b.setCell(&cBlue, 1, 1);

        std::vector<Candy*> exploded = b.explodeAndDrop();

        // 3 reds + 3 blues = 6
        if (exploded.size() != 6)
        {
            std::cout << "Error: cascada debería explotar 6 candies, explotó " << exploded.size()
                      << std::endl;
            ok = false;
        }
    }

    // --- Multiples gruops ---
    {
        Board b(5, 5);
        Candy cRed(CandyType::TYPE_RED);
        Candy cBlue(CandyType::TYPE_BLUE);

        b.setCell(&cRed, 4, 0);
        b.setCell(&cRed, 4, 1);
        b.setCell(&cRed, 4, 2);
        b.setCell(&cBlue, 3, 0);
        b.setCell(&cBlue, 3, 1);
        b.setCell(&cBlue, 3, 2);

        std::vector<Candy*> exploded = b.explodeAndDrop();

        if (exploded.size() != 6)
        {
            std::cout << "Error: grupos múltiples deberían explotar 6 candies, explotó "
                      << exploded.size() << std::endl;
            ok = false;
        }
    }

    // --- Cap explosio ---
    {
        Board b(3, 3);
        Candy cRed(CandyType::TYPE_RED);
        Candy cBlue(CandyType::TYPE_BLUE);
        Candy cGreen(CandyType::TYPE_GREEN);

        // No matches
        b.setCell(&cRed, 0, 0);
        b.setCell(&cBlue, 0, 1);
        b.setCell(&cGreen, 0, 2);

        std::vector<Candy*> exploded = b.explodeAndDrop();

        if (exploded.size() != 0)
        {
            std::cout << "Error: no debería haber explosiones, explotó " << exploded.size()
                      << std::endl;
            ok = false;
        }

        if (b.getCell(0, 0) != &cRed || b.getCell(0, 1) != &cBlue)
        {
            std::cout << "Error: tablero cambió sin explosiones" << std::endl;
            ok = false;
        }
    }

    return ok;
}

bool candyParseTest()
{
    bool ret = true;
    for (int i = 0; i < NUM_CANDYTYPES; i++)
        ret = ret && Candy::parse_new(i)->getType() == static_cast<CandyType>(i);
    return ret;
}

bool gameTest()
{
    const int SIZE = 10;
    bool ret = true;
    Game game;

    ret = ret && game == game;
    auto c = new Candy(CandyType::TYPE_BLUE);
    std::vector<Candy*> vec(3, c);
    game.scoreUpdate(vec);
    ret = ret && game.getScore() == 1000;

    int acc = 1000;
    for (int i = 4; i <= 6; i++)
    {
        vec.push_back(c);
        game.scoreUpdate(vec);
        acc += 1000 * (1 << i - 3);
    }
    ret = ret && game.getScore() == acc;

    return ret;
}
