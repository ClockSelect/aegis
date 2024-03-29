#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "aegis.h"

typedef struct rect
{
	uint8_t	left;
	uint8_t	top;
	uint8_t	right;
	uint8_t	bottom;
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
	void	(*Shutdown)( void );
	void	(*Sleep)( void );
	void	(*Wakeup)( void );
	void	(*SetColor)( uint32_t rgb );
	void	(*SetBgColor)( uint32_t rgb );
	void	(*CLS)( void );
	void	(*SetBrightness)( uint32_t b );
	void	(*SetFont)( const font_t* );
	void	(*Print)( const char*, const rect_t*, unsigned align );
}
display_t;


extern const display_t *display;
extern const display_t ST7735S;

extern DISPLAY_STATUS	display_status;

extern uint16_t fgcolor;
extern uint16_t bgcolor;


extern void	DMStartup( void );
extern void DMShutdown( void );
extern void	DMSetStatus( DISPLAY_STATUS status );
extern DISPLAY_STATUS DMGetStatus();
extern void DMSetBrightness( uint8_t b );
extern void DMDimmer( void );
extern void DMEvent( Event_t *ev );

extern void	FormatNumber( char *str, uint32_t num, int nbdig, int nbdec, int exp );


#endif /* __DISPLAY_H__ */
