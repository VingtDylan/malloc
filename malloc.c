#include "malloc.h"
#include "unistd.h"
#include <stdlib.h>

//=========================================================
// Your implementations HERE
//=========================================================

typedef struct block *mblock;
struct block{
  size_t size;
  mblock next;
  int free;
  int padding;
  char data[1]; 
};

void *fblock=NULL;
mblock find_block(mblock *last,size_t size);
mblock heap_extra(mblock last,size_t size);
void split(mblock blk,size_t size);
size_t align8(size_t size);
void* malloc_unsafe(size_t size);

void* do_malloc(size_t size) {
    /*
    pthread_mutex_lock(&mutex);
    malloc_unsafe(size);
    pthread_mutex_unlock(&mutex);
    */
    //return malloc(size);
    return malloc_unsafe(size);
}

void do_free(void *ptr) {
    /*
    pthread_mutex_lock(&mutex);
    free_unsafe(ptr);
    pthread_mutex_unlock(&mutex);
    */
    free(ptr);
}

mblock find_block(mblock *last,size_t size){
  mblock b=fblock;
  while(b&&!(b->free&&b->size>=size)){
    *last=b;
    b=b->next;
  }  
  return b;
}

mblock heap_extra(mblock last,size_t size){
  mblock new;
  new=sbrk(0);
  if(sbrk(BLOCK_SIZE+size)==(void*)-1)
     return NULL;
  new->size=size;
  new->next=NULL;
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

size_t align8(size_t size){
  if((size&0x7)==0)
     return size;
  return ((size>>3)+1)<<3;
}

void* malloc_unsafe(size_t size){
  mblock blk,last;
  size_t newsize;
  newsize=align8(size); 
  if(fblock){
    last=fblock;
    blk=find_block(&last,newsize);
    if(blk){
       if((blk->size-newsize)>=(BLOCK_SIZE+8))
          split(last,newsize);
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




















