//==============================================================================
// aegis/src/inputs.c
//------------------------------------------------------------------------------
// User input management
//==============================================================================
#include "aegis.h"

#include <string.h>


//------------------------------------------------------------------------------
/*	Structure defining what is a keypress event
 */

typedef struct key
{
	int			(*pf_test)( void );		// Key/combo pressed test function
	EV_K_e		key_event;				// Key event type for the key/combo
	uint8_t		nbuttons;				// Number of keys in the combo
	uint16_t	kh1_ticks;				// Min ticks before event hold 1
	uint16_t	kh2_ticks;				// Min ticks before event hold 2
}
key_t;


//------------------------------------------------------------------------------
/*	Structure defining a sequence of keypresses
 */

#define MAX_SEQ_KEYS	6

typedef struct sequence
{
	EV_K_e		key_event;				// Sequence event
	uint8_t		reset;					// Sequence cannot be prefix of another sequence
	uint8_t		length;					// Sequence length
	EV_K_e		sequence[MAX_SEQ_KEYS];	// Sequence keys
}
sequence_t;


//------------------------------------------------------------------------------
/* Key state
 */

typedef struct keystate
{
	EV_K_e		last;					// Last key event
	EV_K_e		current;				// Current key event
	uint8_t		rstep;					// Repeat step
	uint8_t		nbuttons;				// # of buttons implied in the event
	uint32_t	last_tick;				// Repeat tick counter
	uint32_t	tick;					// Tick of event start
	uint32_t	seq_tick;				// Last sequence tick
	uint8_t		rcount;					// Repeat count
	uint8_t		seq_count;				// Current sequence length
	EV_K_e		seq[MAX_SEQ_KEYS];		// Current key sequence
}
keystate_t;


static keystate_t ks = { 0 };


//------------------------------------------------------------------------------
/*	Predefinition of test functions
 */

static int TestFMP();
static int TestFM();
static int TestFP();
static int TestMP();
static int TestF();
static int TestM();
static int TestP();


//------------------------------------------------------------------------------
/*	Available key input events
 *	Defined by priority order
 */

static const key_t keys[] = 
{
	{
		TestFMP,
		EV_K_FMP,
		3,
		50,
		1000
	},
	{
		TestFM,
		EV_K_FM,
		2,
		50,
		1000
	},
	{
		TestFP,
		EV_K_FP,
		2,
		50,
		1000
	},
	{
		TestMP,
		EV_K_MP,
		2,
		50,
		2000
	},
	{
		TestF,
		EV_K_FIRE,
		1,
		50,
		1000
	},
	{
		TestM,
		EV_K_MINUS,
		1,
		50,
		1000
	},
	{
		TestP,
		EV_K_PLUS,
		1,
		50,
		1000
	}
};

#define NUM_KEYS (sizeof(keys)/sizeof(key_t))


//------------------------------------------------------------------------------
// Key sequences

static const sequence_t	sequences[] =
{
	{
		EV_K_TEST,
		1,
		6,
		{ EV_K_MINUS, EV_K_MINUS, EV_K_MINUS, EV_K_PLUS, EV_K_PLUS, EV_K_PLUS }
	},
	{
		EV_K_ONOFF,
		1,
		5,
		{ EV_K_FIRE, EV_K_FIRE, EV_K_FIRE, EV_K_FIRE, EV_K_FIRE }
	}
};

#define NUM_SEQS (sizeof(sequences)/sizeof(sequence_t))


//------------------------------------------------------------------------------
// Inputs test functions
//------------------------------------------------------------------------------
static int TestFMP()
{
	int count = 6;
	while ( --count )
		if (( BTN_FIRE ) || ( BTN_MINUS ) || ( BTN_PLUS )) break;

	return ( count == 0 );
}

static int TestFM()
{
	int count = 6;
	while ( --count )
		if (( BTN_FIRE ) || ( BTN_MINUS )) break;

	return ( count == 0 );
}

static int TestFP()
{
	int count = 6;
	while ( --count )
		if (( BTN_FIRE ) || ( BTN_PLUS )) break;

	return ( count == 0 );
}

static int TestMP()
{
	int count = 6;
	while ( --count )
		if (( BTN_MINUS ) || ( BTN_PLUS )) break;

	return ( count == 0 );
}

static int TestF()
{
	int count = 6;
	while ( --count ) if ( BTN_FIRE ) break;
	return ( count == 0 );
}

static int TestM()
{
	int count = 6;
	while ( --count ) if ( BTN_MINUS ) break;
	return ( count == 0 );
}

static int TestP()
{
	int count = 6;
	while ( --count ) if ( BTN_PLUS ) break;
	return ( count == 0 );
}


