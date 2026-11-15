/*
	Standalone proof for the EDF tie-break re-code.
	Checks heap.c + heap_utils.c -- no sim, no threads.
	It inserts coders that all share the same deadline (the exact situation
	at t=0, when nobody has compiled yet and every edf_key is identical),
	then extracts them in priority order and prints who is served first.
	BEFORE the recode : ties come out in arbitrary/heap order.
	AFTER  the recode : on equal deadlines the HIGHER coder_id comes first.

	build: cc -Wall -Wextra -Werror -I./include src/heap/.c -o tie_break_demo
	run  : ./tie_break_demo
 */
#include "codexion.h"
#include <stdio.h>

static void	insert_coder(t_heap *h, int id, long deadline)
{
	t_heap_node	node;

	node.coder_id = id;
	node.priority = deadline;
	heap_insert(h, node);
}

int	main(void)
{
	t_heap		h;
	t_heap_node	out;
	int			i;

	heap_init(&h, 16);
	/* five coders, all with the same deadline 1000 */
	i = 1;
	while (i <= 5)
	{
		insert_coder(&h, i, 1000);
		i++;
	}
	printf("equal deadlines (all are 1000), service order: ");
	while (heap_extract(&h, &out) == 0)
		printf("%d ", out.coder_id);
	printf("\n  -> after the recode should be read in correct reverse order\n");

	/* mixed: coder 2 is in real danger (earliest), the rest tie */
	insert_coder(&h, 1, 1000);
	insert_coder(&h, 4, 1000);
	insert_coder(&h, 2, 400);
	insert_coder(&h, 3, 1000);
	insert_coder(&h, 5, 1000);
	printf("\n mixed (c2 earliest, c1/c3/c4/c5 tie), service order: ");
	while (heap_extract(&h, &out) == 0)
		printf("%d ", out.coder_id);
	printf("\n  -> with tie, both should be: 2 5 4 3 1\n");
	heap_free(&h);
	return (0);
}
