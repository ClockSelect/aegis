//==============================================================================
// aegis/src/ST7735S.c
//------------------------------------------------------------------------------
// Display management
//==============================================================================
#include "aegis.h"


//------------------------------------------------------------------------------
// Initialization sequence
//------------------------------------------------------------------------------

struct c_or_d_byte
{
	uint8_t	c_or_d;
	uint8_t	byte;
};

static const struct c_or_d_byte init_sequence[] =
{
	{ 0, 0x21 },	// INVON
	{ 0, 0xB1 },	// FRMCTR1
	{ 1, 0x05 },
	{ 1, 0x3A },
	{ 1, 0x3A },
	{ 0, 0xB2 },	// FRMCTR2
	{ 1, 0x05 },
	{ 1, 0x3A },
	{ 1, 0x3A },
	{ 0, 0xB3 },	// FRMCTR3
	{ 1, 0x05 },
	{ 1, 0x3A },
	{ 1, 0x3A },
	{ 1, 0x05 },
	{ 1, 0x3A },
	{ 1, 0x3A },
	{ 0, 0xB4 },	// INVCTR
	{ 1, 0x03 },
	{ 0, 0xC0 },	// PWCTR1
	{ 1, 0x62 },
	{ 1, 0x02 },
	{ 1, 0x04 },
	{ 0, 0xC1 },	// PWCTR2
	{ 1, 0xC0 },
	{ 0, 0xC2 },	// PWCTR3
	{ 1, 0x0D },
	{ 1, 0x00 },
	{ 0, 0xC3 },	// PWCTR4
	{ 1, 0x8D },
	{ 1, 0x6A },
	{ 0, 0xC4 },	// PWCTR5
	{ 1, 0x8D },
	{ 1, 0xEE },
	{ 0, 0xC5 },	// VMCTR1
	{ 1, 0x0E },
	{ 0, 0xE0 },	// GMCTRP1
	{ 1, 0x10 },
	{ 1, 0x0E },
	{ 1, 0x02 },
	{ 1, 0x03 },
	{ 1, 0x0E },
	{ 1, 0x07 },
	{ 1, 0x02 },
	{ 1, 0x07 },
	{ 1, 0x0A },
	{ 1, 0x12 },
	{ 1, 0x27 },
	{ 1, 0x37 },
	{ 1, 0x00 },
	{ 1, 0x0D },
	{ 1, 0x0E },
	{ 1, 0x10 },
	{ 0, 0xE1 },	// GMCTRN1
	{ 1, 0x10 },
	{ 1, 0x0E },
	{ 1, 0x03 },
	{ 1, 0x03 },
	{ 1, 0x0F },
	{ 1, 0x06 },
	{ 1, 0x02 },
	{ 1, 0x08 },
	{ 1, 0x0A },
	{ 1, 0x13 },
	{ 1, 0x26 },
	{ 1, 0x36 },
	{ 1, 0x00 },
	{ 1, 0x0D },
	{ 1, 0x0E },
	{ 1, 0x10 },
	{ 0, 0x3A },	// COLMOD
	{ 1, 0x05 },	// 16-bit/pixel
	{ 0, 0x36 },	// MADCTL
	{ 1, 0xC0 }
};


static uint16_t drawing_color;
static const font_t *font;


static int send_bytes( uint8_t *bytes, uint32_t len, uint32_t timeout )
{
	int r = 0;
	uint32_t start_tick = GetSysTick();

	while ( len && !r )
	{
		while ( SPI_GET_TX_FIFO_FULL_FLAG( SPI0 ) )
		{
			if ( GetSysTick() - start_tick > timeout )
			{
				r = 3;
				break;
			}
		}
		SPI_WRITE_TX( SPI0, *bytes++ );
		--len;
	}
	while ( SPI_IS_BUSY( SPI0 ) )
		;
	return r;
}

static int send_cmd_byte( uint8_t cmd )
{
	int r;
	DISP_CSX = 0;
	DISP_DCX = 0;
	r = send_bytes( &cmd, 1, 5 );
	DISP_CSX = 1;
	return r;
}

static int send_data_byte( uint8_t data )
{
	int r;
	DISP_CSX = 0;
	DISP_DCX = 1;
	r = send_bytes( &data, 1, 5 );
	DISP_CSX = 1;
	return r;
}

static int send_data_word( uint16_t data )
{
	int r;
	uint16_t bew;

	bew = ( data << 8 ) | ( data >> 8 );

	DISP_CSX = 0;
	DISP_DCX = 1;
	r = send_bytes( (uint8_t*)&bew, 2, 5 );
	DISP_CSX = 1;

	return r;
}

static void address_rect( const rect_t *r )
{
	uint16_t left	= r->left   + 26;
	uint16_t right	= r->right  + 26;
	uint16_t top	= r->top    + 1;
	uint16_t bottom	= r->bottom + 1;

	send_cmd_byte( 0x2A );
	send_data_byte( left >> 8 );
	send_data_byte( left & 0xFF );
	send_data_byte( right >> 8 );
	send_data_byte( right & 0xFF );
	send_cmd_byte( 0x2B );
	send_data_byte( top >> 8 );
	send_data_byte( top & 0xFF );
	send_data_byte( bottom >> 8 );
	send_data_byte( bottom & 0xFF );
	send_cmd_byte( 0x2C );
}

static void fill_rect( rect_t *r )
{
	address_rect( r );

	int nbPix = ( r->right - r->left + 1 ) * ( r->bottom - r->top + 1 );
	while ( nbPix-- )
	{
		send_data_word( drawing_color );
	}
}

