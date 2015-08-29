#include <libndls.h>
#include "StateMachine.h"
#include "Timers.h"
#include "Graphics.h"
#include "render/Text.h"
#include "version.h"

using namespace WalrusRPG::Graphics;
using namespace WalrusRPG::States;
using namespace WalrusRPG::Timers;

#define STATEMACHINE WalrusRPG::StateMachine

STATEMACHINE::StateMachine(State *state)
{
    push(state);
}

STATEMACHINE::~StateMachine()
{
}

void STATEMACHINE::push(State *state)
{
    stack.push_back(state);
}

void STATEMACHINE::pop()
{
    delete stack.back();
    stack.pop_back();
}

void STATEMACHINE::run()
{
    Timers::mode(0, true, false, false, 1, true);
    Timers::load(0, 0);
    const unsigned loop_time = TIMER_FREQ / 60;
    unsigned loop_next = loop_time;
    unsigned last_update = 0, update_stamp, update_time;
    unsigned last_frame = 0, frame_stamp, frame_time;

    while (!stack.empty())
    {
        update_stamp = Timers::read(0);
        update_time = update_stamp - last_update;
        stack.back()->update(update_time);
        last_update = update_stamp;

        if (Timers::read(0) < loop_next)
        {
            frame_stamp = Timers::read(0);
            frame_time = frame_stamp - last_frame;
            stack.back()->render(frame_time);
            last_frame = frame_stamp;

            Text::print_format(0, 0, "WalrusRPG test build %s", git_version);
            Text::print_format(0, 240 - 8, "%ufps, %uups", TIMER_FREQ / frame_time,
                               TIMER_FREQ / update_time);
            Graphics::buffer_swap_render();
        }

        if (isKeyPressed(KEY_NSPIRE_ESC))
        {
            while (isKeyPressed(KEY_NSPIRE_ESC))
                ;
            this->pop();
        }

        while (Timers::read(0) < loop_next)
            ;
        loop_next += loop_time;
    }
}
