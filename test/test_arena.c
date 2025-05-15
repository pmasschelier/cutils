#include <assert.h>
#include <cutils/arena.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int *x;
  int *y;
  int *z;
  char *msg;
} vec3;

const char *msg1 = "This is a random point in the world";

void read_point(allocator_t *allocator, vec3 *point) {
  point->x = allocator->alloc(allocator, sizeof(int));
  *point->x = 34;
  point->y = allocator->alloc(allocator, sizeof(int));
  *point->y = 13;
  point->z = allocator->alloc(allocator, sizeof(int));
  *point->z = 45;
  point->msg = allocator->alloc(allocator, strlen(msg1));
  strcpy(point->msg, msg1);
}

int main(int argc, char *argv[]) {
  vec3 point;
  arena_t arena;
  arena_init(&arena);
  read_point(&arena.allocator, &point);
  assert(*point.x == 34);
  assert(*point.y == 13);
  assert(*point.z == 45);
  int ret = memcmp(point.msg, msg1, strlen(msg1));
  assert(ret == 0);
  arena_free(&arena);
  return EXIT_SUCCESS;
}
