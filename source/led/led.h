/*
 *
 *
 */

enum{
	LED_RED,
	LED_DERRN,
	LED_ALL_ON,
	LED_ALL_OFF
};

extern void led_init(void);
extern void led_contrl(unsigned char cnt);

