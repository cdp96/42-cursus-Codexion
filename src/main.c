#include "codexion.h"

int	main(int argc, char **argv)
{
	t_simulation	sim;

	if (parse_input(&sim, argc, argv) != 0)
		return (1);
	if (init_simulation(&sim) != 0)
		return (1);
	if (run_simulation(&sim) != 0)
	{
		cleanup_simulation(&sim);
		return (1);
	}
	cleanup_simulation(&sim);
	return (0);
}
