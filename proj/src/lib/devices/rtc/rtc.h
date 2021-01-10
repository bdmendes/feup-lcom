#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

#define RTC_IRQ 8
#define RTC_WAIT_DELAY_MS 20

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define RTC_MIN_YEAR 2000
#define RTC_DONT_CARE_VALUE BIT(7) | BIT(6)

/* Registers reserved for time-related functionality */
#define RTC_ADDR_SECONDS 0
#define RTC_ADDR_SEC_ALARM 1
#define RTC_ADDR_MINUTES 2
#define RTC_ADDR_MIN_ALARM 3
#define RTC_ADDR_HOUR 4
#define RTC_ADDR_HOUR_ALARM 5
#define RTC_ADDR_DAY_OF_WEEK 6
#define RTC_ADDR_DAY_OF_MONTH 7
#define RTC_ADDR_MONTH 8
#define RTC_ADDR_YEAR 9

/* Registers reserved for control of the RTC */
#define RTC_ADDR_A 10
#define RTC_ADDR_B 11
#define RTC_ADDR_C 12
#define RTC_ADDR_D 13

/* Register A control */
#define RTC_REG_A_UPDATE_IP BIT(7)
#define RTC_REG_A_RS3 BIT(3) // periodic int rate selector bit 2
#define RTC_REG_A_RS2 BIT(2) // periodic int rate selector bit 1
#define RTC_REG_A_RS1 BIT(1) // periodic int rate selector bit 0

/* Register B control */
#define RTC_REG_B_INHIBIT_UPDATES BIT(7)
#define RTC_REG_B_PERIODIC_IE BIT(6)
#define RTC_REG_B_ALARM_IE BIT(5)
#define RTC_REG_B_UPDATE_IE BIT(4)
#define RTC_REG_B_DM_BINARY BIT(2)
#define RTC_REG_B_SQW BIT(3)

/* Register C control */
#define RTC_REG_C_IRQF BIT(7)
#define RTC_REG_C_PF BIT(6)
#define RTC_REG_C_AF BIT(5)
#define RTC_REG_C_UF BIT(4)

/* Register D control */
#define RTC_REG_D_VALID_RT BIT(7)

/* RTC functions */

/**
 * @brief  reads register C to clear possible error flags and then subscribes RTC interrupts
 * @param  bit_no: the bit number
 * @retval OK if success, !OK otherwise
 */
int(rtc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief  unsubscribes RTC interrupts
 * @retval OK if success, !OK otherwise
 */
int(rtc_unsubscribe_int)();


/**
 * @brief  the RTC interrupt handler. Does nothing besides reading register C to clear the pending interrupt.
 * @retval None
 */
void(rtc_ih)();


/**
 * @brief  disables every type of RTC interrupts: alarm, periodic and update
 * @retval OK if succeess, !OK otherwise
 */
int(rtc_disable_all_interrupts)();


/**
 * @brief  sets the RTC to cause an alarm interrupt every second: the second, minute and hour registers are set with 11XXXXXX values.
 * @retval None
 */
void(rtc_set_alarm_every_second)();

/**
 * @brief  enables RTC alarm interrupts
 * @retval OK if successful, otherwise !OK
 */
int(rtc_enable_alarm_interrupts)();


#endif
