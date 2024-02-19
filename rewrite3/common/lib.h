#include <lcom/lcf.h>

int main(int argc, char *argv[]);

int util_sys_inb(int port, uint8_t *value);

void subscribe_device_interrupts(int irq_line, int policy, void (*func)(void));

void unsubscribe_device_interrupts(int irq_line);

void exit_interrupt_loop();

void interrupt_loop();