#include "codexion.h"

/*
	helper function to release a dongle
	on failed acquisition without
	timestamping, so cooldown here does
	not count.
*/
void	drop_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->dongle_held = 0;
	pthread_cond_broadcast(&dongle->waiters_sleep);
	pthread_mutex_unlock(&dongle->mutex);
}

/*
	helper function to release a dongle
	and time stamp its last_released time
	so the cooldown can start.
*/
void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->dongle_held = 0;
	dongle->last_released = get_time_ms();
	pthread_cond_broadcast(&dongle->waiters_sleep);
	pthread_mutex_unlock(&dongle->mutex);
}

/*
	locks both dongles in ascending id order (to prevent circular wait),
	logging each grab in the order it happens.
*/
int	take_dongles(t_coder *coder, t_simulation *sim)
{
	t_dongle	*first;
	t_dongle	*second;

	set_edf_key(coder, sim);
	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first->id > second->id)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	if (acquire_both(sim, first, second, coder) != 0)
		return (-1);
	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "has taken a dongle");
	return (0);
}

/*
	helper function for handling a single coder dongle path.
	It has only one dongle, so it can never compile.
	It takes the one dongle and waits until the monitor stops the run
	(which happens when its burnout deadline passes).
*/
void	*lone_coder(t_coder *coder, t_simulation *sim)
{
	pthread_mutex_lock(&coder->left_dongle->mutex);
	log_state(sim, coder->id, "has taken a dongle");
	while (!simulation_stopped(sim))
		usleep(500);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	return (NULL);
}
