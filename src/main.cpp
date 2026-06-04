/// Project entry point
#include "../test/test.h"
#include "game.h"
#include "signals.h"
#include "sound_mgr.h"

int main(int argc, const char* argv[])
{
    test();
    SoundManager::SoundInit();
    setup_signals();
    // The following code runs the graphic part
    Game game;
    if (argc > 1)
        game.load(argv[1]);

    // Run until ESC is pressed
    game.run();
    return 0;
}
