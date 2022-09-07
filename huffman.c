#include <stdbool.h>
#include <stdio.h>
#include "huffman.h"

//static int _cmp_fn(const void* arg_1, const void* arg_2);
//static void _lookup(TreeNode* a_root, _BitCode* character_to_code);

static int _cmp_fn(const void* arg_1, const void* arg_2) {
	if(((TreeNode*)arg_1) -> frequency == ((TreeNode*)arg_2) -> frequency) {
		if((((TreeNode*)arg_2) -> character) == '\0' || (((TreeNode*)arg_1) -> character) == '\0'){
			return ((((TreeNode*)arg_2) -> character) - (((TreeNode*)arg_1) -> character));
		}
		else {
			return ((((TreeNode*)arg_1) -> character) - (((TreeNode*)arg_2) -> character));
		}
	}
	return ((((TreeNode*)arg_1) -> frequency) - (((TreeNode*)arg_2) -> frequency));
}

Node* make_huffman_pq(Frequencies freqs) {
	Node* a_head = NULL;
	for(int i = 0; i < 256; i++) {
		if(freqs[i] != 0) {
			TreeNode* tree_node = malloc(sizeof(*tree_node));
			*tree_node = (TreeNode){ .frequency = freqs[i], .character = i, .left = NULL, .right = NULL };
			pq_enqueue(&a_head, tree_node, _cmp_fn);
		}
	}
	return a_head;
}

TreeNode* make_huffman_tree(Node* a_head) {
	if(a_head == NULL) {
		return NULL;
	}
	int frq = 0;
	while(a_head -> next != NULL) {
		Node* deq_node1 = pq_dequeue(&a_head);
		Node* deq_node2 = pq_dequeue(&a_head);
		frq = (((TreeNode*)(deq_node1 -> a_value)) -> frequency) + (((TreeNode*)(deq_node2 -> a_value)) -> frequency);
		TreeNode* new_tree = malloc(sizeof(*new_tree));
		*new_tree = (TreeNode) { .left = deq_node1 -> a_value, .right = deq_node2 -> a_value, .frequency = frq, .character = '\0'};
		pq_enqueue(&a_head, new_tree, _cmp_fn);
		free(deq_node1);
		free(deq_node2);
	}
	Node* deq_node = pq_dequeue(&a_head);
	TreeNode* hffmn_head = deq_node -> a_value;
	free(deq_node);
	return hffmn_head;
}

void write_coding_table(TreeNode* a_root, BitWriter* a_writer) {
	uint8_t one_bit = 0xff;
	uint8_t zero_bit = 0x00;

	if(a_root -> left != NULL && a_root -> right != NULL) {
		write_coding_table(a_root -> left, a_writer);
		write_coding_table(a_root -> right, a_writer);
	}

	uint8_t root_char = a_root -> character;
	uint8_t char_bits = sizeof(&(a_root -> character));

	if(root_char != '\0') {
		write_bits(a_writer, one_bit, 1);
		write_bits(a_writer, root_char, char_bits);
	}
	else {
		write_bits(a_writer, zero_bit, 1);
	}
}

static void _lookup(TreeNode* a_root, _BitCode* character_to_code, uint8_t bits, int num_bits) {
	if(a_root != NULL) {
		_lookup(a_root -> left, character_to_code, bits << 1, num_bits + 1);
		_lookup(a_root -> right, character_to_code, (bits << 1) | 0x01, num_bits + 1);
		if(a_root -> character != '\0') {
			character_to_code[a_root -> character] = (_BitCode) { .bits = bits, .num_bits = num_bits };
		}
	}
}

void write_compressed(TreeNode* a_root, BitWriter* a_writer, uint8_t* uncompressed_bytes) {
	_BitCode character_to_code[256] = { (_BitCode) { .bits = 0x00, .num_bits = 0 } };
	_lookup(a_root, character_to_code, 0x00, 0);
	for(int i = 0; *(uncompressed_bytes + i) != '\0'; i++) {
		uint8_t index = *(uncompressed_bytes + i);
		write_bits(a_writer, (character_to_code[index]).bits, (character_to_code[index]).num_bits);
	}
}

void destroy_huffman_tree(TreeNode** a_root) {
	if(*a_root != NULL) {
		destroy_huffman_tree(&((*a_root)->left));
		destroy_huffman_tree(&((*a_root)->right));
		free(*a_root);
	}
	*a_root = NULL;
}
