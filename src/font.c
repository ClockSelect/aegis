#include <stdint.h>
#include "font.h"

static const uint8_t simple_font_x20[] =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

static const uint8_t simple_font_x21[] =
{
	0b00000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b00000000,
	0b10000000,
	0b00000000
};

static const uint8_t simple_font_x2C[] =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b01000000,
	0b01000000,
	0b01000000
};

static const uint8_t simple_font_x2D[] =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b11111000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

static const uint8_t simple_font_x2E[] =
{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b01000000,
	0b00000000
};

static const uint8_t simple_font_x2F[] =
{
	0b00100000,
	0b00100000,
	0b01000000,
	0b01000000,
	0b01000000,
	0b01000000,
	0b10000000,
	0b10000000,
	0b00000000
};

static const uint8_t simple_font_x30[] =
{
	0b00000000,
	0b01110000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b01110000,
	0b00000000
};

static const uint8_t simple_font_x31[] =
{
	0b00000000,
	0b00100000,
	0b01100000,
	0b10100000,
	0b00100000,
	0b00100000,
	0b00100000,
	0b00100000,
	0b00100000,
	0b00000000
};

static const uint8_t simple_font_x32[] =
{
	0b00000000,
	0b01110000,
	0b10001000,
	0b00001000,
	0b00001000,
	0b00010000,
	0b00100000,
	0b01000000,
	0b11111000,
	0b00000000
};

static const uint8_t simple_font_x33[] =
{
	0b00000000,
	0b01110000,
	0b10001000,
	0b00001000,
	0b00110000,
	0b00001000,
	0b00001000,
	0b10001000,
	0b01110000,
	0b00000000
};

static const uint8_t simple_font_x34[] =
{
	0b00000000,
	0b00010000,
	0b00110000,
	0b01010000,
	0b01010000,
	0b10010000,
	0b11111000,
	0b00010000,
	0b00010000,
	0b00000000
};

static const uint8_t simple_font_x35[] =
{
	0b00000000,
	0b01111000,
	0b01000000,
	0b10000000,
	0b11110000,
	0b00001000,
	0b00001000,
	0b10001000,
	0b01110000,
	0b00000000
};

static const uint8_t simple_font_x36[] =
{
	0b00000000,
	0b01110000,
	0b10001000,
	0b10000000,
	0b11110000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b01110000,
	0b00000000
};

static const uint8_t simple_font_x37[] =
{
	0b00000000,
	0b11111000,
	0b00001000,
	0b00001000,
	0b00010000,
	0b00010000,
	0b00100000,
	0b00100000,
	0b00100000,
	0b00000000
};

static const uint8_t simple_font_x38[] =
{
	0b00000000,
	0b01110000,
	0b10001000,
	0b10001000,
	0b01110000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b01110000,
	0b00000000
};

static const uint8_t simple_font_x39[] =
{
	0b00000000,
	0b01110000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b01111000,
	0b00001000,
	0b10001000,
	0b01110000,
	0b00000000
};

static const uint8_t simple_font_x41[] =
{
	0b00000000,
	0b00010000,
	0b00101000,
	0b00101000,
	0b00101000,
	0b01000100,
	0b01111100,
	0b10000010,
	0b10000010,
	0b00000000
};

static const uint8_t simple_font_x42[] =
{
	0b00000000,
	0b11111000,
	0b10000100,
	0b10000100,
	0b11111100,
	0b10000100,
	0b10000100,
	0b10000100,
	0b11111000,
	0b00000000
};

static const uint8_t simple_font_x43[] =
{
	0b00000000,
	0b00111000,
	0b01000100,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b01000100,
	0b00111000,
	0b00000000
};

static const uint8_t simple_font_x44[] =
{
	0b00000000,
	0b11110000,
	0b10001000,
	0b10000100,
	0b10000100,
	0b10000100,
	0b10000100,
	0b10001000,
	0b11110000,
	0b00000000
};

static const uint8_t simple_font_x45[] =
{
	0b00000000,
	0b11111000,
	0b10000000,
	0b10000000,
	0b11111000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b11111000,
	0b00000000
};

static const uint8_t simple_font_x46[] =
{
	0b00000000,
	0b11111000,
	0b10000000,
	0b10000000,
	0b11111000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b00000000
};

static const uint8_t simple_font_x48[] =
{
	0b00000000,
	0b10000100,
	0b10000100,
	0b10000100,
	0b11111100,
	0b10000100,
	0b10000100,
	0b10000100,
	0b10000100,
	0b00000000
};

static const uint8_t simple_font_x4C[] =
{
	0b00000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b10000000,
	0b11111000,
	0b00000000
};

static const uint8_t simple_font_x4F[] =
{
	0b00000000,
	0b00111000,
	0b01000100,
	0b10000010,
	0b10000010,
	0b10000010,
	0b10000010,
	0b01000100,
	0b00111000,
	0b00000000
};

static const uint8_t simple_font_x52[] =
{
	0b00000000,
	0b11111000,
	0b10000100,
	0b10000100,
	0b11111000,
	0b10010000,
	0b10001000,
	0b10001000,
	0b10000100,
	0b00000000
};

static const uint8_t simple_font_x57[] =
{
	0b00000000, 0b00000000,
	0b10000100, 0b00100000,
	0b10001010, 0b00100000,
	0b01001010, 0b01000000,
	0b01001010, 0b01000000,
	0b01010001, 0b01000000,
	0b01010001, 0b01000000,
	0b00100000, 0b10000000,
	0b00100000, 0b10000000,
	0b00000000, 0b00000000
};

static const font_chardesc_t simple_font_chars_1[] =
{
	{ 3, simple_font_x20 },
	{ 2, simple_font_x21 }
};

static const font_chardesc_t simple_font_chars_2[] =
{
	{ 3, simple_font_x2C },
	{ 6, simple_font_x2D },
	{ 3, simple_font_x2E },
	{ 3, simple_font_x2F },
	{ 6, simple_font_x30 },
	{ 6, simple_font_x31 },
	{ 6, simple_font_x32 },
	{ 6, simple_font_x33 },
	{ 6, simple_font_x34 },
	{ 6, simple_font_x35 },
	{ 6, simple_font_x36 },
	{ 6, simple_font_x37 },
	{ 6, simple_font_x38 },
	{ 6, simple_font_x39 }
};

static const font_chardesc_t simple_font_chars_3[] =
{
	{ 7, simple_font_x41 },
	{ 7, simple_font_x42 },
	{ 7, simple_font_x43 },
	{ 6, simple_font_x44 },
	{ 6, simple_font_x45 },
	{ 6, simple_font_x46 },
	{ 0, 0 },
	{ 7, simple_font_x48 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 6, simple_font_x4C },
	{ 0, 0 },
	{ 0, 0 },
	{ 8, simple_font_x4F },
	{ 0, 0 },
	{ 0, 0 },
	{ 7, simple_font_x52 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 12, simple_font_x57 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 }
};

static const font_chunk_t simple_font_chunk_3 =
{
	0x41,
	0x5A,
	simple_font_chars_3,
	0
};

static const font_chunk_t simple_font_chunk_2 =
{
	0x2C,
	0x39,
	simple_font_chars_2,
	&simple_font_chunk_3
};

static const font_chunk_t simple_font_chunk_1 =
{
	0x20,
	0x21,
	simple_font_chars_1,
	&simple_font_chunk_2
};

const font_t simple_font = 
{
	10,
	10,
	&simple_font_chunk_1
};
