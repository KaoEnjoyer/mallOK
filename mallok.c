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

struct _heap_type;
struct _heap_chunk;

// size_t _align_memory

size_t get_page_size() { return sysconf(_SC_PAGE_SIZE); }
int _init_heap(struct _heap_type *heap, size_t size);

struct _heap_chunk {
  short is_used;
  size_t size;
  void *ptr; // ptr to chunk memory
  struct _heap_chunk *next;
};

struct _heap_type {
  size_t empty_chunks;
  struct _heap_chunk *start;
};

struct _heap_type _head;

struct _heap_chunk *_find_chunk(void *ptr_to_memory) {
  struct _heap_chunk *current = _head.start;
  while (current != NULL) {
    if (current->ptr == ptr_to_memory)
      return current;
    current = current->next;
  }
  return NULL;
}

void *mallOK(size_t size) {
  if (_head.start == NULL) {
    _init_heap(&_head, size);
  }

  struct _heap_chunk *toReturn = _head.start;

  while (toReturn->is_used || toReturn->size < size) {
    toReturn = toReturn->next;
    if (toReturn == NULL)
      break;
  }

  if (toReturn == NULL) {
    fprintf(stderr, "MALLOK COULD NOT FIND FREE CHUNK %u , %u \n",
            _head.start->is_used, _head.start->size);
    return MALLOK_FAILED;
  }
//https://stackoverflow.com/questions/8752546/how-does-malloc-understand-alignment
  size_t offset = size;
  if (_head.start->next == NULL) {
    _head.start = (void *)((char *)_head.start + size);
    _head.start->size = toReturn->size - size;
    _head.start->is_used = 0;
  } else {
    _head.start->next = (void *)((char *)_head.start + size);
    _head.start = _head.start->next;
    _head.start->size = _head.start->size + (toReturn->size - size);
    _head.start->is_used = 0;
  }

  printf("next chunk size: %u\n", _head.start->size);
  return toReturn;
}

int _init_heap(struct _heap_type *heap, size_t size) {
  void *ptr = mmap(NULL, get_page_size(), PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1,
                   0); // dowiedz się dalczego można tutaj dać -1
  if (ptr == MAP_FAILED) {
    fprintf(stderr, "BAD ALLOC MMAP FAILED\n");
    exit(BAD_ALLOC);
  }

  struct _heap_chunk *chunk = (struct _heap_chunk *)ptr;
  chunk->size = get_page_size() - sizeof(struct _heap_chunk);
  chunk->is_used = 0;
  chunk->next = NULL;
  chunk->ptr = chunk + 1;
  heap->start = chunk;
  heap->empty_chunks = 1;
  return GOOD_ALLOC;
}

void MallNotOk(void *ptr) {
  // dealocation of memory (like free, delete)
  printf("idk czy to tak działa %d", sizeof(ptr));
}

struct test {
  int a;
  int b;
  char c;
};

int main(int argc, char *argv[]) {

  int *foo = (int *)mallOK(sizeof(int));
  int *faoo = (int *)mallOK(sizeof(int));
  *foo = 22;
  printf("value: %d, addres: %p\n", *foo, foo);
  printf("first chunk is used: %d\n", _head.start->is_used);
  int *bar = (int *)mallOK(sizeof(int));

  struct test *T = (struct test *)mallOK(sizeof(struct test));
  T->a = 2;
  T->b = 33;
  T->c = 'c';

  printf("struct T a: %d , b: %d , c: %c , addres: %p", T->a, T->b, T->c, T);
  printf("{%d}", sizeof(T));
  MallNotOk(T);
}
