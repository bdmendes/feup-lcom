#include <lcom/lcf.h>

#include "i8254.h"

int timer_hook_id = 0;
int timer_count = 0;

int (timer_subscribe_int)(uint8_t* bit_no){
    timer_hook_id = TIMER0_IRQ;
    *bit_no = BIT(timer_hook_id);
    if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK){
        printf("error subscribing kbd int\n");
        return !OK;
    }
    return OK;
}

int (timer_unsubscribe_int)(){
    if (sys_irqrmpolicy(&timer_hook_id) != OK){
        printf("error unsubscribing kbd int\n");
        return !OK;
    }
    return OK;
}

void (timer_int_handler)(){
    timer_count++;
}
