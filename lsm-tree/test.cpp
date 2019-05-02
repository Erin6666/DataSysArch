#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "lsm.hpp"
#include "test.hpp"
#include <iostream>
#include <time.h>
using namespace std;


void test_print_tree(lsm* tree){
  
}


int test_put(lsm* tree, int data_size, int buffer_size, bool sorted, bool timing){

  srand((int)time(0));
  int r;
  clock_t start, end;
  start = clock();

  for(int i = 0; i < data_size; i++){
    keyType k;
    valType v;
    k = (keyType)i;
    v = std::to_string(i); 
	//cout<<"here"<<endl;
    r = put(&k,&v,tree);
	
	//cout<<i<<" "<<r;
    assert(r==0);
  }
  end = clock();
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r;
}

int test_delete(lsm* tree, int data_size, int nops, bool timing){
  /* Deletes a single item in range (0, `data_size`).*/
  int r = 0; 
  clock_t start, end;
  start = clock();
  for(int i = 0; i < nops; i++){
    keyType k;
    k = (keyType)((rand() % data_size));
	node* n=get(&k,tree);
	if(n!=NULL)
	{
	
    printf("deleting key: %d \n", k);
    r = lsmdelete(&k, tree);
	}
  }
  end = clock();
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r; 
}


int test_get(lsm* tree, int data_size, int nops, bool timing){
  int r = 0; 
  clock_t start, end;
  start = clock();
  for(int i = 0; i < nops; i++){
    keyType k;
    valType v;
    k = (keyType)(rand() % data_size-1);
    node* n = get(k, tree);
  }
  end = clock();
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r; 
};


int test_update(lsm* tree, int data_size, int nops, bool timing){
  /* Tests the update function.
     Args:
     tree: pointer to lsm object.
     data_size: int specifying size of data stored.
     nops: specifies number of times to run operation.*/
  int r = 0; 
  clock_t start, end;
  start = clock();
  for(int i = 0; i < nops; i++){
    keyType k;
    valType v;
    k = (keyType)(rand() % data_size-1);
    //int vt = (rand() % data_size-1);
	v= std::to_string(rand() % data_size-1);
    int r = update(&k, &v, tree);
  }
  end = clock();
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r;
}

int test_throughput(lsm* tree, int data_size, int buffer_size, bool sorted, int nops, float put_prob, float update_prob, bool timing){
  return 0;
}

int main(int argc, char* args[]){

  assert(argc >= 5); 
  clock_t start, end;

  int r;
  int data_size = atoi(args[1]);
  int buffer_size = atoi(args[2]);
  int nops = atoi(args[3]);
  string testing;
  testing=args[4];
  
  
  bool sorted = false;
  if(argc == 6){
    sorted = args[5];
  }
  // set probabilities for throughput. 
  float put_prob = 33.0; 
  float update_prob = 33.0; 
  if(argc == 8){
    put_prob = atoi(args[6]);
    update_prob = atoi(args[7]); 
  }
  
  lsm *tree;
  
  tree = init_new_lsm(buffer_size, sorted);
  //cout<<testing<<endl;
 	 bool a=(testing.compare("put")==0);
	bool b=(testing.compare("get")==0);
	bool c=(testing.compare("upd")==0);
	bool d=(testing.compare("del")==0);
 // cout<<a<<b<<c<<d<<endl;
  if(a==1){
    /* TEST PUT */ 
	//cout<<argc<<" "<<data_size<<buffer_size<<nops<<testing<<endl;
    r = test_put(tree, data_size, buffer_size, sorted, true);
  }
  if(b==1){
    /* TEST GET */ 
    r = test_put(tree, data_size, buffer_size, sorted, true);
    r = test_get(tree, data_size, nops, true);
  }
  if(c==1){
  /* TEST UPDATE */
    r = test_put(tree, data_size, buffer_size, sorted, true);
    r = test_update(tree, data_size, nops, true);
  }
  if(d==1){
  /* TEST Delete */;
  r = test_put(tree, data_size, buffer_size, sorted, true);
  r = test_delete(tree, data_size, nops,true);  
  }
  destruct_lsm(tree); 
  return r;
}