static void StartBacklightDriver( void )
{
	PWM_EnableOutput( PWM1, PWM_CH_2_MASK );
	PWM_Start( PWM1, PWM_CH_2_MASK );
}

static void StopBacklightDriver( void )
{
	PWM_SET_CMR( PWM1, 2, 0 );
	PWM_Stop( PWM1, PWM_CH_2_MASK );
}

static void Open( void )
{
	DISP_CSX = 1;
	DISP_DCX = 1;
	SPI_ENABLE( SPI0 );
	StartBacklightDriver();
	PD2 = 1;
	PB1 = 0;
}

static void Close( void )
{
	SPI_DISABLE( SPI0 );
	StopBacklightDriver();
	PD2 = 0;
}

static void Sleep( void )
{
	send_cmd_byte( 0x10 );
	SetDisplayStatus( DISPLAY_SLEEPING );
}

static void Wakeup( void )
{
	send_cmd_byte( 0x11 );

	if ( display_status == DISPLAY_SLEEPING )
		TMCreateTask( 120, SetDisplayStatus, DISPLAY_ON, 0, 0, 0 );
}

static void Reset( void )
{
	PC2 = 1;
	WaitUs( 10 );
	PC2 = 0;
	WaitUs( 10 );
	PC2 = 1;
}

static void CLS( void )
{
	drawing_color = bgcolor;

	rect_t r = { 0, 0, 79, 159 };

	fill_rect( &r );
}

static uint16_t color_rgb_to_565( uint32_t rgb )
{
	return	( rgb	>> 19 &0xF ) << 11
		|	( rgb >> 10 & 0x1F ) << 5
		|	( rgb >> 3 & 0xF )
		;
}

static void SetColor( uint32_t rgb )
{
	fgcolor = color_rgb_to_565( rgb );
}

static void SetBgColor( uint32_t rgb )
{
	bgcolor = color_rgb_to_565( rgb );
}

static void Init( void )
{
	int len = sizeof( init_sequence ) / sizeof( struct c_or_d_byte );
	int i;
	for ( i = 0 ; i < len ; ++i )
	{
		uint8_t byte = init_sequence[i].byte;
		switch ( init_sequence[i].c_or_d )
		{
			case 0:
				send_cmd_byte( byte );
				break;
			case 1:
				send_data_byte( byte );
				break;
		}
	}

	send_cmd_byte( 0x29 );

	WaitTicks( 1 );
}

static void SetBrightness( uint32_t b )
{
	PWM_SET_CMR( PWM1, 2, b );
}

static void SetFont( const font_t *f )
{
	font = f;
}

static int draw_char( const uint16_t c, int x, int y )
{
	const font_chunk_t *chunk = font->chunk;

	while ( chunk )
	{
		if (( c >= chunk->char_min ) && ( c <= chunk->char_max ))
			break;
		chunk = chunk->next;
	}
	
	if ( !chunk ) return 0;

	const font_chardesc_t *chardesc = &chunk->chars[ c - chunk->char_min ];
	int cw = chardesc->width;

	if ( !cw ) return 0;

	int ch = font->char_height;

	rect_t r = { x, y, x + cw - 1, y + ch - 1 };
	address_rect( &r );

	const uint8_t *bitmap = chardesc->bitmap;

	uint8_t bits;
	while ( ch-- )
	{
		for ( int i = 0 ; i < cw ; ++i )
		{
			if ( !( i & 0x7 ) ) bits = *bitmap++;
			send_data_word( bits >> 7 ? fgcolor : bgcolor );
			bits <<= 1;
		}
	}

	return cw;
}

static void Print( const char *str, const rect_t *r )
{
	if ( !font ) return;

	int x = r->left;
	int y = r->top;

	uint16_t c;

	while ( ( c = (uint16_t)*str++ ) )
	{
		x += draw_char( c, x, y );
	}
}

void rainbow( void )
{
	rect_t rect = { 0, 0, 79, 159 };

	address_rect( &rect );

	int r, g, b;

	for ( int l = 0 ; l < 160 ; ++l )
	{
		g = HDIV_Div( 64 * l, 160 );

		for ( int c = 0 ; c < 80 ; ++c )
		{
			r = HDIV_Div( 32 * c, 80 );
			b = HDIV_Div( 32 * ( c + l ), 240 ) ^ 0x1F;

			uint32_t c = b << 11 | g << 5 | r;
			send_data_word( c );
		}
	}
}


//------------------------------------------------------------------------------
// Startup Task
//------------------------------------------------------------------------------
// Does not prevent the box from operating while the screen is not up yet.
//------------------------------------------------------------------------------

static uint8_t startup_task_id;

void startup_task( uint32_t step )
{
	switch ( step )
	{
		case 0:
			Reset();
			TMUpdateTask( startup_task_id, 1, 1 );
			break;

		case 1:
			Open();
			TMUpdateTask( startup_task_id, 2, 120 );
			break;

		case 2:
			Wakeup();
			TMUpdateTask( startup_task_id, 3, 120 );
			break;

		case 3:
			Init();
			TMDestroyTask( startup_task_id );
			SetDisplayStatus( DISPLAY_ON );
			break;
	}
}

void Startup( void )
{
	if ( startup_task_id )
	{
		TMDestroyTask( startup_task_id );
	}

	SetDisplayStatus( DISPLAY_OFF );
	TMCreateTask( 0, startup_task, 0, 1, 120, &startup_task_id );
}


const display_t ST7735S =
{
	Startup,
	Sleep,
	Wakeup,
	SetColor,
	SetBgColor,
	CLS,
	SetBrightness,
	SetFont,
	Print
};

