#include <stdbool.h>
#include <stdio.h>
#include "huffman.h"

static int _test_empty();

static int _test_char();

static int _test_big_small_char();

static int _test_stable();

static int _test_frequency_char();

static int _test_newline_space_char();

static int _test_sentence(); 

static int _test_table();

static bool _check_file_contents(const char* path, size_t num_bytes, uint8_t const* expected, uint8_t* result);

static uint8_t* _read_file(char const* path);

static char const* _write_file(char const* content, char const* path);

int main(int argc, char* argv[]) {

	mu_run(_test_empty);
	mu_run(_test_char);
	mu_run(_test_big_small_char);
	mu_run(_test_stable);
	mu_run(_test_frequency_char);
	mu_run(_test_newline_space_char);
	mu_run(_test_sentence); 
	mu_run(_test_table);

	return EXIT_SUCCESS;
}

static uint8_t* _read_file(char const* path) {
	FILE* fp = fopen(path, "r");
	int num_bytes = 0;
	while(!feof(fp)) {
		num_bytes++;
		fgetc(fp);
	}
	if(fp != NULL) {
		fclose(fp);
	}
	fp = fopen(path, "r");
	uint8_t* string = malloc(num_bytes * sizeof(*string));
	int idx = 0;
	for(uint8_t ch = fgetc(fp); !feof(fp); ch = fgetc(fp)) {
		string[idx++] = ch;
	}
	string[idx] = '\0';
	if(fp != NULL) {
		fclose(fp);
	}

	return string;
}

static char const* _write_file(char const* content, char const* path) {
	const char* mode = "w"; 
	FILE* file_open = fopen(path, mode);
	fputs(content, file_open);
	if(file_open != NULL) {
		fclose(file_open);
	}
	return path;
}

static bool _check_file_contents(const char* path, size_t num_bytes, uint8_t const* expected, uint8_t* result) {
	FILE* file_read = fopen(path, "r");
	size_t read_bytes = fread(result, sizeof(*result), num_bytes, file_read);
	fclose(file_read);

	if(read_bytes != num_bytes) {
		return false;
	}
	for(int bit_idx = 0; bit_idx < num_bytes; bit_idx++) {
		if(result[bit_idx] != expected[bit_idx]) {
			return false;
		}
	}
	return true;
}

/*
static void _destroy_fn(void* a_value) {
	free(a_value);
}
*/

static int _test_empty() {
	mu_start();

	char const* content = "";
	char const* path = "test.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	bool freq = calc_frequencies(freqs, file_name, &a_error);

	Node* head = make_huffman_pq(freqs);
	TreeNode* root = make_huffman_tree(head);

	if(!freq) {
		mu_check(root == NULL);
	}

	mu_end();
}

static int _test_char() {
	mu_start();
/*
	mu_check(root -> character == '\0');
	mu_check(root -> frequency == 3);

	mu_check(root -> right -> character == '\0');
	mu_check(root -> right -> frequency == 2);

	mu_check(root -> left -> character == 'c');            // INPUT TREE
	mu_check(root -> left -> frequency == 1);
	
	mu_check(root -> right -> right -> character == 'b');
	mu_check(root -> right -> right -> frequency == 1);
	
	mu_check(root -> right -> left -> character == 'a');
	mu_check(root -> right -> left -> frequency == 1);
*/

	char const* content = "abc";
	char const* path = "test1.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	calc_frequencies(freqs, file_name, &a_error);

	Node* head = make_huffman_pq(freqs);

	TreeNode* root = make_huffman_tree(head);

	uint8_t* file_contents = _read_file(file_name);
	char const* path_result = "result1.txt";
	BitWriter writer = open_bit_writer(path_result);
	write_compressed(root, &writer, file_contents);

	free(file_contents);
	destroy_huffman_tree(&root);
	close_bit_writer(&writer);

	size_t size = 1;
	uint8_t expected[1] = { 0xB0 };
	uint8_t result[1] = { 0x00 };
	bool file_check = _check_file_contents(path_result, size, expected, result);

	mu_check(file_check);

	mu_end();
}

static int _test_big_small_char() {
	mu_start();
/*
	mu_check(root -> character == '\0');
	mu_check(root -> frequency == 3);

	mu_check(root -> left -> character == 'c');
	mu_check(root -> left -> frequency == 1);

	mu_check(root -> right -> character == '\0');               // INPUT TREE
	mu_check(root -> right -> frequency == 2);
	
	mu_check(root -> right -> right -> character == 'a');
	mu_check(root -> right -> right -> frequency == 1);

	mu_check(root -> right -> left -> character == 'B');
	mu_check(root -> right -> left -> frequency == 1);

	destroy_huffman_tree(&root);
*/

	char const* content = "aBc";
	char const* path = "test2.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	calc_frequencies(freqs, file_name, &a_error);

	Node* head = make_huffman_pq(freqs);

	TreeNode* root = make_huffman_tree(head);

	uint8_t* file_contents = _read_file(file_name);
	char const* path_result = "result2.txt";
	BitWriter writer = open_bit_writer(path_result);
	if(file_contents != NULL) {
		write_compressed(root, &writer, file_contents);
	}

	if(file_contents != NULL) {
		free(file_contents);
	}
	destroy_huffman_tree(&root);
	close_bit_writer(&writer);

	size_t size = 1;
	uint8_t expected[1] = { 0xE0 };
	uint8_t result[1] = { 0x00 };
	bool file_check = _check_file_contents(path_result, size, expected, result);

	mu_check(file_check);

	mu_end();
}

