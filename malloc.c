#include "malloc.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"
#include "stdio.h"i
#include "pthread.h"
//=========================================================
// Your implementations HERE
//=========================================================

typedef struct block* mblock;
struct block{
  size_t size;
  mblock next;
  mblock prev;
  int free;
  void *ptr;
  char data[1]; 
};

//#define BLOCK_SIZE sizeof(struct block)
#define BLOCK_SIZE 24

static void *fblock=NULL;
pthread_mutex_t mutex;
mblock find_block(mblock* last,size_t size);
mblock heap_extra(mblock last,size_t size);

void* malloc_unsafe(size_t size);
void free_unsafe(void *ptr);

void* do_malloc(size_t size) {
    
    pthread_mutex_lock(&mutex);
    void *retval=malloc_unsafe(size);
    pthread_mutex_unlock(&mutex);
    //return malloc(size);
    return retval;//malloc_unsafe(size);
}

void do_free(void *ptr) {
    pthread_mutex_lock(&mutex);
    free_unsafe(ptr);
    pthread_mutex_unlock(&mutex);
    //free(ptr);
}

mblock find_block(mblock* last,size_t size){
  mblock b=fblock;
  while(b&&(!(b->free&&b->size>=size))){
    *last=b;
    b=b->next;
  }  
  return b;
}

mblock heap_extra(mblock last,size_t size){
  mblock new;
  new=(sbrk(0));
  if((int)sbrk(BLOCK_SIZE+size)<0)//(void *)-1
     return NULL;
  new->size=size;
  //new->prev=last;
  new->next=NULL;
  //new->ptr=new->data;
  if(last)
     last->next=new;
  new->free=0;
  return new;
}

void split(mblock blk,size_t size){
  mblock new;
  new=blk->data+size;
  new->size=blk->size-size-BLOCK_SIZE;
  new->next=blk->next;
  new->free=1;
  blk->size=size;
  blk->next=new;
}

void* malloc_unsafe(size_t size){
  mblock blk,last;
  if(size<=0)
     return NULL;
  size_t newsize;
  newsize=align4(size); 
  if(base){
    last=base;
    blk=find_block(&last,newsize);
    if(blk){
       //if((blk->size-newsize)>=(BLOCK_SIZE+4))
         // split(last,newsize);
       blk->free=0;
    }else{
       blk=heap_extra(last,newsize);
       if(!blk)
         return NULL;
    } 
  }else{
    blk=heap_extra(NULL,newsize);
    if(!blk)
       return NULL;
    fblock=blk;
  }
  return blk->data;
}




















