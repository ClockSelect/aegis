#ifndef __AEGIS_H__
#define __AEGIS_H__
//==============================================================================
// aegis/inc/aegis.h
//------------------------------------------------------------------------------
// Constants, Type definitions and Function prototypes
// for the Aegis Legend firmware.
//==============================================================================

#include <NUC1261.h>

#include "printf.h"
#include "devices.h"
#include "font.h"
#include "display.h"


//------------------------------------------------------------------------------
// Battery Voltage Offsets
// Correction values from ADC readings (mV)
//------------------------------------------------------------------------------
// Those values may vary from box to box. One may test the voltage values
// delivered by the ADC with accurate voltmeter measures to find out the
// exact values needed for his specific box.
//------------------------------------------------------------------------------
#define BVO_CELL1	(+40)
#define BVO_CELL2	(-65)
#define BVO_USB		(+365)


//------------------------------------------------------------------------------
// Input buttons GPIO ports
//------------------------------------------------------------------------------
#define BTN_FIRE	PE10
#define	BTN_MINUS	PE13
#define BTN_PLUS	PD0


//------------------------------------------------------------------------------
//	Events constants
//------------------------------------------------------------------------------

typedef enum eEvent
{
	EVENT_NULL = 0,			// Null event
	EVENT_KEY,				// Key input event
	EVENT_DISPLAY,			// Display event
	EVENT_MAX				// Total number of event types
}
Event_e;

typedef enum eEV_K
{
	EV_K_NULL = 0,
	EV_K_FIRE,				// Fire Button
	EV_K_PLUS,				// (+) Button
	EV_K_MINUS,				// (-) Button
	EV_K_FP,				// ( Fire, + )
	EV_K_FM,				// ( Fire, - )
	EV_K_MP,				// ( +, - )
	EV_K_FMP,				// ( Fire, +, - )
	EV_K_TEST,				// { ---+++ }
	EV_K_ONOFF				// 5 * Fire
}
EV_K_e;

typedef enum eEV_D
{
	EV_D_NULL = 0,
	EV_D_DISPLAY_STATUS,	// Display on/off/sleep
	EV_D_SCREEN,			// Invoke screen
	EV_D_BRIGHTNESS			// Set screen brightness
}
EV_D_e;


//------------------------------------------------------------------------------
//	Event structure
//------------------------------------------------------------------------------
//	An event consist of four bytes:
//	- The event type ( Key, screen, system, ... )
//	- The event subtype, depending on the event type ( Key id, ... )
//	- Two parameters, p1 and p2, depending on the event.
//------------------------------------------------------------------------------
//	Depending on the situation, events may be passed over as a pointer on an
//	event structure or as a 32 bits integer, wichever is handiest.
//------------------------------------------------------------------------------
typedef struct sEvent
{
	Event_e	type;
	union {
		EV_K_e	k;
		EV_D_e	d;
		uint8_t	id;
	};
	uint8_t	p1;
	uint8_t	p2;
}
Event_t;

// Enforce a 4-bytes structure.
_Static_assert( sizeof(Event_t) == 4 , "Bad event structure size" );

// Build an integer event from 4 bytes.
#define EVENT(a,b,c,d)	((uint32_t)(((uint32_t)(d)) << 24 | \
									((uint32_t)((uint8_t)(c))) << 16 | \
									((uint32_t)((uint8_t)(b))) <<  8 | \
									((uint8_t)(a))))

// Send event after some time delay
#define DELAYED_EVENT(t,a,b,c,d)	\
	TMCreateTask(t,(void(*)(uint32_t))EMSendEventInt,EVENT(a,b,c,d),0,0,0)


//------------------------------------------------------------------------------
//	Event management
//------------------------------------------------------------------------------

extern void	EMStartup();
extern int	EMSendEvent( const Event_t *ev );
extern int	EMSendEventInt( const uint32_t ev );
extern int	EMSendEventNP( Event_e type, uint8_t id );
extern int	EMSendEvent1P( Event_e type, uint8_t id, uint8_t p1 );
extern int	EMSendEvent2P( Event_e type, uint8_t id, uint8_t p1, uint8_t p2 );
extern int	EMEventsPending();
extern int	EMGetNextEvent( Event_t *ev );
extern void	EMHandleEvents();


//------------------------------------------------------------------------------
//	System miscs
//------------------------------------------------------------------------------

typedef enum eSysRbStatus
{
	RB_UNUSED	= 0,
	RB_AVAIL,
	RB_FULL
}
SYS_RB_STATUS;

#define SYS_MAX_RINGBUFFERS		1	// Maximum number of ringbuffers

extern void	RBStartup( void );
extern int	RBAlloc( void *buf, size_t bs, size_t ps );
extern void RBReset( int rbid );
extern int	RBSendMessage( int rbid, const void *msg );
extern int	RBReadMessage( int rbid, void *msg );
extern int	RBHasMessages( int rbid );


typedef enum eSysTaskStatus
{
	TASK_UNUSED	= 0,
	TASK_ACTIVE
}
SYS_TASK_STATUS;

#define SYS_MAX_TASKS	10	// Maximum number of running tasks

extern void	TMStartup();
extern int	TMCreateTask( uint32_t delay,
						  void (*pftask)( uint32_t ),
						  uint32_t param,
						  uint8_t repeat,
						  uint32_t interval,
						  uint8_t *pid );
extern void	TMDestroyTask( uint8_t id );
extern void	TMUpdateTask( uint8_t id, uint32_t param, uint32_t delay );
extern void	TMExecTasks();


//------------------------------------------------------------------------------
//	User inputs
//------------------------------------------------------------------------------

extern void ReadUserInputs( void );


//------------------------------------------------------------------------------
//	Screen Management
//------------------------------------------------------------------------------

typedef enum eScreenId
{
	SCREEN_OFF = 0,
	SCREEN_MAIN
}
SCREENID;

extern void	SMStartup( void );
extern void	SMScreen( SCREENID s );
extern void	SMShowScreen( SCREENID s );
extern void	SMRefresh( void );
extern int	SMInputEvent( Event_t *ev );


//------------------------------------------------------------------------------
//	Battery Management
//------------------------------------------------------------------------------

extern void	BMStartup( void );
extern void BMUpdateBattery( void );
extern void BMReadBattery( void );
extern void BMGetCells( uint32_t *v1, uint32_t *v2 );


//------------------------------------------------------------------------------
//	Timings (in ticks) (1 tick = 1ms)
//------------------------------------------------------------------------------

#define TI_POLL_INPUT		2		// User inputs (keys) polling interval
#define TI_EVENT_HANDLING	5		// Max time spent in event handling
#define TI_USB_TEST			10		// Test USB plug
#define TI_KEY_REPEAT		50		// Key repeat period
#define TI_SEQUENCE_TIMEOUT	500		// Max interval between keys in a sequence
#define TI_UPDATE_BATTERY	10		// Battery sampling interval


//==============================================================================
// Miscs Externs & Prototypes
//------------------------------------------------------------------------------

extern volatile uint32_t TickCounter;
static inline uint32_t GetSysTick( void ) { return TickCounter; };
extern void WaitTicks( uint32_t ticks );
extern void WaitUs( uint32_t us );
extern void ResetChip( int );

extern uint32_t isqrt( uint32_t );


//==============================================================================
#endif /* __AEGIS_H__ */
