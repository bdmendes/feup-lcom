#include "bitwise.h"
#include "minix/syslib.h"
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(x, msg)                                                         \
  if (!(x)) {                                                                  \
    printf("%s FAILED!\n", msg);                                               \
    exit(1);                                                                   \
  } else {                                                                     \
    printf("%s passed.\n", msg);                                               \
  }

void test_bitwise() {
  ASSERT(clear(1, 0) == 0, "clear 0 op");
  ASSERT(clear(0, 0) == 0, "clear 0 nop");
  ASSERT(clear(7, 2) == 3, "clear 2");

  ASSERT(set(1, 0) == 1, "set 0 nop");
  ASSERT(set(0, 0) == 1, "set 0 op");
  ASSERT(set(3, 2) == 7, "set 2");

  ASSERT(is_set(3, 0), "is_set 0");
  ASSERT(is_set(3, 1), "is_set 1");
  ASSERT(!is_set(3, 2), "is_set 2");

  ASSERT(lsb(0xF4) == 0xF4, "lsb0");
  ASSERT(lsb(0xEEF4) == 0xF4, "lsb1");

  ASSERT(msb(0xF4) == 0, "msb0");
  ASSERT(msb(0xEEF4) == 0xEE, "msb1");

  ASSERT(mask(0, MSK_END) == 1, "mask simple");
  ASSERT(mask(0, 1, MSK_END) == 3, "mask complex");
  ASSERT(mask(0, 2, MSK_END) == 5, "mask interleaved");
}

void test_privileged_operation() {
  ASSERT(sys_outb(0x70, 0xA) == 0, "privileged I/O operation example");
}

int main() {
  // During initialization, a privileged user process should synchronize with
  // the RS. This will be done by LCOM facilities later on, but for now we call
  // sef_startup() ourselves.
  sef_startup();

  // System services have no controlling terminal and typically redirect their
  // output to some log file. For the purpose of this lab, we redirect stdout
  // and stderr to the console so we can see our test results. LCOM facilities
  // will handle this in a better way later on.
  freopen("/dev/console", "w", stdout);
  setvbuf(stdout, NULL, _IONBF, 0);

  // Run our tests.
  test_bitwise();
  test_privileged_operation();
  printf("All tests passed! Way to go.\n");
}