static int _test_stable() {
	mu_start();

/*
	mu_check(root -> character == '\0');
	mu_check(root -> frequency == 5);

	mu_check(root -> right -> character == 'a');
	mu_check(root -> right -> frequency == 3);

	mu_check(root -> left -> character == '\0');          // INPUT TREE
	mu_check(root -> left -> frequency == 2);
	
	mu_check(root -> left -> left -> character == 'b');
	mu_check(root -> left -> left -> frequency == 1);
	
	mu_check(root -> left -> right -> character == 'c');
	mu_check(root -> left -> right -> frequency == 1);
*/

	char const* content = "bcaaa";
	char const* path = "test3.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	calc_frequencies(freqs, file_name, &a_error);

	Node* head = make_huffman_pq(freqs);

	TreeNode* root = make_huffman_tree(head);

	uint8_t* file_contents = _read_file(file_name);
	char const* path_result = "result3.txt";
	BitWriter writer = open_bit_writer(path_result);
	if(file_contents != NULL) {
		write_compressed(root, &writer, file_contents);
	}

	if(file_contents != NULL) {
		free(file_contents);
	}
	destroy_huffman_tree(&root);
	close_bit_writer(&writer);

	size_t size = 1;
	uint8_t expected[1] = { 0x1E };
	uint8_t result[1] = { 0x00 };
	bool file_check = _check_file_contents(path_result, size, expected, result);

	mu_check(file_check);

	mu_end();
}

static int _test_frequency_char() {
	mu_start();

/*
	mu_check(root -> character == '\0');
	mu_check(root -> frequency == 10);

	mu_check(root -> left -> character == 'd');
	mu_check(root -> left -> frequency == 4);

	mu_check(root -> right -> character == '\0');              // INPUT TREE
	mu_check(root -> right -> frequency == 6);
	
	mu_check(root -> right -> right -> character == '\0');
	mu_check(root -> right -> right -> frequency == 3);

	mu_check(root -> right -> left -> character == 'c');
	mu_check(root -> right -> left -> frequency == 3);
	
	mu_check(root -> right -> right -> right -> character == 'a');
	mu_check(root -> right -> right -> right -> frequency == 2);
	
	mu_check(root -> right -> right -> left -> character == 'b');
	mu_check(root -> right -> right -> left -> frequency == 1);
*/

	char const* content = "ddddaabccc";
	char const* path = "test4.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	calc_frequencies(freqs, file_name, &a_error);

	Node* head = make_huffman_pq(freqs);

	TreeNode* root = make_huffman_tree(head);
	
	uint8_t* file_contents = _read_file(file_name);
	char const* path_result = "result4.txt";
	BitWriter writer = open_bit_writer(path_result);
	if(file_contents != NULL) {
		write_compressed(root, &writer, file_contents);
	}

	if(file_contents != NULL) {
		free(file_contents);
	}
	destroy_huffman_tree(&root);
	close_bit_writer(&writer);

	size_t size = 3;
	uint8_t expected[3] = { 0x0F, 0xF5, 0x40 };
	uint8_t result[3] = { 0x00 };
	bool file_check = _check_file_contents(path_result, size, expected, result);

	mu_check(file_check);

	mu_end();
}

static int _test_newline_space_char() {
	mu_start();
/*
	mu_check(root -> character == '\0');
	mu_check(root -> frequency == 7);

	mu_check(root -> left -> character == '\0');
	mu_check(root -> left -> frequency == 3);

	mu_check(root -> right -> character == '\0');
	mu_check(root -> right -> frequency == 4);
	
	mu_check(root -> left -> right -> character == 'k');
	mu_check(root -> left -> right -> frequency == 2);

	mu_check(root -> left -> left -> character == '3');
	mu_check(root -> left -> left -> frequency == 1);
	
	mu_check(root -> right -> right -> left -> character == '\n');    // INPUT TREE
	mu_check(root -> right -> right -> left -> frequency == 1);

	mu_check(root -> right -> right -> right -> character == ' ');
	mu_check(root -> right -> right -> right -> frequency == 1);

	mu_check(root -> right -> right -> character == '\0');
	mu_check(root -> right -> right -> frequency == 2);

	mu_check(root -> right -> left -> character == '\0');
	mu_check(root -> right -> left -> frequency == 2);
	
	mu_check(root -> right -> left -> right -> character == '2');
	mu_check(root -> right -> left -> right -> frequency == 1);

	mu_check(root -> right -> left -> left -> character == '$');
	mu_check(root -> right -> left -> left -> frequency == 1);
*/
	char const* content = "$ \nkk32";
	char const* path = "test5.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	calc_frequencies(freqs, file_name, &a_error);

	Node* head = make_huffman_pq(freqs);

	TreeNode* root = make_huffman_tree(head);
	
	uint8_t* file_contents = _read_file(file_name);
	char const* path_result = "result5.txt";
	BitWriter writer = open_bit_writer(path_result);
	if(file_contents != NULL) {
		write_compressed(root, &writer, file_contents);
	}

	if(file_contents != NULL) {
		free(file_contents);
	}
	destroy_huffman_tree(&root);
	close_bit_writer(&writer);
	
	size_t size = 3;
	uint8_t expected[3] = { 0x9F, 0X29, 0X40 };
	uint8_t result[3] = { 0x00 };
	bool file_check = _check_file_contents(path_result, size, expected, result);

	mu_check(file_check);

	mu_end();
}

