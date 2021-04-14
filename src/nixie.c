#include "nixie.h"
#include "mgos.h"
#include "mgos_spi.h"
#include "mgos_timers.h"

unsigned char spi_buf[EXIXE_SPI_BUF_SIZE];

// cap digit between 1 and 10
static unsigned char cap_digit(unsigned char digit)
{
  digit = digit % 10;
  if(digit == 0)
    digit = 10;
  return digit;
}

// cap a float between 0 and 127
static unsigned char cap_float(float temp)
{
  if(temp > 127)
    temp = 127;
  if(temp < 0)
    temp = 0;
  return (unsigned char)temp;
}

static void spi_write(int position)
{
  struct mgos_spi *spi;
  int cs_gpio;

  if (position == 1) cs_gpio = mgos_sys_config_get_nixie_cs1_gpio();
  else if (position == 2) cs_gpio = mgos_sys_config_get_nixie_cs2_gpio();
  else if (position == 3) cs_gpio = mgos_sys_config_get_nixie_cs3_gpio();
  else if (position == 4) cs_gpio = mgos_sys_config_get_nixie_cs4_gpio();
  else return;

  /* Global SPI instance is configured by the `spi` config section. */
  spi = mgos_spi_get_global();
  if (spi == NULL) {
    LOG(LL_ERROR, ("SPI is not configured, make sure spi.enable is true"));
    return;
  }

  struct mgos_spi_txn txn = {
      .cs = 0, /* Use CS0 line as configured by cs0_gpio */
      .mode = 0,
      .freq = 1000000,
  };
  /* Half-duplex, command/response transaction setup */
  txn.hd.tx_len = EXIXE_SPI_BUF_SIZE;
  txn.hd.tx_data = spi_buf;
  txn.hd.dummy_len = 0;   /* No dummy bytes necessary. */
  txn.hd.rx_len = 0;
  txn.hd.rx_data = NULL;
  //LOG(LL_INFO, ("SPI: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x", spi_buf[0], spi_buf[1], spi_buf[2], spi_buf[3], spi_buf[4], spi_buf[5], spi_buf[6], spi_buf[7], spi_buf[8], spi_buf[9], spi_buf[10], spi_buf[11], spi_buf[12], spi_buf[13], spi_buf[14], spi_buf[15]));

  mgos_gpio_write(cs_gpio, 0);
  if (!mgos_spi_run_txn(spi, false /* full_duplex */, &txn)) {
    LOG(LL_ERROR, ("SPI transaction failed"));
  }
  mgos_gpio_write(cs_gpio, 1);
}

// Turn off all digits and LED
void nixie_clear(unsigned char position)
{
  memset(spi_buf, 0x80, EXIXE_SPI_BUF_SIZE);
  spi_buf[0] = EXIXE_SPI_HEADER;
  spi_write(position);
}


void show_digit(unsigned char position, unsigned char digit, unsigned char brightness, unsigned char overdrive)
{
  // cap brightness to 127
  brightness &= 0x7f;
  digit = cap_digit(digit);

  memset(spi_buf, 0, EXIXE_SPI_BUF_SIZE); // clear SPI buffer
  memset(spi_buf, 0x80, EXIXE_SPI_BUF_SIZE - 5); // set EN bit to 1 for all digits
  spi_buf[0] = overdrive ? EXIXE_SPI_HEADER_OD : EXIXE_SPI_HEADER; // first byte, header
  spi_buf[digit] = 0x80 | brightness; // set digit brightness

  spi_write(position);
}

void set_led(unsigned char position, unsigned char red, unsigned char green, unsigned char blue)
{
  // cap brightness to 127
  red &= 0x7f;
  green &= 0x7f;
  blue &= 0x7f;

  memset(spi_buf, 0, EXIXE_SPI_BUF_SIZE - 3); // clear SPI buffer
  spi_buf[0] = EXIXE_SPI_HEADER;
  spi_buf[13] = 0x80 | red; // set LED colors
  spi_buf[14] = 0x80 | green;
  spi_buf[15] = 0x80 | blue;

  spi_write(position);
}

void set_dots(unsigned char position, unsigned char left_brightness, unsigned char right_brightness)
{
  // cap brightness to 127
  left_brightness &= 0x7f;
  right_brightness &= 0x7f;

  memset(spi_buf, 0, EXIXE_SPI_BUF_SIZE);
  spi_buf[0] = EXIXE_SPI_HEADER;
  spi_buf[11] = 0x80 | left_brightness;
  spi_buf[12] = 0x80 | right_brightness;

  spi_write(position);
}
