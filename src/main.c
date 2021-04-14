#include "mgos.h"
#include "nixie.h"
#include "mgos_timers.h"
#include "mgos_time.h"
#include "mgos_sys_config.h"

void timer_cb(void *arg)
{
    int hour;
    char buf[10];
    time_t now;
    time(&now);
    now += mgos_sys_config_get_nixie_utcoffset()*60*60;

    mgos_strftime(buf, sizeof(buf), "%H", now);
    hour = (buf[0]-'0')*10 + (buf[1]-'0');

    // Reduce brightness when it's (probably) dark outside
    int brightness =  mgos_sys_config_get_nixie_brightness();
    if ((hour >= 20) || (hour <= 7))  brightness /= 4;

    if (mgos_sys_config_get_nixie_militarytime())
    {
      mgos_strftime(buf, sizeof(buf), "%H%M", now);
    }
    else
    {
      mgos_strftime(buf, sizeof(buf), "%I%M", now);
    }

    // Set RGB LED backlights
    int r = ((mgos_sys_config_get_nixie_backlight()>>16)&0xFF)*brightness/127;
    int g = ((mgos_sys_config_get_nixie_backlight()>>8)&0xFF)*brightness/127;
    int b = ((mgos_sys_config_get_nixie_backlight()>>0)&0xFF)*brightness/127;
    LOG(LL_INFO, ("RGB: %i %i %i", r, g, b));
    set_led(2, r, g, b);
    set_led(3, r, g, b);
    set_led(4, r, g, b);

    if (buf[0] == '0')
    {
      // Turn nixie tube off, rather than show a 'zero'
      nixie_clear(1);
    }
    else
    {
      set_led(1, r, g, b);
      show_digit(1, (unsigned char)(buf[0]-'0'), brightness, 0);
    }
    show_digit(2, (unsigned char)(buf[1]-'0'), brightness, 0);
    show_digit(3, (unsigned char)(buf[2]-'0'), brightness, 0);
    show_digit(4, (unsigned char)(buf[3]-'0'), brightness, 0);
    set_dots(2, 0, brightness);
}

enum mgos_app_init_result mgos_app_init(void)
{
  if (mgos_sys_config_get_nixie_cs1_gpio() >= 0) {
    if (mgos_gpio_set_mode(mgos_sys_config_get_nixie_cs1_gpio(), MGOS_GPIO_MODE_OUTPUT)) {
      mgos_gpio_write(mgos_sys_config_get_nixie_cs1_gpio(), 1);
    } else {
      LOG(LL_INFO, ("Error setting cs1_gpio"));
    }
  }
  if (mgos_sys_config_get_nixie_cs2_gpio() >= 0) {
    if (mgos_gpio_set_mode(mgos_sys_config_get_nixie_cs2_gpio(), MGOS_GPIO_MODE_OUTPUT)) {
      mgos_gpio_write(mgos_sys_config_get_nixie_cs2_gpio(), 1);
    } else {
      LOG(LL_INFO, ("Error setting cs2_gpio"));
    }
  }
  if (mgos_sys_config_get_nixie_cs3_gpio() >= 0) {
    if (mgos_gpio_set_mode(mgos_sys_config_get_nixie_cs3_gpio(), MGOS_GPIO_MODE_OUTPUT)) {
      mgos_gpio_write(mgos_sys_config_get_nixie_cs3_gpio(), 1);
    } else {
      LOG(LL_INFO, ("Error setting cs3_gpio"));
    }
  }
  if (mgos_sys_config_get_nixie_cs4_gpio() >= 0) {
    if (mgos_gpio_set_mode(mgos_sys_config_get_nixie_cs4_gpio(), MGOS_GPIO_MODE_OUTPUT)) {
      mgos_gpio_write(mgos_sys_config_get_nixie_cs4_gpio(), 1);
    } else {
      LOG(LL_INFO, ("Error setting cs4_gpio"));
    }
  }

  mgos_set_timer(1000, true, timer_cb, NULL);

  return MGOS_APP_INIT_SUCCESS;
}
