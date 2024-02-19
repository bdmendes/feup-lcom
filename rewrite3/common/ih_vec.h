typedef struct ih_vec ih_vec_t;

typedef struct {
  int irq_line;
  int hook_id;
  int bit_no;
  void (*ih)(void);
} ih_entry;

ih_vec_t *ih_vec_create();

void ih_vec_push(ih_vec_t *self, ih_entry entry);

void ih_vec_remove_at(ih_vec_t *self, int index);

ih_entry ih_vec_at(ih_vec_t *self, int index);

int ih_vec_size(ih_vec_t *self);

void ih_vec_destroy(ih_vec_t *self);