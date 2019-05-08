#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "lsm.hpp"
#include <iostream>
using namespace std;

void check_file_ret(FILE* f, int r){

  if(r == 0){ 
    if(ferror(f)){
      perror("ferror\n");
    }
  }
}

lsm* init_new_lsm(size_t buffer_size, bool sorted){
  /*Initializes
    buffer_size (size_t): size of in-memory buffer.
    sorted :true :need sort.
  */ 
  lsm* tree=new lsm;
  //tree = malloc(sizeof(lsm));
  if(!tree){
    perror("init_lsm: block is null \n");
    return NULL;
  }

  tree->block_size = buffer_size;
  tree->k = 2; 
  tree->next_empty = 0; 
  tree->node_size = sizeof(node);
  tree->block = malloc(tree->block_size*(tree->node_size)); 
  if(!tree->block){
    perror("init_lsm: block is null \n");
    return NULL;
  }
  tree->disk1 = "disk_storage1.txt";
  tree->disk2 = "disk_storage2.txt";
  tree->disk3 = "disk_storage3.txt";
  tree->disk4 = "disk_storage4.txt";
  tree->sorted = sorted;
  return tree;
}


void destruct_lsm(lsm* tree){
  /* Destruct LSM object.
     Args: 
     tree (lsm*): pointer to LSM object to destroy.
  */  
  free(tree->block);
  free(tree);
}

void merge(node *whole, node *left,int left_size,node *right,int right_size){
  /* whole (node*): pointer to the node representing the merged array of nodes. 
     left (node*): pointer to the left split of nodes to be merged. 
     left_size (int): int representing the the size of the left split. 
     right (node*): pointer to the right split of nodes to be merged. 
     rightt_size (int): int representing the the size of the right split. 
  */    
  int l, r, i;
  l = 0; r = 0; i = 0;
  //cout<<left_size<<" lr "<<right_size<<endl;
  while(l < left_size && r < right_size) {

	//cout<<i<<l<<r<<endl;
    if(left[l].key  < right[r].key){

      whole[i++] = left[l++];
    } else{
      whole[i++] = right[r++];
    }
  }

  while(l < left_size){
    whole[i++] = left[l++];
  }

  while(r < right_size){
    whole[i++] = right[r++];
	}

}


void merge_sort(node *block, int n){
  /* sorts by key.

 */
  assert(block != NULL);
  if(n < 2){
    return;
  }
  int mid, i;
  mid = n/2;
  node *left;
  node *right;

  /* create and populate left and right subarrays */
  left = (node*)malloc(mid*sizeof(node));
  right = (node*)malloc((n-mid)*sizeof(node));

  memcpy(left, block, sizeof(node) * mid);
  memcpy(right, &block[mid], sizeof(node) * (n - mid));

  /* sort and merge the two arrays */
  merge_sort(left,mid);  // sort left subarray
  merge_sort(right,n-mid);  // sort right subarray
  merge(block,left,mid,right,n-mid);
  free(left);
  free(right);
}


nodei* search_buffer(const keyType* key, lsm* tree){
  valType value;
  bool find=tree->sl.Find(key,value);
  if(find)
  {
	  nodei* nodei = malloc(sizeof(nodei));
      nodei->node = malloc(sizeof(node));
      nodei->node->key = key;
      nodei->node->val = value;
      nodei->index = key;
      return nodei;
  }
  for (int i = 0; i < tree->next_empty; i++){
    if (tree->block[i].key == *key){
      nodei* nodei = malloc(sizeof(nodei));
      nodei->node = malloc(sizeof(node));
      nodei->node->key = tree->block[i].key;
      nodei->node->val = tree->block[i].val;
      nodei->index = i;
      return nodei;
    }
  }  
  return NULL;
}

