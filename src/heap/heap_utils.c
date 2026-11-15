#include "codexion.h"

/*
	the classic swap function helper
*/
void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
	function to hold the arithmetic operations for
	the bubble-down method of the heap_extract function.
	It finds up to two children of a parent,
	picks the smaller, and if the current element
	is bigger than that smaller child, swap down.
	Stop when the element is ≤ both children or has no children.
*/
void	bubble_down(t_heap *heap, int i)
{
	t_heap_node	*n;
	int			smallest;
	int			left;
	int			right;

	n = heap->nodes;
	while (1)
	{
		smallest = i;
		left = 2 * i + 1;
		right = 2 * i + 2;
		if (left < heap->size && n[left].priority < n[smallest].priority)
			smallest = left;
		if (right < heap->size && n[right].priority < n[smallest].priority)
			smallest = right;
		if (smallest == i)
			break ;
		swap_nodes(&heap->nodes[i], &heap->nodes[smallest]);
		i = smallest;
	}
}

/*
	moves an element up the heap until it sits in the right place.
	It compares the element with its parent, and if it is smaller,
	swaps them, then repeats from the new position. It stops when the
	element is no longer smaller than its parent, or reaches the top.
	Used by heap_insert (new elements start at the bottom) and by
	heap_remove_id (the replacement element may need to rise).
*/
void	bubble_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->nodes[i].priority >= heap->nodes[parent].priority)
			break ;
		swap_nodes(&heap->nodes[i], &heap->nodes[parent]);
		i = parent;
	}
}

/*
	removes a specific coder from the heap by its id.
*/
int	heap_remove_id(t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	while (i < heap->size && heap->nodes[i].coder_id != coder_id)
		i++;
	if (i == heap->size)
		return (-1);
	heap->size--;
	heap->nodes[i] = heap->nodes[heap->size];
	bubble_up(heap, i);
	bubble_down(heap, i);
	return (0);
}
