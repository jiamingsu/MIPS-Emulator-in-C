
#ifndef HEAP_H_
#define HEAP_H_

#include <inttypes.h>
#include "uthash.h"

extern uint32_t HEAP_END;
extern uint32_t BLOCKNUM;
extern uint32_t current_break;

typedef struct heap_stat {

  uint32_t addr;
  int      status; 
  UT_hash_handle hh;
} heap_stat;

extern struct heap_stat *HEAPSTATUS ;


 extern void initHeap();
 extern void heapDump();
 extern uint32_t mm_malloc(uint32_t size);
 extern void mm_free(uint32_t addr);
 extern uint32_t mm_sbrk(int32_t value);

#endif



