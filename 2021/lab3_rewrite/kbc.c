#include "kbc.h"

#include <lcom/lcf.h>

#include "kbc.h"

int kbd_hook_id;
uint8_t buffer_data = 0;

int (read_buffer_data)(){
    uint8_t st;

    if (util_sys_inb(KBC_STAT_REGISTER, &st) != OK){
        printf("error reading stat register");
        return !OK;
    }

    if (st & OBF){
        if (st & (ST_PARITY | ST_TIMEOUT | ST_AUX)){
            printf("stat error!");
            return !OK;
        }

        if (util_sys_inb(KBC_OUT_BUF, &buffer_data) != OK){
            printf("error reading output buffer");
            return !OK;
        }

        return OK;
    }

    else return !OK;
}

void (kbd_ih)(){
    read_buffer_data();
}

int (kbd_subscribe_int)(uint8_t* bit_no){
    kbd_hook_id = KDB_IRQ;
    *bit_no = BIT(kbd_hook_id);
    if (sys_irqsetpolicy(KDB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK){
        printf("error subscribing kbd int\n");
        return !OK;
    }
    return OK;
}

int (kbd_unsubscribe_int)(){
    if (sys_irqrmpolicy(&kbd_hook_id) != OK){
        printf("error unsubscribing kbd int\n");
        return !OK;
    }
    return OK;
}

bool (is_make_code)(uint8_t code){
    return !(BIT(7) & code);
}

void delay_milliseconds(uint32_t milliseconds){
    tickdelay(micros_to_ticks(milliseconds*1000));
}

int read_command_byte(uint8_t* cmd_byte){
    if (sys_outb(KBC_IN_BUF_COMMAND,READ_COMMAND_BYTE) != OK){
        printf("error attempting to read");
        return !OK;
    }

    if (util_sys_inb(KBC_OUT_BUF,cmd_byte) != OK){
        printf("error reading");
        return !OK;
    }
    return OK;
}

int write_command_byte(uint8_t cmd_byte){

    uint8_t st;
    if (util_sys_inb(KBC_STAT_REGISTER, &st) != OK){
        printf("error reading stat register");
        return !OK;
    }

    if (st & IBF){
        printf("ibf full!");
        return !OK;
    }

    return ((sys_outb(KBC_IN_BUF_COMMAND,WRITE_COMMAND_BYTE) != OK) |
        (sys_outb(KBC_IN_BUF_COMMAND_ARG,cmd_byte) != OK)) ? !OK : OK;
}

int (kbd_reenable_interrupts)(){
    uint8_t cmd_byte;
    read_command_byte(&cmd_byte);
    cmd_byte |= INT;
    return write_command_byte(cmd_byte);
}
