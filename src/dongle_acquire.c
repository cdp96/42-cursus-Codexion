#include "codexion.h"

/*
	builds an absolute wake-up time (now + ms) for pthread_cond_timedwait.
*/
static void	set_timeout(struct timespec *ts, long ms)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	ts->tv_sec = now.tv_sec + (ms / 1000);
	ts->tv_nsec = (now.tv_usec * 1000) + ((ms % 1000) * 1000000);
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec += 1;
		ts->tv_nsec -= 1000000000;
	}
}

/*
	function to block for the first dongle,
	try the second, and if that fails,
	put the first back and retry.
	returns 0 holding both dongles,
	or -1 if the simulation stopped.
*/
int	acquire_both(t_simulation *sim, t_dongle *first, t_dongle *second,
		t_coder *coder)
{
	while (!simulation_stopped(sim))
	{
		if (grab_dongle(sim, first, coder) != 0)
			return (-1);
		if (try_grab_dongle(sim, second, coder) == 0)
			return (0);
		drop_dongle(first);
		usleep(500);
	}
	return (-1);
}

/*
	function to check whether a coder may take the dongle right now.
	These three things must meet: the coder is first in the wait line,
	the dongle is free, and its cooldown has passed. Called while holding
	the dongle mutex, so reading its state is safe.
*/
static int	can_take_dongle(t_simulation *sim, t_dongle *dongle, t_coder *coder)
{
	if (top_heap(&dongle->wait_line) != coder->id)
		return (0);
	if (dongle->dongle_held)
		return (0);
	if (get_time_ms() < dongle->last_released + sim->dongle_cooldown)
		return (0);
	return (1);
}

/*
	helper function to check if
	the coder took the dongle right now,
	return 0, -1 if it couldn't.
*/
int	try_grab_dongle(t_simulation *sim, t_dongle *dongle, t_coder *coder)
{
	t_heap_node		node;

	pthread_mutex_lock(&dongle->mutex);
	node.coder_id = coder->id;
	if (sim->scheduler == SCHEDULER_EDF)
		node.priority = coder->edf_key;
	else
		node.priority = dongle->fifo_ticket++;
	heap_insert(&dongle->wait_line, node);
	if (can_take_dongle(sim, dongle, coder))
	{
		heap_extract(&dongle->wait_line, &node);
		dongle->dongle_held = 1;
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	else
	{
		heap_remove_id(&dongle->wait_line, coder->id);
		pthread_mutex_unlock(&dongle->mutex);
		return (-1);
	}
}

/*
	function to make a coder take a dongle, waiting its turn if needed.
	The coder joins the dongle's wait line, then sleeps on the condition
	variable until can_take_dongle() is true. It uses a short timed wait
	so it wakes regularly to re-check the stop flag: if the sim stopped,
	the coder leaves the line and gives up instead of waiting forever.
	Returns 0 if it took the dongle, or -1 if the sim stopped first.
*/
int	grab_dongle(t_simulation *sim, t_dongle *dongle, t_coder *coder)
{
	t_heap_node		node;
	struct timespec	ts;

	pthread_mutex_lock(&dongle->mutex);
	node.coder_id = coder->id;
	if (sim->scheduler == SCHEDULER_EDF)
		node.priority = coder->edf_key;
	else
		node.priority = dongle->fifo_ticket++;
	heap_insert(&dongle->wait_line, node);
	while (!can_take_dongle(sim, dongle, coder))
	{
		if (simulation_stopped(sim))
		{
			heap_remove_id(&dongle->wait_line, coder->id);
			pthread_mutex_unlock(&dongle->mutex);
			return (-1);
		}
		set_timeout(&ts, 10);
		pthread_cond_timedwait(&dongle->waiters_sleep, &dongle->mutex, &ts);
	}
	heap_extract(&dongle->wait_line, &node);
	dongle->dongle_held = 1;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}
