#include "malloc.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"
#include "stdio.h"
#include "pthread.h"
//=========================================================
// Your implementations HERE
//=========================================================


void *fblock=NULL;
pthread_mutex_t mutex;

void* do_malloc(size_t size) {
    //pthread_mutex_lock(&mutex);
    //void *retval=malloc_unsafe(size);
    //pthread_mutex_unlock(&mutex);
    return malloc(size);
    //return retval;
}

void do_free(void *ptr) {
    //pthread_mutex_lock(&mutex);
    free_unsafe(ptr);
    //pthread_mutex_unlock(&mutex);
    free(ptr);
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
  if(sbrk(BLOCK_SIZE+size)==(void *)(-1))
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
  new=(void *)(blk->data+size);
  new->size=blk->size-size-BLOCK_SIZE;
  new->next=blk->next;
  new->free=1;
  blk->size=size;
  blk->next=new;
}

mblock get_block(void *ptr){
  char *tmp;
  tmp=ptr;
  return (ptr=tmp-=BLOCK_SIZE);
}

int valid_addr(void *ptr){
  if(fblock){
     if(ptr>fblock&&ptr<sbrk(0)){
        return (int)(ptr=(get_block(ptr))->ptr);
     }
  }
  return 0;
}

void* malloc_unsafe(size_t size){
  mblock blk,last;
  if(size<=0)
     return NULL;
  size_t newsize;
  newsize=align4(size); 
  if(fblock){
    last=fblock;
    blk=find_block(&last,newsize);
    if(blk){
       if((blk->size-newsize)>=(BLOCK_SIZE+4))
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

void free_unsafe(void *ptr){
  if(!ptr||(!valid_addr(ptr)))
     return ;
  mblock used;
  used=get_block(ptr);
  used->free=1;
}


















