#include "codexion.h"

/*
	function to "stamp" the exact moment compiling begins,
	it goes through the compile_state_mutex guard because
	the monitor will read it from another thread.
*/
void	set_compile_start(t_coder *coder)
{
	pthread_mutex_lock(&coder->compile_state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->compile_state_mutex);
}

/*
	function to just count coder compiles but guarded
	with a mutext because the monitor will read it
	from another thread.
*/
void	add_compile_count(t_coder *coder)
{
	pthread_mutex_lock(&coder->compile_state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->compile_state_mutex);
}

/*
	function to return a coder's compile count,
	reads under its lock since the monitor reads it too.
*/
int	compiles_done(t_coder *coder)
{
	int	count;

	pthread_mutex_lock(&coder->compile_state_mutex);
	count = coder->compile_count;
	pthread_mutex_unlock(&coder->compile_state_mutex);
	return (count);
}

/*
	helper function to compute the coder deadline for edf.
*/
void	set_edf_key(t_coder *coder, t_simulation *sim)
{
	pthread_mutex_lock(&coder->compile_state_mutex);
	coder->edf_key = coder->last_compile_start + sim->t_burnout;
	pthread_mutex_unlock(&coder->compile_state_mutex);
}
