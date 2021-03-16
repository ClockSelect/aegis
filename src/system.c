//==============================================================================
// aegis/src/system.c
//------------------------------------------------------------------------------
// Miscellaneous systems objects
//==============================================================================
#include "aegis.h"

#include <string.h>


//==============================================================================
// Ring Buffers
//------------------------------------------------------------------------------
// Ring buffers are the way different parts of the software communicate.
// The maximum number of available ringbuffers is defined in the aegis.h header.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Structure definition
//------------------------------------------------------------------------------
typedef struct ringbuffer
{
	void*			pBuffer;
	size_t			BufferLength;
	size_t			PacketSize;
	intptr_t		Head;
	intptr_t		Tail;
	uint32_t		NMsgs;
	SYS_RB_STATUS	eStatus;
}
ringbuffer_t;


//------------------------------------------------------------------------------
// Buffers
//------------------------------------------------------------------------------

static ringbuffer_t ringbuffers[SYS_MAX_RINGBUFFERS];


//------------------------------------------------------------------------------
// Startup
//------------------------------------------------------------------------------
// This method should be called at system startup.
//------------------------------------------------------------------------------
void RBStartup()
{
	for ( int rbid = 0 ; rbid < SYS_MAX_RINGBUFFERS ; ++rbid )
	{
		ringbuffers[rbid].eStatus = RB_UNUSED;
	}
}


//------------------------------------------------------------------------------
// Allocation
//------------------------------------------------------------------------------
// Allocate a ringbuffer for a specific purpose.
// A buffer must be provided, along with the buffer length and the individual
// message length. The buffer size needs not to be an exact multiple of the
// message length, but this would be a loss of space.
//------------------------------------------------------------------------------
int RBAlloc( void *buf, size_t bsize, size_t msize )
{
	int rbid;
	ringbuffer_t *rb = NULL;

	for ( rbid = 0 ; rbid < SYS_MAX_RINGBUFFERS ; ++rbid )
	{
		rb = &ringbuffers[rbid];
		if ( rb->eStatus == RB_UNUSED )
			break;
	}

	if ( rbid >= SYS_MAX_RINGBUFFERS )
		return -1;

	rb->pBuffer = buf;
	rb->BufferLength = bsize;
	rb->PacketSize = msize;
	RBReset( rbid );

	return rbid;
}


//------------------------------------------------------------------------------
// Reset
//------------------------------------------------------------------------------
// Clears a buffer.
//------------------------------------------------------------------------------
void RBReset( int rbid )
{
	ringbuffer_t *rb = &ringbuffers[rbid];

	rb->eStatus = RB_AVAIL;
	rb->Head = 0;
	rb->Tail = 0;
	rb->NMsgs = 0;
}


//------------------------------------------------------------------------------
// Polling
//------------------------------------------------------------------------------
// Returns a non-zero value if messages are pending.
//------------------------------------------------------------------------------
int RBHasMessages( int rbid )
{
	return ( ringbuffers[rbid].NMsgs );
}


//------------------------------------------------------------------------------
// Send a packet
//------------------------------------------------------------------------------
// Returns 0 for success, -1 if buffer full.
//------------------------------------------------------------------------------
int RBSendMessage( int rbid, const void *msg )
{
	ringbuffer_t *rb = &ringbuffers[rbid];

	if ( rb->eStatus == RB_FULL ) return -1;

	memcpy( rb->pBuffer + rb->Tail, msg, rb->PacketSize );
	rb->Tail += rb->PacketSize;

	if ( rb->Tail + rb->PacketSize > rb->BufferLength )
	{
		rb->Tail = 0;
	}

	++rb->NMsgs;

	if ( rb->Tail == rb->Head )
	{
		rb->eStatus = RB_FULL;
	}

	return 0;
}


//------------------------------------------------------------------------------
// Read a packet
//------------------------------------------------------------------------------
// Returns 0 for success, -1 if buffer empty.
//------------------------------------------------------------------------------
int RBReadMessage( int rbid, void *msg )
{
	ringbuffer_t *rb = &ringbuffers[rbid];

	if ( rb->NMsgs == 0 ) return -1;

	memcpy( msg, rb->pBuffer + rb->Head, rb->PacketSize );
	rb->Head += rb->PacketSize;

	if ( rb->Head + rb->PacketSize > rb->BufferLength )
	{
		rb->Head = 0;
	}

	--rb->NMsgs;
	rb->eStatus = RB_AVAIL;

	return rb->NMsgs;
}


//==============================================================================
// Task management
//------------------------------------------------------------------------------
typedef struct task
{
	uint8_t			*pid;
	void			(*pftask)( uint32_t );
	uint32_t		param;
	uint32_t		delay;
	uint32_t		interval;
	uint32_t		last;
	uint8_t			repeat;
	SYS_TASK_STATUS	status;
}
task_t;

static task_t tasks[SYS_MAX_TASKS];


//------------------------------------------------------------------------------
// Call at startup.
//------------------------------------------------------------------------------
void TMStartup()
{
	for ( int k = 0 ; k < SYS_MAX_TASKS ; ++k ) tasks[k].status = TASK_UNUSED;
}


//------------------------------------------------------------------------------
// Task creation.
//------------------------------------------------------------------------------
int TMCreateTask( uint32_t delay,
				  void (*pftask)( uint32_t ),
				  uint32_t param,
				  uint8_t repeat,
				  uint32_t interval,
				  uint8_t *pid )
{
	int k;

	for ( k = 0 ; k < SYS_MAX_TASKS ; ++k )
		if ( tasks[k].status == TASK_UNUSED ) break;

	if ( k == SYS_MAX_TASKS )
	{
		if ( pid ) *pid = 0;
		return 0;
	}

	task_t *task = &tasks[k];

	task->pid = pid;
	if ( pid ) *pid = ++k;

	task->pftask = pftask;
	task->param = param;
	task->delay = delay;
	task->repeat = repeat;
	task->interval = interval;

	task->last = GetSysTick();
	task->status = TASK_ACTIVE;

	return k;
}


void TMDestroyTask( uint8_t id )
{
	if ( !id || id > SYS_MAX_TASKS ) return;
	task_t *task = &tasks[--id];

	if ( task->status == TASK_UNUSED ) return;

	task->status = TASK_UNUSED;
	if ( task->pid ) *task->pid = 0;
}


void TMUpdateTask( uint8_t id, uint32_t param, uint32_t delay )
{
	if ( !id || id > SYS_MAX_TASKS ) return;
	task_t *task = &tasks[--id];

	task->param = param;
	task->delay = delay;

	if ( task->repeat )
	{
		task->interval = delay;
	}
}


void TMExecTasks()
{
	for ( int k = 0 ; k < SYS_MAX_TASKS ; ++k )
	{
		if ( tasks[k].status != TASK_ACTIVE ) continue;

		uint32_t t = GetSysTick();

		if ( t - tasks[k].last >= tasks[k].delay )
		{
			tasks[k].pftask( tasks[k].param );
			tasks[k].last = t;

			if ( tasks[k].repeat )
			{
				tasks[k].delay = tasks[k].interval;
			}
			else
			{
				tasks[k].status = TASK_UNUSED;
				if ( tasks[k].pid ) *tasks[k].pid = 0;
			}
		}
	}
}
