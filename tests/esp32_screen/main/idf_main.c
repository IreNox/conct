#include <nvs_flash.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <freertos/task.h>
//#include <rom/ets_sys.h>
#include <esp_log.h>

#include <string.h>

#define TX_GPIO 16
#define RX_GPIO 3
#define PIXEL_COUNT 256

#define USE_SPI 1
#define SPI_CS GPIO_NUM_1
#define SPI_MOSI GPIO_NUM_7
#define SPI_MISO GPIO_NUM_2
#define SPI_CLK GPIO_NUM_6

#define TEST_MIN( a, b ) ((a) < (b) ? (a) : (b))

uint8_t hex_to_nibble( char c )
{
	if( c >= '0' && c <= '9' )
	{
		return c - '0';
	}
	else if( c >= 'a' && c <= 'f' )
	{
		return (c - 'a') + 10u;
	}
	else if( c >= 'A' && c <= 'F' )
	{
		return (c - 'A') + 10u;
	}

	return 0;
}

char nibble_to_hex( uint8_t b )
{
	if( b <= 9 )
	{
		return '0' + b;
	}

	return 'a' + (b - 10u);
}

size_t hex_to_bytes2( uint8_t* target, const char* source )
{
	uint8_t* pTarget = target;
	const char* pHex = source;
	while( *pHex != '\0' )
	{
		while (*pHex == ' ') pHex++;
		if( *pHex == '\0' ) break;

		*pTarget = hex_to_nibble( *pHex ) << 4u;
		pHex++;

		while (*pHex == ' ') pHex++;
		if( *pHex == '\0' ) break;

		*pTarget |= hex_to_nibble( *pHex );
		pHex++;

		pTarget++;
	}
	return pTarget - target;
}

size_t hex_to_bytes( char* data )
{
	return hex_to_bytes2( (uint8_t*)data, data );
}

void bytes_to_hex( char* target, const uint8_t* bytes, size_t length )
{
	for( size_t i = 0u; i < length; ++i )
	{
		const uint8_t b = bytes[ i ];

		*target = nibble_to_hex( b >> 4u );
		target++;

		*target = nibble_to_hex( b & 0xf );
		target++;
	}
	*target = '\0';
}

#define COMET_CONSOLE_ARGUMENT_MAX 16

