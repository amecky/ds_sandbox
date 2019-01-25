#pragma once

const int MAX_BLOCKS = 16;

struct BlockArray {

	char* data;
	int size;
	int capacity;
	int total_capacity;
	int _sizes[MAX_BLOCKS];
	int _indices[MAX_BLOCKS];
	int _num_blocks;

	BlockArray();

	~BlockArray();

	void init(int* sizes, int num);

	void* get_ptr(int index);

	bool resize(int new_size);

	void remove(int index);
	
};


