/// Project entry point
#include "../test/test.h"
#include "game.h"
#include "signals.h"

int main(int argc, const char* argv[])
{
    #ifndef GRADESCOPE
    setup_signals();
    #endif
    test();
    // The following code runs the graphic part
    Sound_Init();
    Game game;
    if (argc > 1)
        game.load(argv[1]);

    // Run until ESC is pressed
    game.run();
    return 0;
}