//------------------------------------------------------------------------------
// Sequence detection
//------------------------------------------------------------------------------
static void DetectSequence()
{
	if ( ks.current == EV_K_NULL || ks.rstep != 0 )
		return;

	EV_K_e ke = ks.current;
	EV_K_e se = EV_K_NULL;

	if ( ks.tick - ks.seq_tick > TI_SEQUENCE_TIMEOUT )
	{
		ks.seq_count = 0;
	}

	if ( ks.seq_count == MAX_SEQ_KEYS )
	{
		memcpy( &ks.seq[0], &ks.seq[1], sizeof( ks.seq[0] ) * ( MAX_SEQ_KEYS - 1 ) );
		--ks.seq_count;
	}

	ks.seq[ks.seq_count++] = ke;
	ks.seq_tick = ks.tick;

	for ( int s = 0 ; s < NUM_SEQS ; ++s )
	{
		const sequence_t *seq = &sequences[s];

		if ( seq->length > ks.seq_count ) continue;

		int i = ks.seq_count - seq->length;
		int j;

		for ( j = 0 ; j < seq->length ; ++j )
		{
			if ( seq->sequence[j] != ks.seq[i+j] )
				break;
		}
		
		if ( j == sequences[s].length )
		{
			se = seq->key_event;
			if ( seq->reset ) ks.seq_count = 0;
			break;
		}
	}

	if ( se != EV_K_NULL )
	{
		EMSendEvent1P( EVENT_KEY, se, 1 );
	}
}


//------------------------------------------------------------------------------
// Single keys & Combos inputs detection
//------------------------------------------------------------------------------
static void ReadKeyInputs()
{
	uint8_t nb;
	int k;

	EV_K_e last = ks.last;

	ks.last = ks.current;
	EV_K_e ke = EV_K_NULL;

	for ( k = 0 ; k < NUM_KEYS ; ++k )
	{
		if ( keys[k].pf_test() )
		{
			ke = keys[k].key_event;
			break;
		}
	}

	uint32_t t = GetSysTick();

	ks.current = ke;

	if ( ks.current != ks.last )
	{
		ks.tick		= t;
		ks.rstep	= 0;
		ks.rcount	= 0;

		if ( ks.current != EV_K_NULL )
		{
			nb = keys[k].nbuttons;

			if ( ks.last != EV_K_NULL )
			{
				if ( ks.nbuttons < nb )
				{
					ks.nbuttons = nb;
					EMSendEvent1P( EVENT_KEY, ks.current, 1 );
				}
				else
				{
					ks.last = last;
				}
			}
			else
			{
				ks.nbuttons = nb;
				EMSendEvent1P( EVENT_KEY, ks.current, 1 );
			}
		}
		else
		{
			ks.nbuttons = 0;
			EMSendEvent1P( EVENT_KEY, last, 0 );
		}
	}
	else if ( ks.current != EV_K_NULL )
	{
		if ( ks.nbuttons > keys[k].nbuttons )
		{
			ks.last = last;
			return;
		}

		switch ( ks.rstep )
		{
			case 0:
				++ks.rstep;
				// nobreak;
			case 1:
				if ( t - ks.tick > keys[k].kh1_ticks )
				{
					ks.last_tick = t;
					++ks.rstep;
					EMSendEvent2P( EVENT_KEY, ks.last, ks.rstep, 0 );
				}
				break;

			case 2:
				if ( t - ks.last_tick > TI_KEY_REPEAT )
				{
					if ( t - ks.tick > keys[k].kh2_ticks )
						++ks.rstep;

					if ( ks.rcount < 255 ) ++ks.rcount;
					EMSendEvent2P( EVENT_KEY, ks.last, ks.rstep, ks.rcount );

					ks.last_tick = t;
				}
				break;

			case 3:
				if ( t - ks.last_tick > TI_KEY_REPEAT )
				{
					if ( ks.rcount < 255 ) ++ks.rcount;
					EMSendEvent2P( EVENT_KEY, ks.last, ks.rstep, ks.rcount );

					ks.last_tick = t;
				}
				break;
		}
	}
}


//------------------------------------------------------------------------------
// User inputs detection
//------------------------------------------------------------------------------
void ReadUserInputs()
{
	static uint32_t last = 0;

	uint32_t t = GetSysTick();
	if ( t - last > TI_POLL_INPUT )
	{
		last = t;
		ReadKeyInputs();
		DetectSequence();
	}
}


//------------------------------------------------------------------------------
// USB Plug in/out detection
//------------------------------------------------------------------------------
static USB_STATUS USB_status = USB_UNK;

void TestUSBPlug( void )
{
	USB_STATUS old = USB_status;
	static uint32_t last = 0;
	uint32_t t = GetSysTick();

	int count = 6;

	switch ( old )
	{
		case USB_UNK:
			while ( --count ) if ( !PE0 ) break;
			USB_status = count ? USB_UNPLUGGED : USB_PLUGGED;
			break;

		case USB_PLUGGED:
			while ( --count ) if ( PE0 ) break;
			if ( count ) last = t;
			else if ( t - last > 250 ) USB_status = USB_UNPLUGGED;
			break;

		case USB_UNPLUGGED:
			while ( --count ) if ( !PE0 ) break;
			if ( count ) last = t;
			else if ( t - last > 250 ) USB_status = USB_PLUGGED;
			break;
	}

	if ( USB_status != old )
	{
		last = t;
		EMSendEvent1P( EVENT_HARDWARE, EV_H_USB_PLUG, USB_status );
	}
}

int IsUSBPlugged( void )
{
	return ( USB_status == USB_PLUGGED );
}
