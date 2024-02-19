#include "ih_vec.h"
#include <stdlib.h>

struct ih_vec {
  ih_entry storage[1024];
  int size;
};

ih_vec_t *ih_vec_create() {
  ih_vec_t *vec = (ih_vec_t *)malloc(sizeof(struct ih_vec));
  vec->size = 0;
  return vec;
}

void ih_vec_push(ih_vec_t *self, ih_entry entry) {
  self->storage[self->size] = entry;
  self->size += 1;
}

void ih_vec_remove_at(ih_vec_t *self, int index) {
  if (index >= self->size) {
    return;
  }
  for (int i = index; i < self->size - 1; i++) {
    self->storage[i] = self->storage[i + 1];
  }
  self->size -= 1;
}

ih_entry ih_vec_at(ih_vec_t *self, int index) { return self->storage[index]; }

int ih_vec_size(ih_vec_t *self) { return self->size; }

void ih_vec_destroy(ih_vec_t *self) { free(self); }