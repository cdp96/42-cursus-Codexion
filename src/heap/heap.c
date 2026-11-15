#include "codexion.h"

/*
	heap queue initialization,
	capacity is the max cap and corresponds to n_coders,
	size starts at 0, and is how many are currently in it
	(subject to changes with insert/extract).
*/
int	heap_init(t_heap *heap, int capacity)
{
	heap->nodes = malloc(sizeof(t_heap_node) * capacity);
	if (!heap->nodes)
		return (-1);
	heap->size = 0;
	heap->capacity = capacity;
	return (0);
}

/*
	helper function for freeing the heap.
*/
void	heap_free(t_heap *heap)
{
	free(heap->nodes);
}

/*
	function to insert a new element in the heap queue.
	It first inserts it at the end of the line, then
	compares it to its parent, if the new one is smaller
	swaps them. Repeat with the new position until either
	it’s no longer smaller than its parent,
	or it reaches index 0 (bubble-up sort)..
*/
int	heap_insert(t_heap *heap, t_heap_node node)
{
	if (heap->size >= heap->capacity)
		return (-1);
	heap->nodes[heap->size] = node;
	heap->size++;
	bubble_up(heap, heap->size - 1);
	return (0);
}

/*
	function to extract the top element of the heap queue.
	It takes the index 0 element, shrink the size of the queue,
	then moves the last element of the queue to the top to
	then bubble it down until the heap is ordered again.
*/
int	heap_extract(t_heap *heap, t_heap_node *first)
{
	if (heap->size == 0)
		return (-1);
	*first = heap->nodes[0];
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	bubble_down(heap, 0);
	return (0);
}

/*
	helper function to return the coder id
	at the top of the heap queue.
*/
int	top_heap(t_heap *heap)
{
	if (heap->size == 0)
		return (-1);
	return (heap->nodes[0].coder_id);
}
