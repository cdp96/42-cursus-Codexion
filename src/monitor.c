#include "codexion.h"

/*
	function to check if every coder reached n compiles.
	It accesses the coder compile_code guarded by the
	compile_state_mutex lock.
*/
static int	coders_done(t_simulation *sim)
{
	long	i;

	i = 0;
	while (i < sim->n_coders)
	{
		if (compiles_done(&sim->coder[i]) < sim->n_compiles)
			return (0);
		i++;
	}
	return (1);
}

/*
	function to detect coder burnout.
	For each coder, we read last_compile_start
	and check the compile count under its
	compile_state_mutex, we also keep tabs
	on the elapsed time since last compile
	to know if a coder has completed the
	required compiles. We then compare with
	the designated burnout time, if bigger,
	it will return the id of the burnt out coder,
	if not, just -1 (none burned).
*/
static int	check_burnout(t_simulation *sim)
{
	long	i;
	long	last_compile;
	long	time_passed;
	int		count;

	i = 0;
	while (i < sim->n_coders)
	{
		pthread_mutex_lock(&sim->coder[i].compile_state_mutex);
		last_compile = sim->coder[i].last_compile_start;
		count = sim->coder[i].compile_count;
		pthread_mutex_unlock(&sim->coder[i].compile_state_mutex);
		time_passed = get_time_ms() - last_compile;
		if (count < sim->n_compiles && time_passed > sim->t_burnout)
			return (i);
		i++;
	}
	return (-1);
}

/*
	function that gets called when a coder burned out
	to output the burnout print log and set the flag for
	stopping the simulation.
*/
static void	report_burnout(t_simulation *sim, long i)
{
	pthread_mutex_lock(&sim->log_mutex);
	set_stop(sim);
	printf("%ld %d burned out\n", get_time_ms() - sim->start, sim->coder[i].id);
	pthread_mutex_unlock(&sim->log_mutex);
}

/*
	main function to orchestrate the monitor thread.
	Loops over all coders checking the stop conditions
	and sets the stop flag when the run should end.
	It waits only a short time between checks, to react fast.
*/
void	*monitor_cycle(void *arg)
{
	t_simulation	*sim;
	long			is_burnt;

	sim = (t_simulation *)arg;
	while (!simulation_stopped(sim))
	{
		is_burnt = check_burnout(sim);
		if (is_burnt != -1)
		{
			report_burnout(sim, is_burnt);
			return (NULL);
		}
		if (coders_done(sim))
		{
			set_stop(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