static int _test_sentence() {
	mu_start();
/*
	mu_check(root -> character == '\0');
	mu_check(root -> frequency == 24);

	mu_check(root -> right -> character == '\0');
	mu_check(root -> right -> frequency == 14);

	mu_check(root -> left -> character == '\0');
	mu_check(root -> left -> frequency == 10);
	
	mu_check(root -> left -> left -> character == 'f');
	mu_check(root -> left -> left -> frequency == 5);
	
	mu_check(root -> left -> right -> character == '\0');
	mu_check(root -> left -> right -> frequency == 5);
	
	mu_check(root -> left -> right -> right -> character == ' ');
	mu_check(root -> left -> right -> right -> frequency == 3);
	
	mu_check(root -> left -> right -> left -> character == '\0');
	mu_check(root -> left -> right -> left -> frequency == 2);
	
	mu_check(root -> left -> right -> left -> right -> character == 'l');
	mu_check(root -> left -> right -> left -> right -> frequency == 1);
	
	mu_check(root -> left -> right -> left -> left -> character == 'h');        // INPUT TREE
	mu_check(root -> left -> right -> left -> left -> frequency == 1);

	mu_check(root -> right -> right -> character == '\0');
	mu_check(root -> right -> right -> frequency == 8);
	
	mu_check(root -> right -> left -> character == '\0');
	mu_check(root -> right -> left -> frequency == 6);

	mu_check(root -> right -> right -> right -> character == '\0');
	mu_check(root -> right -> right -> right -> frequency == 4);
	
	mu_check(root -> right -> right -> right -> right -> character == 's');
	mu_check(root -> right -> right -> right -> right -> frequency == 2);
	
	mu_check(root -> right -> right -> right -> left -> character == 'n');
	mu_check(root -> right -> right -> right -> left -> frequency == 2);
	
	mu_check(root -> right -> left -> left -> character == 'u');
	mu_check(root -> right -> left -> left -> frequency == 3);

	mu_check(root -> right -> left -> right -> character == '\0');
	mu_check(root -> right -> left -> right -> frequency == 3);

	mu_check(root -> right -> left -> right -> left -> character == 'y');
	mu_check(root -> right -> left -> right -> left -> frequency == 1);
	
	mu_check(root -> right -> left -> right -> right -> character == 'a');
	mu_check(root -> right -> left -> right -> right -> frequency == 2);

	destroy_huffman_tree(&root);
*/	
	char const* content = "huffman fluffs many mums";
	char const* path = "test6.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	calc_frequencies(freqs, path, &a_error);

	Node* head = make_huffman_pq(freqs);

	TreeNode* root = make_huffman_tree(head);

	uint8_t* file_contents = _read_file(file_name);
	char const* path_result = "result6.txt";
	BitWriter writer = open_bit_writer(path_result);
	if(file_contents != NULL) {
		write_compressed(root, &writer, file_contents);
	}

	if(file_contents != NULL) {
		free(file_contents);
	}
	destroy_huffman_tree(&root);
	close_bit_writer(&writer);

	size_t size = 10;
	uint8_t expected[10] = { 0x48, 0x1A, 0xF9, 0x8B, 0x03, 0xDE, 0xBE, 0xA7, 0xA6, 0xF0 };
	uint8_t result[10] = { 0x00 };
	bool file_check = _check_file_contents(path_result, size, expected, result);

	mu_check(file_check);

	mu_end();
}
// for %100 code coverage.
static int _test_table() {
	mu_start();

	char const* content = "abc";
	char const* path = "test_table.txt";
	char const* file_name = _write_file(content, path);

	char const* a_error = NULL;
	Frequencies freqs = { 0 };
	calc_frequencies(freqs, file_name, &a_error);

	Node* head = make_huffman_pq(freqs);

	TreeNode* root = make_huffman_tree(head);

	BitWriter writer = open_bit_writer(file_name);
	write_coding_table(root, &writer);

	destroy_huffman_tree(&root);
	close_bit_writer(&writer);

	size_t size = 4;
	uint8_t expected[4] = { 0xB1, 0xD8, 0x6C, 0x40 };
	uint8_t result[4] = { 0x00 };
	bool file_check = _check_file_contents(file_name, size, expected, result);

	mu_check(file_check);

	mu_end();
}
