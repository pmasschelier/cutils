#include <cutils/array_list.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  array_list_t a1 = array_list_init(int);
  for (int i = 0; i < 4925; i++) {
    array_list_push_back(&a1, &i);
  }
  for (int i = 0; i < 4925; i++) {
    assert(*get_array_list_ref(a1, i, int) == i);
  }
  array_list_deinit(&a1);
  return EXIT_SUCCESS;
}
