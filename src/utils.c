#include "codexion.h"

/*
	helper function for output log prints.
	time / coder id / coder state,
	eg: "203 1 is compiling"
	however this function DOES NOT output
	the burnout state.
*/
void	log_state(t_simulation *sim, int id, char *state)
{
	long	timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	if (simulation_stopped(sim))
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return ;
	}
	timestamp = get_time_ms() - sim->start;
	printf("%ld %d %s\n", timestamp, id, state);
	pthread_mutex_unlock(&sim->log_mutex);
}