void app_main()
{
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	  ESP_ERROR_CHECK(nvs_flash_erase());
	  ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	gpio_set_direction( SPI_CS, GPIO_MODE_OUTPUT );
	gpio_set_level( SPI_CS, 1 );

	spi_device_handle_t spiDevice = NULL;

	spi_bus_config_t busConfig =
	{
		.flags				= SPICOMMON_BUSFLAG_MASTER,
		.mosi_io_num		= SPI_MOSI,
		.miso_io_num		= SPI_MISO,
		.sclk_io_num		= SPI_CLK,
		.quadwp_io_num		= -1,
		.quadhd_io_num		= -1,
		.data4_io_num		= -1,
		.data5_io_num		= -1,
		.data6_io_num		= -1,
		.data7_io_num		= -1,
		.max_transfer_sz	= 256u
	};

	spi_device_interface_config_t deviceConfig =
	{
		.mode				= 0,
		.queue_size			= 3u,
		.clock_speed_hz	 	= 1000000,
		//.spics_io_num		= 5,
		.cs_ena_pretrans	= 0,
		.cs_ena_posttrans	= 0
	};
	//SPI_DEVICE_HALFDUPLEX
	
	ESP_ERROR_CHECK( spi_bus_initialize( SPI2_HOST, &busConfig, SPI_DMA_CH_AUTO ) );
	ESP_ERROR_CHECK( spi_bus_add_device( SPI2_HOST, &deviceConfig, &spiDevice ) );

	uint8_t* readBuffer = malloc( 256u );
	uint8_t* writeBuffer = malloc( 256u );
	char* textBuffer = malloc( 513u );

	uint32_t time = 0u;
	size_t length = 0u;
	spi_transaction_t transactions[ 10u ];
	size_t transactionCount = 0u;
	while( true )
	{
		const char c = fgetc(stdin);
		const bool isRedo = (c == 0x08);
		const bool isAlphaNum = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
		const bool isEnd = (c == '\r' || c == '\n');

		if( isAlphaNum )
		{
			textBuffer[ length ] = c;
			length++;
			textBuffer[ length ] = '\0';

			printf( "> %s\n", textBuffer );
		}
		else if( isRedo && length == 0u )
		{
			length = strlen( textBuffer );

			printf( "> %s\n", textBuffer );
		}

		if( isEnd )
		{
			printf( "Command: %s\n", textBuffer );

			char* argv[COMET_CONSOLE_ARGUMENT_MAX];
    		int argc = 0;

			// Split up parameters
			bool quote = false;
			int arg_len = 0; // Just used for quoting
			char *s = (char*)textBuffer; // console_args.buffer;
			while (s[0] != '\0') {
				char ch = s[0];
				if ((!quote) && (ch <= ' ')) {  // White-space
					arg_len = 0;
					*s = 0;
					s++;
					continue;
				}
				if (ch == '"') {                // Quotes
					quote = !quote;
					*s = 0;
					s++;
					continue;
				}
				if (arg_len == 0) {             // Beginning of a new argument
					argv[argc] = s;
					argc++;
					if (argc == COMET_CONSOLE_ARGUMENT_MAX) break;
				}
				arg_len++;
				s++;
			}

			int readResult = 0u;
			bool parseTp = false;
			if( argc != 0 )
			{
				if( argc == 2u && strcmp( argv[ 0u ], "tp" ) == 0u )
				{
					for( uint8_t i = 0u ; i < 9u; ++i )
					{
						memset( &transactions[ i ], 0u, sizeof( transactions[ i ] ) );
					}

					size_t t = 0u;
					transactions[ t ].flags			= SPI_TRANS_USE_TXDATA;
					transactions[ t ].tx_data[ 0u ]	= 0xb1;
					transactions[ t ].length		= 16u;
					t++;

					transactions[ t ].flags			= SPI_TRANS_USE_TXDATA;
					transactions[ t ].tx_data[ 0u ]	= 0xc1;
					transactions[ t ].length		= 16u;
					transactions[ t ].rx_buffer		= readBuffer;
					transactions[ t ].rxlength		= 16u;
					t++;

					transactions[ t ].flags			= SPI_TRANS_USE_TXDATA;
					transactions[ t ].tx_data[ 0u ]	= 0x91;
					transactions[ t ].length		= 16u;
					transactions[ t ].rx_buffer		= readBuffer + 2u;
					transactions[ t ].rxlength		= 16u;
					t++;

					transactionCount = t;
					readResult = 18u;
					parseTp = true;
				}
				else
				{
					length = hex_to_bytes2( (uint8_t*)writeBuffer, textBuffer );

					spi_transaction_t* transaction = &transactions[ 0u ];
					memset( transaction, 0, sizeof( *transaction ) );
					transaction->tx_buffer	= writeBuffer;
					transaction->length		= length * 8u;
					transaction->rx_buffer	= readBuffer;
					transaction->rxlength	= length * 8u;

					transactionCount = 1u;
					readResult = length;
				}
			}

			if( length > 0)
			{
				bytes_to_hex( textBuffer, writeBuffer, TEST_MIN( length, 32 ) );
				printf( "Send: %s\n", textBuffer );

				memset( readBuffer, 0, 256u );

				//spi_transaction_t transaction = {};
				//transaction.tx_buffer	= writeBuffer;
				//transaction.length	= length * 8u;
				//transaction.rx_buffer	= readBuffer;
				//transaction.rxlength	= length * 8u;

				gpio_set_level( SPI_CS, 0 );
				for( uint8_t i = 0u ; i < transactionCount; ++i )
				{
					ESP_ERROR_CHECK( spi_device_transmit( spiDevice, &transactions[ i ] ) );
				}
				gpio_set_level( SPI_CS, 1 );
				
				ESP_LOGI( "test", "Write %d bytes", length );		

				bytes_to_hex( textBuffer, readBuffer, TEST_MIN( readResult, 32 ) );
				printf( "Received: %s\n", textBuffer );

				length = 0u;
				//ets_delay_us( 3000 );
			}
		}
	
		vTaskDelay( configTICK_RATE_HZ / 100 ); // 10ms
		time++;
	}
}