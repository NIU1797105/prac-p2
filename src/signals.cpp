#include <csignal>
#include <cstdlib>

void setup_signals()
{
    std::signal(SIGINT, [](int _sig) { std::exit(130); });
}
