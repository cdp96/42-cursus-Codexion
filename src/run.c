#include "codexion.h"

int	simulation_stopped(t_simulation *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop_flag;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

/*
	function to follow a coder's routine:
	compile → debug → refactor
*/
static void	*coder_cycle(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	if (coder->left_dongle == coder->right_dongle)
		return (lone_coder(coder, sim));
	while (!simulation_stopped(sim) && compiles_done(coder) < sim->n_compiles)
	{
		if (take_dongles(coder, sim) != 0)
			break ;
		set_compile_start(coder);
		log_state(sim, coder->id, "is compiling");
		add_compile_count(coder);
		sim_sleep(sim, sim->t_compile);
		release_dongle(coder->left_dongle);
		release_dongle(coder->right_dongle);
		log_state(sim, coder->id, "is debugging");
		sim_sleep(sim, sim->t_debug);
		log_state(sim, coder->id, "is refactoring");
		sim_sleep(sim, sim->t_refactor);
	}
	return (NULL);
}

/*
	function to join all the succesfully
	created threads from launch_coders().
*/
static void	join_coders(t_simulation *sim, long count)
{
	long	i;

	i = 0;
	while (i < count)
	{
		pthread_join(sim->coder[i].thread, NULL);
		i++;
	}
}

/*
	function to create a thread per coder
	that will run the coder routine.
*/
static int	launch_coders(t_simulation *sim)
{
	long	i;

	i = 0;
	while (i < sim->n_coders)
	{
		if (pthread_create(&sim->coder[i].thread, NULL,
				coder_cycle, &sim->coder[i]) != 0)
		{
			set_stop(sim);
			join_coders(sim, i);
			return (-1);
		}
		i++;
	}
	return (0);
}

/*
	main function that runs the whole simulation.
	Launches one thread per coder + a monitor thread,
	then waits for them all to finish. The monitor ends
	the run by setting the stop flag (completion or burnout),
	after which the coders gradually stop.
*/
int	run_simulation(t_simulation *sim)
{
	if (launch_coders(sim) != 0)
		return (-1);
	if (pthread_create(&sim->monitor, NULL, monitor_cycle, sim) != 0)
	{
		set_stop(sim);
		join_coders(sim, sim->n_coders);
		return (-1);
	}
	pthread_join(sim->monitor, NULL);
	join_coders(sim, sim->n_coders);
	return (0);
}
