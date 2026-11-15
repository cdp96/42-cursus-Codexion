#include "codexion.h"

/*
	time helper function to return time in milliseconds.
*/
long	get_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000L + time.tv_usec / 1000);
}

/*
	helper function to enable the stop flag.
*/
void	set_stop(t_simulation *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop_flag = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}

/*
	sleep (wait execution) for milliseconds, but in short increments so the coder
	can bail early if the simulation stops.
*/
void	sim_sleep(t_simulation *sim, long ms)
{
	long	deadline;

	deadline = get_time_ms() + ms;
	while (get_time_ms() < deadline)
	{
		if (simulation_stopped(sim))
			return ;
		usleep(500);
	}
}
