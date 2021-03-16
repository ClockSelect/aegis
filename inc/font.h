#ifndef __FONT_H__
#define __FONT_H__

typedef struct font_chardesc
{
	const uint16_t	width;
	const uint8_t	*bitmap;
}
font_chardesc_t;

typedef struct font_chunk
{
	const uint16_t	char_min;
	const uint16_t	char_max;
	const font_chardesc_t *chars;
	const struct font_chunk *next;
}
font_chunk_t;

typedef struct font
{
	const uint16_t	char_height;
	const uint16_t	line_height;
	const font_chunk_t *chunk;
}
font_t;


extern const font_t simple_font;

#endif /* __FONT_H__ */
