//==============================================================================
// aegis/src/screens/screens.h
//------------------------------------------------------------------------------
// Screens
//==============================================================================
#include "aegis.h"


extern void	ScrMainInit( void );
extern int	ScrMainDraw( void );
extern int	ScrMainRefresh( void );
extern void	ScrMainClose( void );
extern int	ScrMainEvent( Event_t *ev );

extern void	ScrIdleInit( void );
extern int	ScrIdleDraw( void );
extern void	ScrIdleClose( void );
extern int	ScrIdleEvent( Event_t *ev );
