#ifndef exixe_h
#define exixe_h

#define EXIXE_SPI_BUF_SIZE (16)
#define EXIXE_SPI_HEADER (0xaa)
#define EXIXE_SPI_HEADER_OD (0xab)

void show_digit(unsigned char position, unsigned char digit, unsigned char brightness, unsigned char overdrive);
void set_dots(unsigned char position, unsigned char left_brightness, unsigned char right_brightness);
void set_led(unsigned char position, unsigned char red, unsigned char green, unsigned char blue);
void nixie_clear(unsigned char position);

#endif
