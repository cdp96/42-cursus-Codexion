#include "codexion.h"

/*
	helper function for freeing the dongle array
	as well as all their mutexes.
*/
void	destroy_dongles(t_simulation *sim, long count)
{
	long	i;

	i = 0;
	while (i < count)
	{
		heap_free(&sim->dongle[i].wait_line);
		pthread_cond_destroy(&sim->dongle[i].waiters_sleep);
		pthread_mutex_destroy(&sim->dongle[i].mutex);
		i++;
	}
	free(sim->dongle);
}

/*
	helper function for freeing the coder array and
	destroying their compile_state_mutex.
*/
void	destroy_coders(t_simulation *sim, long count)
{
	long	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->coder[i].compile_state_mutex);
		i++;
	}
	free(sim->coder);
}

/*
	function for freeing all the coder and dongle arrays
	as well as the log mutex.
*/
void	cleanup_simulation(t_simulation *sim)
{
	destroy_coders(sim, sim->n_coders);
	destroy_dongles(sim, sim->n_coders);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}
