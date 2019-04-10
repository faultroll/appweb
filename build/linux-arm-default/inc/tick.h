/**
 * \file: tick.h
 * \brief: tick(TICK) module. supply the timer services for project.
 */

#ifndef _TICK_H_
#define _TICK_H_

#include "projectdefs.h"
#include "drivers.h"

#ifndef SYSTEM_TICK_PER_SECOND
#ifdef BOARD_OF_VIRTUAL
#define SYSTEM_TICK_PER_SECOND 1000
#else
#define SYSTEM_TICK_PER_SECOND 200
#endif
#endif //SYSTEM_TICK_PER_SECOND

/**
 * \note: tick must be the data type: TICK, and the seconds is 0.01 * N, N is an integer
 */
#define TICK_SET(tick, seconds) tick = system_tick + (int)((seconds) * SYSTEM_TICK_PER_SECOND)
#define TICK_SET_TICK(tick, n) tick = system_tick + (n)
#define TICK_BEFORE(tick) (tick - system_tick > 0) // tick must be the data type: TICK
#define TICK_AFTER(tick) (tick - system_tick <= 0) // tick must be the data type: TICK
//typedef int TICK; // defined in "projectdefs.h"

extern TICK system_tick;

bool tick_open(void);
void tick_close(void);
#if 1 // when the system_tick is need to be updated by call tick_run() in the same thread
inline void tick_run();
#else // when the system_tick is updated by different thread
#define tick_run()
#endif
void tick_delay_sec(unsigned int seconds); // NOTE: Fractional not allowed.
void tick_delay_tick(unsigned int ticks);

#endif // _TICK_H_
