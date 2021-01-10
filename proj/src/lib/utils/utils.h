#ifndef UTILS_H
#define UTILS_H

#include <lcom/lcf.h>

/* functions from lab2 */

/**
 * @brief  gets the least significant byte from a 2 byte value
 * @param  val: the value
 * @param  lsb: where the LSB is stored
 * @retval OK if success, !OK otherwise
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief  gets the most significant byte from a 2 byte value
 * @param  val: the value
 * @param  msb: where the MSB is stored
 * @retval OK if success, !OK otherwise
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief  wraps the sys_inb() funtion to allow reading 1 byte values
 * @param  port: the port to read from
 * @param  value: the read value
 * @retval OK if success, !OK otherwise
 */
int(util_sys_inb)(int port, uint8_t *value);

/* other functions */

/**
 * @brief  sleep for a certain duration in milliseconds
 * @param  milli_seconds: the duration
 * @retval None
 */
void(delay_milli_seconds)(unsigned milli_seconds);

/**
 * @brief  sleep for a certain duration in microseconds
 * @param  micro_seconds: the duration
 * @retval None
 */
void(delay_micro_seconds)(unsigned micro_seconds);

/**
 * @brief  sleep for a certain duration in seconds
 * @param  seconds: the duration
 * @retval None
 */
void(delay_seconds)(unsigned seconds);

/**
 * @brief  gets a number's bits in a given position range
 * @param  start: the start of the position range
 * @param  end: the end of the position range
 * @retval the seleced bits
 */
uint32_t(bit_range)(uint8_t start, uint8_t end);

/**
 * @brief  Get the slicing of the value corresponding to the requested byte-
 * @param  value: the original value
 * @param  byte: the position of the byte to get
 * @retval requested byte, sliced and aligned right
 */
uint8_t(get_byte)(uint32_t value, uint8_t byte);

/**
 * @brief  converts a number from binary coded decimal to binary
 * @param  value: the bcd value
 * @retval the binary value
 */
uint8_t(convert_bcd_to_binary)(uint8_t value);

/**
 * @brief  converts a number from binary to binary coded decimal
 * @param  value: the binaty value
 * @retval the bcd value
 */
uint8_t(convert_binary_to_bcd)(uint8_t value);

#endif
