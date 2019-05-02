#include <iostream>
#include "skiplist.hpp"
using namespace std;
typedef int keyType;
typedef string valType;

typedef struct _node{
  keyType key;
  valType val;
} node;


typedef struct _lsm{
  SkipList <int, string> sl;
  size_t block_size;
  size_t block2_size;
  size_t block3_size;
  size_t block4_size; 
  int k; 
  int node_size;
  size_t next_empty;
  node *block;
  char* disk1;
  char* disk2;
  char* disk3;
  char* disk4;
  bool sorted;
} lsm;

typedef struct _nodei{
  node *node;
  int index;
} nodei;

void check_file_ret(FILE* f, int r);

lsm* init_new_lsm(size_t block_size, bool sorted);

void destruct_lsm(lsm* tree); 

void merge(node *whole, node *left, int left_size, node *rght, int right_size);

void merge_sort(node *block, int n);

nodei* search_buffer(const keyType* key, lsm* tree);

nodei* search_disk(const keyType* key, lsm* tree);

node* get(const keyType key, lsm* tree);

int write_to_disk(lsm* tree);

int put(const keyType* key, const valType* val, lsm* tree);

int lsmdelete(const keyType* key, lsm* tree);

int update(const keyType* key, const valType* val, lsm* tree);

void print_buffer_data(lsm* tree);

void print_disk_data(lsm* tree); 
