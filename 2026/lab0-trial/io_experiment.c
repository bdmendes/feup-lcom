#include "minix/syslib.h"

int main() {
    sef_startup();
    freopen("/dev/console", "w", stdout);
    int ret = sys_outb(0x70, 0xA); // Request to read Register A of the RTC
    printf("sys_outb returned: %d\n", ret);
    return 0;
}
