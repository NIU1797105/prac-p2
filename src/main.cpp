/// Project entry point
#include "../test/test.h"
#include "game.h"

#include <iostream>

int main(int argc, const char* argv[])
{
    if (!constructorTest())
        cout << "Test del constructor no superado" << endl;
    else
        cout << "Test constructor pasado" << endl;
    if (!shouldExplodeTest())
        cout << "Test de la función shouldExplode no superado" << endl;
    else
        cout << "Test de la función shouldExplode pasado" << endl;
    if (!test())
    {
        std::cout << "Tests NO superados. Los has definido ya?" << std::endl;
    }
    else
    {
        std::cout << "Test superados." << std::endl;
    }

    // The following code runs the graphic part
    Game game;
    // Run until ESC is pressed
    game.run();
    return 0;
}
