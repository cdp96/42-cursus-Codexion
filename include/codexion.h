#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# include <limits.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}	t_scheduler;

typedef struct s_coder			t_coder;
typedef struct s_dongle			t_dongle;
typedef struct s_simulation		t_simulation;
typedef struct s_heap_node		t_heap_node;
typedef struct s_heap			t_heap;

struct	s_heap_node
{
	int		coder_id;
	long	priority;
};

struct s_heap
{
	t_heap_node		*nodes;
	int				size;
	int				capacity;
};

struct	s_coder
{
	int					id;
	long				last_compile_start;
	int					compile_count;
	long				edf_key;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	pthread_t			thread;
	pthread_mutex_t		compile_state_mutex;
	t_simulation		*sim;
};

struct	s_dongle
{
	int					id;
	long				last_released;
	pthread_mutex_t		mutex;
	int					dongle_held;
	long				fifo_ticket;
	pthread_cond_t		waiters_sleep;
	t_heap				wait_line;
};

struct	s_simulation
{
	long				n_coders;
	long				t_burnout;
	long				t_compile;
	long				t_debug;
	long				t_refactor;
	long				n_compiles;
	long				dongle_cooldown;
	int					scheduler;
	long				start;
	int					stop_flag;
	pthread_mutex_t		log_mutex;
	pthread_mutex_t		stop_mutex;
	t_coder				*coder;
	t_dongle			*dongle;
	pthread_t			monitor;
};

// parsing.c
int		parse_input(t_simulation *sim, int argc, char **argv);

// initialization
int		init_simulation(t_simulation *sim);

// cleanup
void	destroy_dongles(t_simulation *sim, long count);
void	destroy_coders(t_simulation *sim, long count);
void	cleanup_simulation(t_simulation *sim);

// running
int		run_simulation(t_simulation *sim);
int		simulation_stopped(t_simulation *sim);

// monitor
void	*monitor_cycle(void *arg);

// dongle basic management
int		take_dongles(t_coder *coder, t_simulation *sim);
void	release_dongle(t_dongle *dongle);
void	drop_dongle(t_dongle *dongle);
void	*lone_coder(t_coder *coder, t_simulation *sim);

// dongle acquisition
int		acquire_both(t_simulation *sim, t_dongle *first, t_dongle *second,
			t_coder *coder);
int		try_grab_dongle(t_simulation *sim, t_dongle *dongle, t_coder *coder);
int		grab_dongle(t_simulation *sim, t_dongle *dongle, t_coder *coder);

// coder state
void	set_compile_start(t_coder *coder);
void	add_compile_count(t_coder *coder);
int		compiles_done(t_coder *coder);
void	set_edf_key(t_coder *coder, t_simulation *sim);

// time
long	get_time_ms(void);
void	set_stop(t_simulation *sim);
void	sim_sleep(t_simulation *sim, long ms);

// utils
void	log_state(t_simulation *sim, int id, char *state);

// heap
int		heap_init(t_heap *heap, int capacity);
void	heap_free(t_heap *heap);
int		heap_insert(t_heap *heap, t_heap_node node);
int		heap_extract(t_heap *heap, t_heap_node *first);
int		top_heap(t_heap *heap);

// heap utils
void	swap_nodes(t_heap_node *a, t_heap_node *b);
void	bubble_down(t_heap *heap, int i);
void	bubble_up(t_heap *heap, int i);
int		heap_remove_id(t_heap *heap, int coder_id);

#endif
