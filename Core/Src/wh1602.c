#include <stdint.h>
#include <string.h>
#include "wh1602.h"
#include "hw_delay.h"

#define LCD_BIT_7_MASK	    (0x80)
#define LCD_BIT_6_MASK	    (0x40)
#define LCD_BIT_5_MASK	    (0x20)
#define LCD_BIT_4_MASK	    (0x10)
#define LCD_BIT_3_MASK	    (0x08)
#define LCD_BIT_2_MASK	    (0x04)
#define LCD_BIT_1_MASK	    (0x02)
#define LCD_BIT_0_MASK	    (0x01)

static void lcd_write_command_init(uint8_t data);
static void lcd_set_nibble(uint8_t data);
inline static void lcd_write(uint8_t data);
inline static void lcd_wait(void);
static void lcd_write_command(uint8_t data);
static void lcd_write_data(uint8_t data);
static void lcd_strobe_e(void);
static void lcd_gpio_init();

static void lcd_set_nibble(uint8_t data)
{
    if (data & LCD_BIT_7_MASK)
    {
        HAL_GPIO_WritePin(LCD_DB7_PORT, LCD_DB7_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LCD_DB7_PORT, LCD_DB7_PIN, GPIO_PIN_RESET);
    }

    if (data & LCD_BIT_6_MASK)
    {
        HAL_GPIO_WritePin(LCD_DB6_PORT, LCD_DB6_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LCD_DB6_PORT, LCD_DB6_PIN, GPIO_PIN_RESET);
    }

    if (data & LCD_BIT_5_MASK)
    {
        HAL_GPIO_WritePin(LCD_DB5_PORT, LCD_DB5_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LCD_DB5_PORT, LCD_DB5_PIN, GPIO_PIN_RESET);
    }

    if (data & LCD_BIT_4_MASK)
    {
        HAL_GPIO_WritePin(LCD_DB4_PORT, LCD_DB4_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LCD_DB4_PORT, LCD_DB4_PIN, GPIO_PIN_RESET);
    }
}

static void lcd_strobe_e(void)
{
	HAL_GPIO_WritePin(LCD_E_PORT, LCD_E_PIN, GPIO_PIN_SET);
	delay_us(LCD_E_PULSE_DURATION);
	HAL_GPIO_WritePin(LCD_E_PORT, LCD_E_PIN, GPIO_PIN_RESET);
}

static void lcd_write_command_init(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
	lcd_set_nibble(data);
	lcd_strobe_e();
}

inline static void lcd_write(uint8_t data)
{
	lcd_set_nibble(data);
	lcd_strobe_e();

	lcd_set_nibble((data << 4));
	lcd_strobe_e();
}

inline static void lcd_wait(void)
{
	delay_us(LCD_WAIT_DELAY_US);
}

static void lcd_write_command(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
	lcd_write(data);
	lcd_wait();
}

static void lcd_write_data(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);
	lcd_write(data);
	lcd_wait();
}

static void lcd_gpio_init()
{
    GPIO_InitTypeDef gpio;

    /* Enable LCD GPIOs clocks */
    __HAL_RCC_GPIOE_CLK_ENABLE();

    memset(&gpio, 0, sizeof(GPIO_InitTypeDef));

    gpio.Speed = GPIO_SPEED_MEDIUM;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_NOPULL;

    gpio.Pin = LCD_RS_PIN;
    HAL_GPIO_Init(LCD_RS_PORT, &gpio);

    gpio.Pin = LCD_RW_PIN;
    HAL_GPIO_Init(LCD_RW_PORT, &gpio);

    gpio.Pin = LCD_E_PIN;
    HAL_GPIO_Init(LCD_E_PORT, &gpio);

    gpio.Pin = LCD_DB4_PIN;
    HAL_GPIO_Init(LCD_DB4_PORT, &gpio);

    gpio.Pin = LCD_DB5_PIN;
    HAL_GPIO_Init(LCD_DB5_PORT, &gpio);

    gpio.Pin = LCD_DB6_PIN;
    HAL_GPIO_Init(LCD_DB6_PORT, &gpio);

    gpio.Pin = LCD_DB7_PIN;
    HAL_GPIO_Init(LCD_DB7_PORT, &gpio);
}

void lcd_init(void)
{
    lcd_gpio_init();

    HAL_GPIO_WritePin(LCD_DB4_PORT, LCD_DB4_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_DB5_PORT, LCD_DB5_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_DB6_PORT, LCD_DB6_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_DB7_PORT, LCD_DB7_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_E_PORT, LCD_E_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RW_PORT, LCD_RW_PIN, GPIO_PIN_RESET);

    delay_ms(100);
    lcd_write_command_init(0x30);
    delay_ms(10);
    lcd_write_command_init(0x30);
    delay_ms(1);
    lcd_write_command_init(0x30);
    lcd_wait();
    lcd_write_command_init(0x20);
    lcd_write_command(0x28);
    lcd_write_command(0x08);
    lcd_write_command(0x0C);
    lcd_write_command(0x01);
    delay_ms(2);
    lcd_write_command(0x06);
}

void lcd_print_char(char c)
{
	lcd_write_data((uint8_t)c);
}

void lcd_print_string(const char* str)
{
	while (*str)
	{
		lcd_print_char(*str++);
	}
}

void lcd_move_to(uint8_t x, uint8_t y)
{
	uint8_t py = 0xFF;

    if (x > 15 || y > 1)
    {
        return;
    }

	switch (y)
	{
        case 0:
        {
            py = 0;
            break;
        }

        case 1:
        {
            py = 0x40;
            break;
        }

        default:
        {
            break;
        }
	}

    if (py != 0xFF)
    {
        py += x;
        py |= 0x80;
        lcd_write_command(py);
    }
}

void lcd_print_char_at(char c, uint8_t x, uint8_t y)
{
	lcd_move_to(x, y);
	lcd_print_char(c);
}

void lcd_print_string_at(const char* str, uint8_t x, uint8_t y)
{
	lcd_move_to(x, y);
	lcd_print_string(str);
}

void lcd_clear(void)
{
	lcd_write_command(0x01);//Clear display
	delay_ms(2);
}
