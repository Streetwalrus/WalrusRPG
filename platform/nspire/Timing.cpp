#include "Timing.h"
#include "Timers.h"

#define TIMING WalrusRPG::Timing
#define TIMER 1
using namespace Nspire;

void TIMING::init()
{
    Timers::init(TIMER);
    Timers::mode(TIMER, true, false, false, 1, true);
    Timers::load(TIMER, 0xFFFFFFFF);
}

void TIMING::deinit()
{
    Timers::restore(TIMER);
}

unsigned TIMING::gettime()
{
    return 0xFFFFFFFF - Timers::read(TIMER);
}
