#include <cstddef>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#define GOOD_ALLOC 0
#define BAD_ALLOC -1
#define MALLOK_FAILED (void *)(-1)
#define FREE_LIST_ARRAY_SIZE 33
#define ALIGN_VALUE 16


struct _heap_type;
struct _heap_chunk;

void *mallOK(size_t size);

int _init_heap(struct _heap_type*heap, size_t);

size_t _alignt_size(size_t size);
size_t _get_page_size();

size_t _get_chunk_array_id(size_t size);
struct _heap_chunk* _add_memory_to_id(size_t id, int size);

struct _heap_chunk {
  short is_used;
  size_t size;
  void *ptr; // ptr to chunk memory
  struct _heap_chunk *next;
};

struct _heap_type {
  size_t empty_chunks;
  struct _heap_chunk *free_chunk_array[33];
  // array contains free chunks of memory in i >= 0 & i< 31 
  // and the i depends on how big chunk we want 
  // now the i = log2(size)
  // on 32 array contains free not divided memory
};
