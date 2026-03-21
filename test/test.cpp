#include <filesystem>
#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"


bool test()
{
    // Test board 2D container
    Candy c(CandyType::TYPE_ORANGE);
    Board b(10, 10);
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            

        }
    }
    b.setCell(&c, 0, 0);
    if (b.getCell(0, 0) != &c)
    {
        return false;
    }

    // Dump and load board
    {
        Board b2(10, 10);
        if (!b.dump(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (!b2.load(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (b2.getCell(0, 0)->getType() != c.getType())
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    }

    // Dump and load game
    {
        Game g;
        Controller cont;
        g.update(cont);
        if (!g.dump(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        Game g2;
        if (!g2.load(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        if (g != g2)
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }

    return true;
}
bool constructorTest()
{
    bool ok = true;
    Board b(5,5);

    //Width y height
    if (b.getWidth() != 5)
    {
        cout << "Error: getWidth() devuelve " << b.getWidth() << ", esperado 5" << endl;
        ok = false;
    }
    if (b.getHeight() != 5)
    {
        cout << "Error: getHeight() devuelve " << b.getHeight() << ", esperado 5" << endl;
        ok = false;
    }

    //Inicializacion
    for (int y = 0; y < b.getHeight(); y++) {
        for (int x = 0; x < b.getWidth(); x++) {
            if (b.getCell(x, y) != nullptr) {
                cout << "Error: celda (" << x << "," << y << ") no es nullptr al inicializar" << endl;
                ok = false;
            }
        }
    }

    //setCell y getCell
    Candy c1(CandyType::TYPE_RED);
    Candy c2(CandyType::TYPE_BLUE);
    b.setCell(&c1, 0, 0);
    b.setCell(&c2, 4, 4);

    if (b.getCell(0, 0) != &c1) {
        cout << "Error: getCell(0,0) no devuelve el candy esperado: " << b.getCell(0, 0) << endl;
        ok = false;
    }
    if (b.getCell(4, 4) != &c2) {
        cout << "Error: getCell(4,4) no devuelve el candy esperado: " << b.getCell(4, 4) << endl;
        ok = false;
    }
    return ok;

    //setCell fuera de rango
    b.setCell(&c1, -1, 0);
    b.setCell(&c1, 0, 5);
    b.setCell(&c1, 5, 5);
    if (b.getCell(-1, 0) != nullptr || b.getCell(0, 5) != nullptr || b.getCell(5, 5) != nullptr) {
        std::cout << "Error: setCell fuera de rango modificó algo\n";
        ok = false;
    }
}

bool shouldExplodeTest()
{
    bool ok = true;
    Board b(5, 5);

    // --- Horizontal ---
    Candy cRed(CandyType::TYPE_RED);
    b.setCell(&cRed, 0, 0);
    b.setCell(&cRed, 1, 0);
    b.setCell(&cRed, 2, 0);

    if (!b.shouldExplode(1, 0)) {
        cout << "Error: horizontal no detectado en (1,0)" << endl;
        ok = false;
    }
    else
        cout << "Horizontal detectado en (1,0)" << endl;

    // --- Vertical ---
    Candy cBlue(CandyType::TYPE_BLUE);
    b.setCell(&cBlue, 0, 1);
    b.setCell(&cBlue, 0, 2);
    b.setCell(&cBlue, 0, 3);

    if (!b.shouldExplode(0, 1)) {
        cout << "Error: vertical no detectado en (0,1)" << endl;
        ok = false;
    }
    else
        cout << "Vertical detectado en (0,2)" << endl;

    // --- Diagonal / ---
    Candy cGreen(CandyType::TYPE_GREEN);
    b.setCell(&cGreen, 0, 4);
    b.setCell(&cGreen, 1, 3);
    b.setCell(&cGreen, 2, 2);

    if (!b.shouldExplode(2, 2)) {
        cout << "Error: diagonal / no detectada en (2,2)" << endl;
        ok = false;
    }
    else
        cout << "Diagonal / detectado en (2,2)" << endl;

    // --- Diagonal \ ---
    Candy cYellow(CandyType::TYPE_YELLOW);
    b.setCell(&cYellow, 0, 0);
    b.setCell(&cYellow, 1, 1);
    b.setCell(&cYellow, 2, 2);

    if (!b.shouldExplode(1, 1)) {
        cout << "Error: diagonal \ no detectada en (1,1)" << endl;
        ok = false;
    }
    else
        cout << "Diagonal \ detectado en (1,1)" << endl;

    // --- No explosion ---
    Candy cOrange(CandyType::TYPE_ORANGE);
    b.setCell(&cOrange, 4, 4);

    if (b.shouldExplode(4, 4)) {
        cout << "Error: shouldExplode devolvió true para candy aislado (4,4)" << endl;
        ok = false;
    }
    else
        cout << "Nada explotó =)" << endl;

    return ok;
}