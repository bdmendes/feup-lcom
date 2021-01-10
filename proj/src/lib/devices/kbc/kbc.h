#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>

/* default delay for polling and wait mechanisms */
#define DEFAULT_DELAY_MILLISECONDS 20

/* registers */
#define KBC_IN_BUF_COMMAND_ARG 0x60
#define KBC_IN_BUF_COMMAND 0x64
#define KBC_OUT_BUF 0x60
#define KBC_STAT_REGISTER 0x64

/* status register */
#define ST_PARITY BIT(7)
#define ST_TIMEOUT BIT(6)
#define ST_AUX BIT(5)
#define INH BIT(4)
#define IBF BIT(1)
#define OBF BIT(0)

/* kbc commands */
#define READ_COMMAND_BYTE 0x20
#define WRITE_COMMAND_BYTE 0x60

/* kbc command byte */
#define INT BIT(0)  // enable interrupt on OBF from keyboard
#define DIS BIT(4)  // disable keyboard
#define INT2 BIT(1) // enable interrupt on OBF from mouse
#define DIS2 BIT(5) // disable mouse

/* kbc commands */
#define KBC_READ_COMMAND_BYTE 0x20
#define KBC_WRITE_COMMAND_BYTE 0x60

/* functions */

/**
 * @brief  reads the data in the output buffer
 * @param  aux_set: true if the data is from the mouse, false if it's from the
 * * keyboard
 * @retval OK if success, otherwise !OK
 */
int(read_buffer_data)(bool aux_set);

/**
 * @brief  gets the data previously retrieved from from the output buffer
 * @retval the byte read
 */
uint8_t(get_buffer_data)();

/**
 * @brief  checks if the input buffer is available to be written to (IBF is unset)
 * @retval true if can the input buffer is ok, false otherwise
 */
bool(kbc_can_write_to_inbuf)();

/**
 * @brief  retrieves the current KBC command byte 
 * @param  cmd_byte: the byte read
 * @retval OK if success, !OK otherwise
 */
int(kbc_read_command_byte)(uint8_t *cmd_byte);

/**
 * @brief  writes a command byte
 * @param  cmd_byte: the byte to be written
 * @retval OK if success, !OK otherwise
 */
int(kbc_write_command_byte)(uint8_t cmd_byte);

#endif
