#include "rtc.h"
#include "../../utils/utils.h"

static int rtc_hook_id = RTC_IRQ;
static char current_time[17];

/**
 * @brief  sets the SET bit to signal RTC registers are being updated
 * @retval None
 */
static void(rtc_set_set_bit)();

/**
 * @brief  usets the SET bit so that the RTC register values may be read
 * @retval None
 */
static void(rtc_unset_set_bit)();

/**
 * @brief waits until the registers can be read (the UIP BIT is unset)
 * @retval None
 */
static void(wait_valid_rtc)();

/**
 * @brief  reads a byte from a register
 * @param  reg: the register to read from
 * @param  byte: the byte to return the read information
 * @retval OK if success, otherwise !OK
 */
static int(rtc_read_byte_from_reg)(uint8_t reg, uint8_t *byte);

/**
 * @brief  writes a byte to a register
 * @param  reg: the register to write the byte to
 * @param  byte: the byte to write
 * @retval OK if success, otherwise !OK
 */
static int(rtc_write_byte_to_reg)(uint8_t reg, uint8_t byte);

/**
 * @brief  disables RTC's alarm updates
 * @retval OK if success, otherwise !OK
 */
static int(rtc_disable_alarm_interrupts)();

/**
 * @brief  reads the hours register
 * @param  hour: the address to store the read hour
 * @retval OK if success, !OK otherwise
 */
static int(rtc_read_hour)(uint8_t *hour);

/**
 * @brief  reads the minutes register
 * @param  minute: the address to store the read minute
 * @retval OK if success, !OK otherwise
 */
static int(rtc_read_minute)(uint8_t *minute);

/**
 * @brief  reads the seconds register
 * @param  second: the address to store the read second
 * @retval OK if success, !OK otherwise
 */
static int(rtc_read_second)(uint8_t *second);

/**
 * @brief  reads the day of the month register
 * @param  month_day: the address to store the read day of the month
 * @retval OK if success, !OK otherwise
 */
static int(rtc_read_month_day)(uint8_t *month_day);

/**
 * @brief  reads the month register
 * @param  month: the address to store the read month
 * @retval OK if success, !OK otherwise
 */
static int(rtc_read_month)(uint8_t *month);

/**
 * @brief  reads the year register
 * @param  year: the address to store the read year
 * @retval OK if success, !OK otherwise
 */
static int(rtc_read_year)(uint8_t *month);

/**
 * @brief  reads all the time registers parsing them and storing in a string string
 * @retval OK if success, !OK otherwise
 */
static int(rtc_read_current_time)();

static int(rtc_read_byte_from_reg)(uint8_t reg, uint8_t *byte) {
  if (sys_outb(RTC_ADDR_REG, reg) != OK) {
    printf("Error: could not write to RTC_ADDR_REG\n");
    return !OK;
  }
  if (util_sys_inb(RTC_DATA_REG, byte) != OK) {
    printf("Error: could read from RTC_DATA_REG\n");
    return !OK;
  }
  return OK;
}

static int(rtc_write_byte_to_reg)(uint8_t reg, uint8_t byte) {
  if (sys_outb(RTC_ADDR_REG, reg) != OK) {
    printf("Error: could not write to RTC_ADDR_REG\n");
    return !OK;
  }
  if (sys_outb(RTC_DATA_REG, byte) != OK) {
    printf("Error: could write to RTC_DATA_REG\n");
    return !OK;
  }
  return OK;
}

static void(rtc_set_set_bit)() {
  uint8_t byte = 0;
  rtc_read_byte_from_reg(RTC_ADDR_B, &byte);
  byte |= RTC_REG_B_INHIBIT_UPDATES;
  rtc_write_byte_to_reg(RTC_ADDR_B, byte);
}

static void(rtc_unset_set_bit)() {
  uint8_t byte = 0;
  rtc_read_byte_from_reg(RTC_ADDR_B, &byte);
  byte &= ~RTC_REG_B_INHIBIT_UPDATES;
  rtc_write_byte_to_reg(RTC_ADDR_B, byte);
}

void(rtc_set_alarm_every_second)() {
  wait_valid_rtc();
  rtc_set_set_bit();
  rtc_write_byte_to_reg(RTC_ADDR_HOUR_ALARM, RTC_DONT_CARE_VALUE);
  rtc_write_byte_to_reg(RTC_ADDR_MIN_ALARM, RTC_DONT_CARE_VALUE);
  rtc_write_byte_to_reg(RTC_ADDR_SEC_ALARM, RTC_DONT_CARE_VALUE);
  rtc_unset_set_bit();
}

