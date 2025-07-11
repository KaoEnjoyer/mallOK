#include "mallok.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

// global head of heap ptr
static struct HeadType g_head;

static int InitHeap() {

  for (int i = 0; i < CHUNK_ARR_SIZE; i++) {
    g_head.freeChunkArray[i] = NULL;
  }
  size_t size = GetPageSize();
  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (ptr == NULL)
    Panic("mmap returen null");

  MemoryChunk *chunk = InitChunk(ptr, size);

  g_head.freeChunkArray[CHUNK_ARR_SIZE - 1] = chunk;
  g_head.freeChunks = 1;
  return 22;
}

MemoryChunk *InitChunk(void *ptr, size_t size) {
  MemoryChunk *chunk = (MemoryChunk *)ptr; // check this later
  chunk->size = size ;//carefull
  chunk->isUsed = false;
  chunk->ptr = (void *)((char *)(ptr) + sizeof(MemoryChunk));
  chunk->next = NULL;
  chunk->prev = NULL;
  return chunk;
}

MemoryChunk *FindChunkToAlloc(size_t size) {
  if (g_head.freeChunks == 0) {
    Panic("List of free chunks is empty");
  }
  int id = 0;
  for (id; id < CHUNK_ARR_SIZE - 1; id++) {
    if ((1 << id) >= size)
      break;
  }

  MemoryChunk *chunk = g_head.freeChunkArray[id];
  if (chunk == NULL) {
    if (g_head.freeChunkArray[id + 1] != NULL) {
      // TODO: pobieranie pamieci z wyższych binów , oraz optylalizcja wobec
      // fragmentacji
    }

      chunk = DivideChunk(size, g_head.freeChunkArray[CHUNK_ARR_SIZE-1]) ;
    return chunk;
    Panic("TUTAJ MA BYĆ IMPLEMENTACAJ POBIERANIA Z WILD CHUNKA ");
  }
  while (chunk != NULL) {
    if (chunk->isUsed)
      return chunk;
    chunk = chunk->next;
  }

  return NULL;
}

static void DebugChunk(MemoryChunk *mc) {

  printf("size: %zu, isUsed: %d , adr: %p, ptr: %p\n", mc->size, mc->isUsed, mc,
         mc->ptr);
}

static size_t GetPageSize() { return sysconf(_SC_PAGE_SIZE); }

void Panic(char *msg) {
  fprintf(stderr, "%s", msg);
  strerror(errno);
  exit(BAD_ALLOC);
}

static size_t GetAligment(size_t size) {
  // for now we assume that the aligmnt is to 16 bytes
  return 16 - (size % 16);

  // TODO: thats trash
}
static size_t GetChunkSize(MemoryChunk *chunk) {
  return chunk->size + sizeof(MemoryChunk);
}
static MemoryChunk *DivideChunk(size_t size, MemoryChunk *from) {
  if (size + sizeof(MemoryChunk) > from->size) {
    Panic("this chunk can be divided!!");
  } else {
    
    MemoryChunk *temp = InitChunk(from->ptr, size);
    from->ptr = (char*)(from->ptr) + GetChunkSize(temp);
    from->size -= size;    
    return temp;
  }
  return NULL;
}

void *mallOK(size_t size) {
  if (g_head.freeChunkArray[CHUNK_ARR_SIZE - 1] == NULL) {
    printf("okej inicjujemy\n");
    InitHeap();
  }
  MemoryChunk *chunk = FindChunkToAlloc(size);

  return chunk->ptr;
  DebugChunk(chunk);
  return 0;
}
