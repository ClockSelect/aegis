//==============================================================================
// aegis/src/screens/screens.h
//------------------------------------------------------------------------------
// Screens
//==============================================================================
#include "aegis.h"


typedef struct screen_elem_s
{
	void (*fct)( struct screen_elem_s * );
	rect_t		rect;
	VOI_ID		voi;
	uint8_t		draw;
	uint8_t		stat;
}
SCREEN_ELEM;


extern void	ScrMainInit( void );
extern int	ScrMainDraw( void );
extern int	ScrMainRefresh( void );
extern void	ScrMainClose( void );
extern int	ScrMainEvent( Event_t *ev );


extern void	ScrIdleInit( void );
extern int	ScrIdleDraw( void );
extern void	ScrIdleClose( void );
extern int	ScrIdleEvent( Event_t *ev );