int(rtc_subscribe_int)(uint8_t *bit_no) {

  /* read register C to clear pending interrupts */
  uint8_t regC = 0;
  rtc_read_byte_from_reg(RTC_ADDR_C, &regC);

  *bit_no = rtc_hook_id;
  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hook_id) != OK) {
    printf("Error: could not subscribe RTC interruption\n");
    return !OK;
  }
  return OK;
}

int(rtc_unsubscribe_int)() {
  if (sys_irqrmpolicy(&rtc_hook_id) != OK) {
    printf("Error: could not unsubscribe RTC interruption\n");
    return !OK;
  }
  return OK;
}

int(rtc_enable_alarm_interrupts)() {
  uint8_t regB = 0;
  if (rtc_read_byte_from_reg(RTC_ADDR_B, &regB) != OK) {
    return !OK;
  }
  regB |= RTC_REG_B_ALARM_IE;
  if (rtc_write_byte_to_reg(RTC_ADDR_B, regB) != OK) {
    return !OK;
  }
  return OK;
}

static int(rtc_disable_alarm_interrupts)() {
  uint8_t regB;
  if (rtc_read_byte_from_reg(RTC_ADDR_B, &regB) != OK) {
    return !OK;
  }
  regB &= ~RTC_REG_B_ALARM_IE;
  if (rtc_write_byte_to_reg(RTC_ADDR_B, regB) != OK) {
    return !OK;
  }
  return OK;
}

static int(rtc_disable_update_interrupts)() {
  uint8_t regB;
  if (rtc_read_byte_from_reg(RTC_ADDR_B, &regB) != OK) {
    return !OK;
  }
  regB &= ~RTC_REG_B_UPDATE_IE;
  if (rtc_write_byte_to_reg(RTC_ADDR_B, regB) != OK) {
    return !OK;
  }
  return OK;
}

static int(rtc_disable_periodic_interrupts)() {
  uint8_t regB;
  if (rtc_read_byte_from_reg(RTC_ADDR_B, &regB) != OK) {
    return !OK;
  }
  regB &= ~RTC_REG_B_PERIODIC_IE;
  if (rtc_write_byte_to_reg(RTC_ADDR_B, regB) != OK) {
    return !OK;
  }
  return OK;
}

int(rtc_disable_all_interrupts)() {
  rtc_disable_alarm_interrupts();
  rtc_disable_periodic_interrupts();
  rtc_disable_update_interrupts();
  return OK;
}

static void(wait_valid_rtc)() {
  for (uint8_t regA = 0;;) {
    rtc_read_byte_from_reg(RTC_ADDR_A, &regA);
    if (!(regA & RTC_REG_A_UPDATE_IP))
      break;
    delay_milli_seconds(RTC_WAIT_DELAY_MS);
  }
}

static void(rtc_alarm_ih)() { return; }

void(rtc_ih)() {
  uint8_t regC = 0;
  rtc_read_byte_from_reg(RTC_ADDR_C, &regC);
  if (regC & RTC_REG_C_AF)
    rtc_alarm_ih();
  if (regC & RTC_REG_C_UF) {
    rtc_read_current_time();
  }
}


static int(rtc_read_hour)(uint8_t *hour) {
  return rtc_read_byte_from_reg(RTC_ADDR_HOUR, hour);
}

static int(rtc_read_minute)(uint8_t *minute) {
  return rtc_read_byte_from_reg(RTC_ADDR_MINUTES, minute);
}

static int(rtc_read_second)(uint8_t *second) {
  return rtc_read_byte_from_reg(RTC_ADDR_SECONDS, second);
}

static int(rtc_read_month_day)(uint8_t *month_day) {
  return rtc_read_byte_from_reg(RTC_ADDR_DAY_OF_MONTH, month_day);
}

static int(rtc_read_month)(uint8_t *month) {
  return rtc_read_byte_from_reg(RTC_ADDR_MONTH, month);
}

static int(rtc_read_year)(uint8_t *year) {
  return rtc_read_byte_from_reg(RTC_ADDR_YEAR, year);
}

static int(rtc_read_current_time)() {
  wait_valid_rtc();
  uint8_t hour = 0, minute = 0, second = 0, day = 0, month = 0, year = 0;

  if (rtc_read_hour(&hour) != OK | rtc_read_minute(&minute) != OK | rtc_read_second(&second) != OK
  | rtc_read_month_day(&day) != OK | rtc_read_month(&month) != OK | rtc_read_year(&year) != OK) {
    return !OK;
  }

  sprintf(current_time, "%02d/%02d/%04d %02d:%02d:%02d\n",
  convert_bcd_to_binary(day), convert_bcd_to_binary(month), 
  (uint32_t) convert_bcd_to_binary(year) + RTC_MIN_YEAR, convert_bcd_to_binary(hour),
  convert_bcd_to_binary(minute), convert_bcd_to_binary(second));
  return OK;
}

