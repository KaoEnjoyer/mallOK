#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
// https://techlifecompilation.wordpress.com/2018/10/04/malloc1-heap-exploitation-101/

// error types
#define GOOD_ALLOC 0
#define BAD_ALLOC -1
#define MALLOK_FAILED (void *)(-1)

typedef struct _MemoryChunk {
  bool isUsed;
  size_t size; // size of memory of ptr
  void *ptr; // ptr to chunk's memory
  struct _MemoryChunk *prev;
  struct _MemoryChunk *next;
} MemoryChunk;

// heap consts
#define CHUNK_ARR_SIZE 33
#define ALIGN_VALUE 16

// public
void *mallOK(size_t size);
void freeOK(void *ptr);

// private
static int InitHeap();

size_t _alignt_size(size_t size);
static size_t GetPageSize();

size_t _get_chunk_array_id(size_t size);

static MemoryChunk *InitChunk(void *ptr, size_t size);

static MemoryChunk *FindChunkToAlloc(size_t size);

static MemoryChunk* DivideChunk(size_t size , MemoryChunk* from);

static size_t GetAligment(size_t size);

static size_t GetChunkSize(MemoryChunk* chunk);

struct _heap_chunk *_add_memory_to_id(size_t id, int size);

static void Panic(char *msg);

static void DebugChunk(MemoryChunk *mc);

// head of heap
typedef struct HeadType {
  size_t freeChunks;
  MemoryChunk *freeChunkArray[CHUNK_ARR_SIZE];
  // array contains free chunks of memory in i >= 0 & i< 31
  // and the i depends on how big chunk we want
  // now the i = log2(size)
  // on 32 array contains free not divided memory
  //
  // In every array field will be chunks linked list 
  // ordered by addres,
} HeadType;
