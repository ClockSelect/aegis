#ifndef __AEGIS_H__
#define __AEGIS_H__
//==============================================================================
// aegis/inc/aegis.h
//------------------------------------------------------------------------------
// Constants, Type definitions and Function prototypes
// for the Aegis Legend firmware.
//==============================================================================

#define FINLINE __attribute__((always_inline))

#include <NUC1261.h>
#include <stddef.h>


//------------------------------------------------------------------------------
// Battery Voltage Offsets
// Correction values from ADC readings (mV)
//------------------------------------------------------------------------------
// Those values may vary from box to box. One may test the voltage values
// delivered by the ADC with accurate voltmeter measures to find out the
// exact values needed for his specific box.
//------------------------------------------------------------------------------
#define BVO_CELL1	(+70)	//	(+40)
#define BVO_CELL2	(+45)	//	(-65)
#define BVO_USB		(+200)
#define BVO_PF4		(+400)


//------------------------------------------------------------------------------
// Input buttons GPIO ports
//------------------------------------------------------------------------------
#define BTN_FIRE	PE10
#define	BTN_MINUS	PE13
#define BTN_PLUS	PD0


//------------------------------------------------------------------------------
// Tick counter
//------------------------------------------------------------------------------
extern volatile uint32_t TickCounter;
static FINLINE uint32_t GetSysTick( void ) { return TickCounter; };


//==============================================================================
//	Event Management
//------------------------------------------------------------------------------
//	Events constants
//------------------------------------------------------------------------------

typedef enum eEvent
{
	EVENT_NULL = 0,			// Null event
	EVENT_KEY,				// Key input event
	EVENT_DISPLAY,			// Display event
	EVENT_HARDWARE,			// Hardware event (battery, ato, usb...)
	EVENT_BOX,				// General box event (lock, sleep...)
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
	EV_D_BRIGHTNESS,		// Set screen brightness
	EV_D_DIMMER				// Dimm screen
}
EV_D_e;

typedef enum eEV_H
{
	EV_H_NULL = 0,
	EV_H_BATT_STATUS,		// Battery status change
	EV_H_USB_PLUG			// USB Plugged / Unplugged
}
EV_H_e;

typedef enum eEV_B
{
	EV_B_NULL = 0,
	EV_B_STATE_CHANGE,		// Change box state
	EV_B_SLEEP				// Go to sleep
}
EV_B_e;

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
		EV_H_e	h;
		EV_B_e	b;
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


//==============================================================================
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


//==============================================================================
//	User inputs
//------------------------------------------------------------------------------

extern void ReadUserInputs( void );


//==============================================================================
//	Screen Management
//------------------------------------------------------------------------------

typedef enum eScreenId
{
	SCREEN_NONE = -1,
	SCREEN_OFF,
	SCREEN_MAIN,
	SCREEN_IDLE
}
SCREENID;

extern void	SMStartup( void );
extern void	SMScreen( SCREENID s );
extern void	SMShowScreen( SCREENID s );
extern void	SMRefresh( void );
extern int	SMInputEvent( Event_t *ev );


//==============================================================================
//	Battery Management
//------------------------------------------------------------------------------

typedef enum eBattStatus
{
	BATT_OK = 0,
	BATT_LOW,
	BATT_UNK
}
BATT_STATUS;

extern void	BMStartup( void );
extern void BMUpdateBattery( void );
extern void BMReadBattery( void );
extern void BMUpdateStatus( void );
extern BATT_STATUS BMGetStatus( void );


//==============================================================================
//	USB
//------------------------------------------------------------------------------

typedef enum eUSBStatus
{
	USB_UNK = 0,
	USB_PLUGGED,
	USB_UNPLUGGED
}
USB_STATUS;

extern void	TestUSBPlug( void );
extern int	IsUSBPlugged( void );


//==============================================================================
//	Box Management
//------------------------------------------------------------------------------

#define BOX_ACT_Pos	(0)
#define BOX_ACT_Msk	(0xF<<BOX_ACT_Pos)
#define BOX_LCK_Pos (4)
#define BOX_LCK_Msk (0xF<<BOX_LCK_Pos)

#define BOX_STARTUP	(0x0<<BOX_ACT_Pos)
#define BOX_ON		(0x1<<BOX_ACT_Pos)
#define BOX_OFF		(0x2<<BOX_ACT_Pos)
#define BOX_LOWBAT	(0x3<<BOX_ACT_Pos)

#define BOX_ACTIVE	(0x0<<BOX_LCK_Pos)
#define BOX_IDLE	(0x1<<BOX_LCK_Pos)
#define BOX_LOCKED	(0x2<<BOX_LCK_Pos)

typedef enum eVapeMode
{
	VAPE_MODE_POWER	= 0,
	VAPE_MODE_MAX
}
VAPE_MODE;

typedef uint8_t BOX_STATE;

extern void	BXStartup( void );
extern void	BXCheckActivity( void );
extern void	BXEvent( Event_t *ev );

extern BOX_STATE bx_box_state;
static FINLINE BOX_STATE BXGetState() { return bx_box_state; }

extern uint32_t bx_last_activity;
static FINLINE void	BXUserActivity() { bx_last_activity = GetSysTick(); }
static FINLINE uint32_t	BXLastActivity() { return bx_last_activity; }


//==============================================================================
//	Values of interest
//------------------------------------------------------------------------------

//	Enumeration of all values of interest.
//	Any value listed above VOI_ENDCONFIG are supposed to be saved in the
//	dataflash configuration.
//	When adding or removing a value in this enum, the voi_sizes array in
//	the voi.c file must also be updated.
typedef enum voi_id_e
{
	VOI_BRIGHTNESS,
	VOI_ENDCONFIG,
	VOI_VCELL1,
	VOI_VCELL2,
	VOI_BATT_STATUS,
	VOI_MAX
}
VOI_ID;

typedef enum voi_user_e
{
	VOI_USCREEN,
	VOI_UCONFIG
}
VOI_USER;

extern uint8_t	VOISet( VOI_ID id, const void *p );
extern void		VOIGet( VOI_ID id, void *p );
extern int		VOIGetChanged( VOI_ID id, void *p, VOI_USER who );


//==============================================================================
//	Timings (in ticks) (1 tick = 1ms)
//------------------------------------------------------------------------------

#define TI_POLL_INPUT		2		// User inputs (keys) polling interval
#define TI_EVENT_HANDLING	5		// Max time spent in event handling
#define TI_USB_TEST			10		// Test USB plug
#define TI_KEY_REPEAT		50		// Key repeat period
#define TI_SEQUENCE_TIMEOUT	500		// Max interval between keys in a sequence
#define TI_UPDATE_BATTERY	10		// Battery sampling interval
#define TI_DIMMING			8000	// Time before screen dimming
#define TI_IDLE				15000	// Time before entering idle mode
#define TI_SLEEP			180000	// Time before entering sleep mode
#define TI_BOX_ACTCHECK		5		// Check box activity interval


//==============================================================================
// Miscs Externs & Prototypes
//------------------------------------------------------------------------------

extern void WaitTicks( uint32_t ticks );
extern void WaitUs( uint32_t us );
extern void ResetChip( int );

extern uint32_t isqrt( uint32_t );


//==============================================================================
// Other headers
//------------------------------------------------------------------------------

#include "printf.h"
#include "devices.h"
#include "font.h"
#include "display.h"


//==============================================================================
#endif /* __AEGIS_H__ */
