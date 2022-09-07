#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"
#include "clog.h"
#include "miniunit.h"
//..
Node* pq_enqueue(Node** a_head, void* a_value, int (*cmp_fn)(const void*, const void*)) {
	Node* new_node = malloc(sizeof(*new_node));
	*new_node = (Node){ .a_value = a_value, .next = NULL };
	if(*a_head == NULL) {
		*a_head = new_node;
	}
	else if(cmp_fn((*a_head)->a_value, new_node->a_value) >= 0) {
		new_node->next = *a_head;
		*a_head = new_node;
	}
	else {
		Node* initial = *a_head;
		while(initial->next != NULL && cmp_fn(initial->next->a_value, new_node->a_value) < 0) {
			initial = initial->next;
		}
		new_node->next = initial->next;
		initial->next = new_node;
	}
	return new_node;
}

Node* pq_dequeue(Node** a_head) {
	Node* removed_node = *a_head;
	if(*a_head != NULL) {
		*a_head = (*a_head)->next;
		removed_node->next = NULL;
	}
	return removed_node;
}

Node* stack_push(Node** a_head, void* a_value) {
	Node* pushed_node = malloc(sizeof(*pushed_node));
	*pushed_node = (Node){ .a_value = a_value, .next = *a_head };
	*a_head = pushed_node;
	return pushed_node;
}

Node* stack_pop(Node** a_head) {
	return pq_dequeue(a_head);
}

void destroy_list(Node** a_head, void (*destroy_fn)(void*)) {
	while(*a_head != NULL) {
		// destroy_fn((*a_head)->a_value); //necessary? Not now, don't call it.
		Node* new_head = *a_head;
		*a_head = (*a_head)->next;
		free(new_head);
	}
}

#define HUFFMAN_C_V1