nodei* search_disk(const keyType* key, lsm* tree){

  int r; 
  FILE* f = fopen("disk_storage1.txt", "r");
  FILE* f2 = fopen("disk_storage2.txt", "r");
  FILE* f3 = fopen("disk_storage3.txt", "r");
  FILE* f4 = fopen("disk_storage4.txt", "r");
  if(f == NULL){
    perror("search_disk: open 1: \n");
    return NULL;
  }
  node *file_data;
  size_t num_elements;
  r = fread(&num_elements, sizeof(size_t), 1, f);
  check_file_ret(f, r);
  file_data = malloc(sizeof(node)*num_elements);
  r = fread(file_data, sizeof(node), num_elements, f);
  check_file_ret(f, r);
  check_file_ret(f2, r);
  check_file_ret(f3, r);
  check_file_ret(f4, r);
  for(int i = 0; i < num_elements; i++){
    if (file_data[i].key == *key){
      nodei* nodei = malloc(sizeof(nodei));
      nodei->node = malloc(sizeof(node));
      nodei->node->key = file_data[i].key;
      nodei->node->val = file_data[i].val;
      nodei->index = i;
      if(fclose(f)){
	perror("search_disk: fclose: ");
      }
      return nodei;
    }
  }
  free(file_data);
  if(fclose(f)){
    perror("search_disk: fclose: ");
  }
  return NULL; 
}

node* get(const keyType key, lsm* tree){

  // search the buffer for this item
  nodei* ni = search_buffer(&key, tree);
  if(ni != NULL){
    return ni->node;
  } else{
    // search through the file on disk for this item
    ni = search_disk(&key, tree);
    if(ni != NULL){
      return ni->node;
    }
  }
  // If it does not find the given key, it will return NULL
  return NULL;
}

