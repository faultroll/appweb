/**
 * \file: tick.h
 * \brief: tick(TICK) module. supply the timer services for project.
 */

#include "project.h"

#ifdef _TICK_H_

TICK system_tick;

int fd_jiffies = -1; ///< the global file device to access to the jiffies of Linux kernel.

bool tick_open(void) {
	if (fd_jiffies < 0) {
		fd_jiffies = open("/dev/" DEVICE_JIFFIES_NAME, 0); ///< \todo : option for open file
		if (fd_jiffies < 0) {
			sysPerror("/dev/" DEVICE_JIFFIES_NAME);
			return 0;
		}
	}
	system_tick = (TICK) ioctl(fd_jiffies, 0, 0);
	// for more 2 times
	system_tick = (TICK) ioctl(fd_jiffies, 0, 0);
	system_tick = (TICK) ioctl(fd_jiffies, 0, 0);
	return 1;
}

inline void tick_run(void) {
	system_tick = (TICK) ioctl(fd_jiffies, 0, 0);
}

void tick_close(void) {
	if (fd_jiffies > 0)
		close(fd_jiffies);
}

void tick_delay_sec(unsigned int seconds) {
	TICK next_tick;
	tick_run(); // Update the system_tick
	TICK_SET(next_tick, seconds);
	while (TICK_BEFORE(next_tick))
		tick_run();
}

void tick_delay_tick(unsigned int ticks) {
	TICK next_tick;
	tick_run(); // Update the system_tick
	next_tick = system_tick + ticks;
	while (TICK_BEFORE(next_tick))
		tick_run();
}

#endif //_TICK_H_
