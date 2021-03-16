#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "aegis.h"

typedef struct rect
{
	uint16_t	left;
	uint16_t	top;
	uint16_t	right;
	uint16_t	bottom;
}
rect_t;

typedef enum eDisplayStatus
{
	DISPLAY_ON = 0,
	DISPLAY_OFF,
	DISPLAY_SLEEPING
}
DISPLAY_STATUS;

typedef struct display
{
	void	(*Startup)( void );
	void	(*Sleep)( void );
	void	(*Wakeup)( void );
	void	(*SetColor)( uint32_t rgb );
	void	(*SetBgColor)( uint32_t rgb );
	void	(*CLS)( void );
	void	(*SetBrightness)( uint32_t b );
	void	(*SetFont)( const font_t* );
	void	(*Print)( const char*, const rect_t* );
}
display_t;


extern const display_t *display;


extern const display_t ST7735S;

extern DISPLAY_STATUS	display_status;

extern uint16_t fgcolor;
extern uint16_t bgcolor;

extern void	SetDisplay( const display_t *d );
extern void	SetDisplayStatus( uint32_t status );
extern DISPLAY_STATUS GetDisplayStatus( void );
extern void	FormatNumber( char *str, uint32_t num, int nbdig, int nbdec, int exp );

#endif /* __DISPLAY_H__ */
