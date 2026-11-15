#include "codexion.h"

/*
	function for coder initialization.
*/
static int	init_coders(t_simulation *sim)
{
	long	i;
	long	left;

	sim->coder = malloc(sizeof(t_coder) * sim->n_coders);
	if (!sim->coder)
		return (-1);
	i = 0;
	while (i < sim->n_coders)
	{
		sim->coder[i].id = i + 1;
		sim->coder[i].last_compile_start = sim->start;
		sim->coder[i].compile_count = 0;
		sim->coder[i].edf_key = 0;
		if (pthread_mutex_init(&sim->coder[i].compile_state_mutex, NULL) != 0)
		{
			destroy_coders(sim, i);
			return (-1);
		}
		sim->coder[i].right_dongle = &sim->dongle[i];
		left = (i - 1 + sim->n_coders) % sim->n_coders;
		sim->coder[i].left_dongle = &sim->dongle[left];
		sim->coder[i].sim = sim;
		i++;
	}
	return (0);
}

/*
	initializes one dongle's resources.
*/
static int	init_one_dongle(t_dongle *dongle, int id, t_simulation *sim)
{
	dongle->id = id;
	dongle->last_released = sim->start - sim->dongle_cooldown;
	dongle->dongle_held = 0;
	dongle->fifo_ticket = 0;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (-1);
	if (pthread_cond_init(&dongle->waiters_sleep, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
		return (-1);
	}
	if (heap_init(&dongle->wait_line, sim->n_coders) != 0)
	{
		pthread_cond_destroy(&dongle->waiters_sleep);
		pthread_mutex_destroy(&dongle->mutex);
		return (-1);
	}
	return (0);
}

/*
	function for dongle initialization.
*/
static int	init_dongles(t_simulation *sim)
{
	long	i;

	sim->dongle = malloc(sizeof(t_dongle) * sim->n_coders);
	if (!sim->dongle)
		return (-1);
	i = 0;
	while (i < sim->n_coders)
	{
		if (init_one_dongle(&sim->dongle[i], i + 1, sim) != 0)
		{
			destroy_dongles(sim, i);
			return (-1);
		}
		i++;
	}
	return (0);
}

/*
	main simulation initialization.
*/
int	init_simulation(t_simulation *sim)
{
	sim->start = get_time_ms();
	sim->stop_flag = 0;
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (-1);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (-1);
	if (init_dongles(sim) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (-1);
	}
	if (init_coders(sim) != 0)
	{
		destroy_dongles(sim, sim->n_coders);
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (-1);
	}
	return (0);
}