int write_to_disk(lsm* tree){
  /* Write all data to disk. 
   */
  size_t disk_size1=1000;
  size_t disk_size2=4000;
  size_t disk_size3=16000;
  node *complete_data = new node;
  node *file_data = new node;
  node *file_data2 = new node;
  node *file_data3 = new node;
  node *file_data4 = new node;
  size_t num_elements = 0;
  size_t num_elements2 = 0;
  size_t num_elements3 = 0;
  size_t num_elements4 = 0;
  int r;
	
  struct stat s; 
  int file_exists1 = stat(tree->disk1, &s); 
  int file_exists2 = stat(tree->disk2, &s); 
  int file_exists3 = stat(tree->disk3, &s); 
  int file_exists4 = stat(tree->disk4, &s); 
  if(file_exists1 == 0){

    // the file already exists 
    FILE* fr  = fopen(tree->disk1, "r");
    // read number of elements 
    r = fread(&num_elements, sizeof(size_t), 1, fr);
    check_file_ret(fr, r);
	//cout<<"ne: "<<num_elements;	
    file_data = malloc(sizeof(node)*num_elements);
    //assert(file_data);
    r = fread(file_data, sizeof(node), num_elements, fr);
    check_file_ret(fr, r);
    if(fclose(fr)){
      perror("put: close 2: \n");
    }
	
    // merge buffer and  disk
    complete_data = malloc(sizeof(node)*(num_elements+tree->next_empty));
	//cout<<sizeof(complete_data);
	//cout<<num_elements<<"  "<<tree->next_empty<<endl;
	for (int it=0;it<100;it++)
		{//cout<<"it: "<<it<<" "<<file_data[it].key<< " "<<file_data[it].key<<endl;	
		}
    merge(complete_data, file_data, num_elements, tree->block,tree->next_empty);
	//cout<<"merge done"<<endl;
    num_elements += tree->block_size;
    //free(file_data);
  }

  FILE* fw  = fopen(tree->disk1, "w");
  if(complete_data == NULL){
    complete_data = tree->block;
  }
  if(num_elements <= 0){
    num_elements = tree->next_empty;
  }
  // seek to the start of the file & write # of elements
  if(fseek(fw, 0, SEEK_SET)){
    perror("put: fseek 4: \n");
  }
  if(!fwrite(&num_elements, sizeof(size_t), 1, fw)){
    perror("put: fwrite 4: \n");
  }
  // seek to the first space after the number of elements
  if(fseek(fw, sizeof(size_t), SEEK_SET)){
    perror("put: fseek 5: \n");
  }
  if(!fwrite(complete_data,  sizeof(node), num_elements, fw)){
    perror("put: fwrite 5: \n");
    }
  // reset next_empty to 0
  //cout<<"here"<<endl;
  tree->sl.~SkipList();
  tree->next_empty = 0;
  if(fclose(fw)){
    perror("put: close 2: \n");
  }


  if  (num_elements>=disk_size1)
  {
    FILE* fr  = fopen(tree->disk2, "r");
	r = fread(&num_elements2, sizeof(size_t), 1, fr);	
    file_data2 = malloc(sizeof(node)*num_elements2);

    r = fread(file_data, sizeof(node), num_elements2, fr);
    check_file_ret(fr, r);
    if(fclose(fr)){
      perror("put: close 2: \n");
    }
    complete_data = malloc(sizeof(node)*(num_elements2+num_elements));
    merge(complete_data, file_data, num_elements, file_data2,num_elements2);
    num_elements2 += num_elements;
    //free(file_data);
  FILE* fw2  = fopen(tree->disk2, "w");
  if(complete_data == NULL){
    complete_data = num_elements;
  }
  if(num_elements <= 0){
    num_elements = tree->next_empty;
  }
  // seek to the start of the file & write # of elements
  if(fseek(fw2, 0, SEEK_SET)){
    perror("put: fseek 4: \n");
  }
  if(!fwrite(&num_elements2, sizeof(size_t), 1, fw2)){
    perror("put: fwrite 4: \n");
  }
  // seek to the first space after the number of elements
  if(fseek(fw2, sizeof(size_t), SEEK_SET)){
    perror("put: fseek 5: \n");
  }
  if(!fwrite(complete_data,  sizeof(node), num_elements2, fw2)){
    perror("put: fwrite 5: \n");
    }
   num_elements2 = 0;
  }


  if  (num_elements>=disk_size2)
  {
    FILE* fr  = fopen(tree->disk3, "r");
	r = fread(&num_elements3, sizeof(size_t), 1, fr);	
    file_data3 = malloc(sizeof(node)*num_elements3);

    r = fread(file_data2, sizeof(node), num_elements3, fr);
    check_file_ret(fr, r);
    if(fclose(fr)){
      perror("put: close 2: \n");
    }
    complete_data = malloc(sizeof(node)*(num_elements2+num_elements3));
    merge(complete_data, file_data3, num_elements3, file_data2,num_elements2);
    num_elements3 += num_elements2;
    //free(file_data);
  FILE* fw3  = fopen(tree->disk3, "w");
  if(complete_data == NULL){
    complete_data = tree->block;
  }
  if(num_elements <= 0){
    num_elements = tree->next_empty;
  }
  // seek to the start of the file & write # of elements
  if(fseek(fw3, 0, SEEK_SET)){
    perror("put: fseek 4: \n");
  }
  if(!fwrite(&num_elements3, sizeof(size_t), 1, fw3)){
    perror("put: fwrite 4: \n");
  }
  // seek to the first space after the number of elements
  if(fseek(fw3, sizeof(size_t), SEEK_SET)){
    perror("put: fseek 5: \n");
  }
  if(!fwrite(complete_data,  sizeof(node), num_elements3, fw3)){
    perror("put: fwrite 5: \n");
    }
   num_elements3 = 0;
  } 


  if  (num_elements>=disk_size3)
  {
    FILE* fr  = fopen(tree->disk4, "r");
	r = fread(&num_elements4, sizeof(size_t), 1, fr);	
    file_data4 = malloc(sizeof(node)*num_elements4);

    r = fread(file_data, sizeof(node), num_elements4, fr);
    check_file_ret(fr, r);
    if(fclose(fr)){
      perror("put: close 2: \n");
    }
    complete_data = malloc(sizeof(node)*(num_elements2+num_elements));
    merge(complete_data, file_data4, num_elements4, file_data3,num_elements3);
    num_elements4 += num_elements3;
    //free(file_data);
  FILE* fw4  = fopen(tree->disk4, "w");
  if(complete_data == NULL){
    complete_data = tree->block;
  }
  if(num_elements <= 0){
    num_elements = tree->next_empty;
  }
  // seek to the start of the file & write # of elements
  if(fseek(fw, 0, SEEK_SET)){
    perror("put: fseek 4: \n");
  }
  if(!fwrite(&num_elements4, sizeof(size_t), 1, fw4)){
    perror("put: fwrite 4: \n");
  }
  // seek to the first space after the number of elements
  if(fseek(fw4, sizeof(size_t), SEEK_SET)){
    perror("put: fseek 5: \n");
  }
  if(!fwrite(complete_data,  sizeof(node), num_elements4, fw4)){
    perror("put: fwrite 5: \n");
    }
   num_elements4 = 0;
  }


  return 0; 
}


