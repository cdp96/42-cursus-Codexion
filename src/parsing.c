#include "codexion.h"

/*
	customized atol with strict validation for rejecting negative values
	and overflow check.
*/
static long	ft_custom_atol(char *n)
{
	long	nb;

	nb = 0;
	while (*n == ' ' || (9 <= *n && *n <= 13))
		n++;
	if (*n == '+')
		n++;
	if (!*n)
		return (-1);
	while ('0' <= *n && *n <= '9')
	{
		if (nb > (LONG_MAX - *n + '0') / 10)
			return (-1);
		nb = 10 * nb + (*n - '0');
		n++;
	}
	if (*n)
		return (-1);
	return (nb);
}

/*
	helper function to validate each struct field value
	against invalid rules.
*/
static int	check_input(t_simulation *sim)
{
	if (sim->n_coders < 1)
		return (-1);
	if (sim->t_burnout < 1)
		return (-1);
	if (sim->t_compile < 0)
		return (-1);
	if (sim->t_debug < 0)
		return (-1);
	if (sim->t_refactor < 0)
		return (-1);
	if (sim->n_compiles < 0)
		return (-1);
	if (sim->dongle_cooldown < 0)
		return (-1);
	return (0);
}

/*
	helper function to check valid scheduler type.
*/
static int	parse_scheduler(t_simulation *sim, char *s)
{
	if (!strcmp(s, "fifo"))
		sim->scheduler = SCHEDULER_FIFO;
	else if (!strcmp(s, "edf"))
		sim->scheduler = SCHEDULER_EDF;
	else
		return (-1);
	return (0);
}

/*
	main parser orchestrator with output error messages.
*/
int	parse_input(t_simulation *sim, int argc, char **argv)
{
	if (argc != 9)
	{
		fprintf(stderr, "Error: expected 8 arguments\n");
		return (-1);
	}
	sim->n_coders = ft_custom_atol(argv[1]);
	sim->t_burnout = ft_custom_atol(argv[2]);
	sim->t_compile = ft_custom_atol(argv[3]);
	sim->t_debug = ft_custom_atol(argv[4]);
	sim->t_refactor = ft_custom_atol(argv[5]);
	sim->n_compiles = ft_custom_atol(argv[6]);
	sim->dongle_cooldown = ft_custom_atol(argv[7]);
	if (check_input(sim) == -1)
	{
		fprintf(stderr, "Error: invalid argument(s)\n");
		return (-1);
	}
	if (parse_scheduler(sim, argv[8]) == -1)
	{
		fprintf(stderr, "Error: scheduler must be fifo or edf\n");
		return (-1);
	}
	return (0);
}