int put(const keyType* key, const valType* val, lsm* tree){
  //cout<<"nextempty: "<<tree->next_empty<<endl;
	//cout<<*key<<*val;
  int r = 0; 
  tree->sl.Insert(*key,*val);
  //tree->sl.Print();
  if(tree->next_empty == tree->block_size){
    // buffer is full and must be written to disk
    r = write_to_disk(tree); 
	
  }
  node n;
  n.key = *key;
  n.val = *val;
  tree->block[tree->next_empty] = n;
  tree->next_empty += 1;
  return r;
}


int lsmdelete(const keyType* key, lsm* tree){
  /* Deletes the node containing the given key.  */ 
  int r = 0;
  nodei *ni = malloc(sizeof(nodei));
  // if the node is in the buffer
  ni = search_buffer(key, tree);
  if(ni != NULL){
    tree->next_empty -= 1;
	tree->sl.Erase(key);
    memmove(&tree->block[ni->index], &tree->block[ni->index+1], tree->block_size-ni->index);
  } 
}


int update(const keyType* key, const valType* val, lsm* tree){
  int r; 
  nodei* ni = search_buffer(key, tree);
  //printf("index is: ni %d \n", ni->index);
  //printf("upating key: %d, val: %d \n", (ni->node->key, ni->node->val));
  if(ni != NULL){
    node n;
    n.key = *key;
    n.val = *val;
    tree->block[ni->index] = n;
    //printf("to key: %d  val: %d \n", (*key, *val));
  } else {
    r = lsmdelete(key, tree);
    r = put(key, val, tree);
  }
  
  return 0;
}


  for(int i = 0; i < tree->next_empty; i++){
    printf("key %i \n",tree->block[i].key);
    printf("value %i\n",tree->block[i].val);
  }
}

void print_disk_data(lsm* tree){
  printf("printing disk data\n");
  FILE* f = fopen(tree->disk1, "r"); 
  node *file_data;
  size_t num_elements = 0;
  int r;
  r = fread(&num_elements, sizeof(size_t), 1, f);
  if(r == 0){ 
    if(ferror(f)){
	perror("ferror\n");
    }
    else if(feof(f)){
      perror("EOF found\n");
    }
  }
  file_data = malloc(sizeof(node)*num_elements);
  if(file_data == NULL){
    perror("print_disk_data: unsuccessful allocation \n");
  }
  r = fread(file_data, sizeof(node), num_elements, f);
  if(r == 0){ 
    if(ferror(f)){
      perror("ferror\n");
    }
    else if(feof(f)){
      perror("EOF found\n");
    }
  }
  for(int i = 0; i < num_elements; i++){
    printf("key %d \n",file_data[i].key);
    printf("value %d\n",file_data[i].val);
  }
}
