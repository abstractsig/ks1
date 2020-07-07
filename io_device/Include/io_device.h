/*
 *
 * key store device configurator based on stm32f407 touch display board configuration
 *
 *                        .--------------------.
 *                        | 1 5V       NRST   1|
 *                        | 2 0V       3V3    2|
 *                        | 3 PA4      Audio  3|
 *                        | 4 PB0             4|
 *                        | 5 PB1             5| 
 *                        | 6 PC0             6| 
 *                        | 7 PC2       PD12  7| 
 *                        | 8 PC3       PD13  8| 
 *                        | 9 PA1       PD14  9|            
 *                        |10 PA2       PD15 10| 
 *                        |11 PB12      PE9  11| 
 *                        |12 PB13      PE10 12| 
 *                        |13 PB14      PC4  13| 
 *                        |14 PB15      PC5  14| 
 *                        |15 PD0       PB2  15| 
 *                        |16 PD1       PA8  16| 
 *          SPI1 SCK      |17 PA15      PB10 17| 
 *               MISO     |18 PA5       PB11 18| 
 *               MOSI     |19 PA6       PC7  19|  UART6 RX 
 *               SS       |20 PA7       PC6  20|  UART6 TX
 *               RST      |21 PC13      PA3  21|  UART2 RX
 *               IRQ      |22 PB3       PD5  22|  UART2 TX
 *                        |23 PB4       PB6  23| 
 *                        |24 PB5       PB7  24| 
 *                        |25 3V3       0V   25| 
 *                        |26 0V        PD7  26| 
 *                        `--------------------'
 *
 *
 */
#ifndef io_device_H_
#define io_device_H_
#include <io_board.h>
#include <io_graphics.h>
#include <stm32f4_spi.h>
#include <nfc/MFRC522.h>

#define DEFAULT_GAMMA_CORRECTION				1.8

	 
enum {
	USART2_SOCKET,
	SPI1_SOCKET_ID,
	SPI2_SOCKET_ID,
	MFRC522_SOCKET,
	
	NUMBER_OF_IO_SOCKETS
};

#define IO_LOG_SOCKET		USART2_SOCKET

#define JS_CONFIG_WITH_BIGNUM
#include <jsimport/std/module.h>
#include <jsimport/io/namespace.h>

typedef struct PACK_STRUCTURE device_io {
	STM32F4_IO_CPU_STRUCT_MEMBERS;

	io_socket_t* sockets[NUMBER_OF_IO_SOCKETS];

} device_io_t;

io_t*	initialise_io_device (void);
bool test_device (io_t*,vref_t);


#ifdef IMPLEMENT_IO_DEVICE

#define UMM_VALUE_MEMORY_HEAP_SIZE		0x1000
#define UMM_GLOBAL_HEAP_SIZE				(KB(128))
#define UMM_SECTION_DESCRIPTOR      	__attribute__ ((section(".umm")))

static uint8_t ALLOCATE_ALIGN(8) UMM_SECTION_DESCRIPTOR
heap_byte_memory_bytes[UMM_GLOBAL_HEAP_SIZE];
static io_byte_memory_t
heap_byte_memory = {
	.heap = (umm_block_t*) heap_byte_memory_bytes,
	.number_of_blocks = (UMM_GLOBAL_HEAP_SIZE / sizeof(umm_block_t)),
};

static io_value_memory_t* value_memories[1] = {0};
io_value_memory_t*
io_get_value_memory_by_id (uint32_t id) {
	if (id < SIZEOF(value_memories)) {
		return value_memories[id];
	} else {
		return NULL;
	}
}

bool
register_io_value_memory (io_value_memory_t *vm) {
	if (value_memories[io_value_memory_id(vm)] == NULL) {
		value_memories[io_value_memory_id(vm)] = vm;
		return true;
	} else {
		return false;
	}
}

/*
 *-----------------------------------------------------------------------------
 *
 * ks1 CPU Clock Tree
 *
 *    +----------+        
 *    | LSE/1    |-+------>                                    // 32kHz
 *    +----------+       
 *
 *    +----------+         +---------+          +--------+
 *    | HSI/1    |-+------>| PLL*21/2|-+------->| CPU    |		// 168MHz
 *    +----------+ |       +---------+ |        +--------+
 *                                     |
 *  .----------------------------------'
 *  |
 *  | +----------+         +---------+  42MHz   +--------+
 *  `-| AHB/1    |-+------>| APB1/4  |--------->| TIM3   |		
 *    +----------+ |       +---------+          +--------+
 *                 |
 *                 |       +---------+  84MHz   +---------+
 *                 +------>| APB2/2  |--------->| UART2   |		// console uart
 *                 |       +---------+   |      +---------+
 *                 |                     |
 *                 |                     |      +---------+
 *                 |                     |----->| SPI1    |		//
 *                 |                     |      +---------+
 *
 *-----------------------------------------------------------------------------
 */
extern EVENT_DATA stm32f4_core_clock_t cpu_core_clock;
static EVENT_DATA stm32f4_pll_t pll;
static EVENT_DATA stm32f4_peripheral_clock_t dma2_clock;

static EVENT_DATA stm32f4_rc_oscillator_t hsi_osc = {
	.implementation = &stm32f4_hsi_oscillator_implementation,
	.frequency = 16000000.0,
};

static EVENT_DATA stm32f4_apb_clock_t ahb_clock = {
	.implementation = &stm32f4_ahb_clock_implementation,
	.input = decl_io_cpu_clock_pointer(&pll),
	.divider = 1,
	.outputs = (const io_cpu_clock_pointer_t []) {
		{NULL}
	},
};

static EVENT_DATA stm32f4_apb_clock_t apb1_clock = {
	.implementation = &stm32f4_apb1_clock_implementation,
	.input = decl_io_cpu_clock_pointer(&ahb_clock),
	.divider = 4,
	.outputs = (const io_cpu_clock_pointer_t []) {
		decl_io_cpu_clock_pointer(&dma2_clock),
		{NULL}
	},
};

static EVENT_DATA stm32f4_apb_clock_t apb2_clock = {
	.implementation = &stm32f4_apb2_clock_implementation,
	.input = decl_io_cpu_clock_pointer(&ahb_clock),
	.divider = 2,
	.outputs = (const io_cpu_clock_pointer_t []) {
		{NULL}
	},
};

static EVENT_DATA stm32f4_pll_t pll = {
	.implementation = &stm32f4_pll_implementation,
	.input = IO_CPU_CLOCK(&hsi_osc),
	.N = 336,.M = 16,.P = 2,.Q = 7,
	.outputs = (const io_cpu_clock_pointer_t []) {
		decl_io_cpu_clock_pointer(&cpu_core_clock),
		decl_io_cpu_clock_pointer(&apb1_clock),
		decl_io_cpu_clock_pointer(&apb2_clock),
		{NULL}
	},
};

EVENT_DATA stm32f4_core_clock_t cpu_core_clock = {
	.implementation = &stm32f4_core_clock_implementation,
	.input = IO_CPU_CLOCK(&pll),
};

static EVENT_DATA stm32f4_peripheral_clock_t dma2_clock = {
	.implementation = &stm32f4_apb_peripheral_clock_implementation,
	.input = decl_io_cpu_clock_pointer(&ahb_clock),
	.enable_apb_clock = stm32f4_enable_dma2_ahb_clock,
};

stm32f4_io_dma_controller_t dma2_controller = {
	.implementation = &stm32f4_io_dma_controller_implementation,
	.peripheral_bus_clock = decl_io_cpu_clock_pointer(&dma2_clock),
	.cpu_registers = DMA2,
};


static EVENT_DATA stm32f4_peripheral_clock_t usart2_clock = {
	.implementation = &stm32f4_apb_peripheral_clock_implementation,
	.input = decl_io_cpu_clock_pointer(&apb1_clock),
	.enable_apb_clock = stm32f4_enable_uart2_apb_clock,
};

static io_socket_t*
usart1_socket (io_t *io,io_address_t address) {
	static stm32f4_uart_t usart1 = {
		.implementation = &stm32f4_uart_implementation,
		.io = NULL,
		.encoding = &io_text_encoding_implementation,
		.peripheral_bus_clock = decl_io_cpu_clock_pointer(&usart2_clock),
		.uart_registers = USART2,
		.interrupt_number = USART2_IRQn,
		.baud_rate = CONTROL_UART_SPEED,
		.tx_pin.stm = {
			.port_id = STM_GPIO_PORT_D,
			.number = GPIO_PinSource5,
			.mode = GPIO_Mode_AF,
			.alternate = GPIO_AF_USART2,
			.speed = GPIO_High_Speed,
			.output_type = GPIO_OType_PP,
			.pull = GPIO_PuPd_NOPULL,
		},
		.rx_pin.stm = {
			.port_id = STM_GPIO_PORT_A,
			.number = GPIO_PinSource3,
			.mode = GPIO_Mode_AF,
			.alternate = GPIO_AF_USART2,
			.speed = GPIO_High_Speed,
			.output_type = GPIO_OType_PP,
			.pull = GPIO_PuPd_NOPULL,
		},
	};
	
	return (io_socket_t*) &usart1;
}

EVENT_DATA io_settings_t usart1_settings = {
	.transmit_pipe_length = 32,
	.receive_pipe_length = 512,
};

static EVENT_DATA stm32f4_peripheral_clock_t spi1_clock = {
	.implementation = &stm32f4_apb_peripheral_clock_implementation,
	.input = decl_io_cpu_clock_pointer(&apb2_clock),
	.enable_apb_clock = stm32f4_enable_spi1_apb_clock,
};

static io_socket_t*
spi1_socket (io_t *io,io_address_t address) {
	static stm32f4_spi_socket_t spi = {
		.implementation = &stm32f4_dma_spi_socket_implementation,
		.peripheral_bus_clock = decl_io_cpu_clock_pointer(&spi1_clock),
		.spi_registers = SPI1,
		.maximum_sck_frequency = 10000000,
		.tx_dma_channel = {
			.controller = (io_dma_controller_t*) &dma2_controller,
			.stream_registers = DMA2_Stream3,
			.interrupt_number = DMA2_Stream3_IRQn,
			.channel_number = 3,
		},
		.rx_dma_channel = {
			.controller = (io_dma_controller_t*) &dma2_controller,
			.stream_registers = DMA2_Stream2,
			.interrupt_number = DMA2_Stream2_IRQn,
			.channel_number = 3,
		},
		.miso_pin.stm = {
			.port_id = STM_GPIO_PORT_A,
			.number = GPIO_PinSource5,
			.mode = GPIO_Mode_AF,
			.alternate = GPIO_AF_SPI1,
			.speed = GPIO_High_Speed,
			.pull = GPIO_PuPd_NOPULL,
		},
		.mosi_pin.stm = {
			.port_id = STM_GPIO_PORT_A,
			.number = GPIO_PinSource6,
			.mode = GPIO_Mode_AF,
			.alternate = GPIO_AF_SPI1,
			.speed = GPIO_High_Speed,
			.pull = GPIO_PuPd_NOPULL,
		},
		.sck_pin.stm = {
			.port_id = STM_GPIO_PORT_A,
			.number = GPIO_PinSource15,
			.mode = GPIO_Mode_AF,
			.alternate = GPIO_AF_SPI1,
			.speed = GPIO_High_Speed,
			.pull = GPIO_PuPd_NOPULL,
		},
		.ss_pin.stm = {
			.port_id = STM_GPIO_PORT_A,
			.number = GPIO_PinSource7,
			.active_level = IO_PIN_ACTIVE_LOW,
			.initial_state = IO_PIN_INACTIVE,
			.mode = GPIO_Mode_OUT,
			.speed = GPIO_High_Speed,
			.output_type = GPIO_OType_PP,
			.pull = GPIO_PuPd_NOPULL,
		},
	};
	return (io_socket_t*) &spi;
}

EVENT_DATA io_settings_t spi1_settings = {
	.encoding = &io_binary_encoding_implementation,
	.transmit_pipe_length = 32,
	.receive_pipe_length = 32,
};

static EVENT_DATA stm32f4_peripheral_clock_t spi2_clock = {
	.implementation = &stm32f4_apb_peripheral_clock_implementation,
	.input = decl_io_cpu_clock_pointer(&apb2_clock),
	.enable_apb_clock = stm32f4_enable_spi2_apb_clock,
};

static io_socket_t*
spi2_socket (io_t *io,io_address_t address) {
	static stm32f4_spi_socket_t spi = {
		.implementation = &stm32f4_dma_spi_socket_implementation,	// change to synchronous version
		.peripheral_bus_clock = decl_io_cpu_clock_pointer(&spi2_clock),
		.spi_registers = SPI2,
		.maximum_sck_frequency = 10000000,
		.miso_pin.stm = def_stm32f4_function_pin (
			STM_GPIO_PORT_B,
			GPIO_PinSource14,
			GPIO_AF_SPI2,
			GPIO_High_Speed
		),
		.mosi_pin.stm = def_stm32f4_function_pin (
			STM_GPIO_PORT_B,
			GPIO_PinSource15,
			GPIO_AF_SPI2,
			GPIO_High_Speed
		),
		.sck_pin.stm = def_stm32f4_function_pin (
			STM_GPIO_PORT_B,
			GPIO_PinSource13,
			GPIO_AF_SPI2,
			GPIO_High_Speed
		),
		.ss_pin.stm = def_stm32f4_output_pin (
			STM_GPIO_PORT_E,
			GPIO_PinSource8,
			IO_PIN_ACTIVE_LOW,
			IO_PIN_INACTIVE,
			GPIO_High_Speed
		), 
	};
	return (io_socket_t*) &spi;
};

EVENT_DATA io_settings_t spi2_settings = {
	.encoding = &io_binary_encoding_implementation,
	.transmit_pipe_length = 32,
	.receive_pipe_length = 32,
};


static io_socket_t*
mfrc522_socket (io_t *io,io_address_t address) {
	mfrc522_io_socket_t *socket = io_byte_memory_allocate (
		io_get_byte_memory (io),sizeof(mfrc522_io_socket_t)
	);

	stm32f4_io_pin_t reset = {.stm ={
		.port_id = STM_GPIO_PORT_C,
		.number = GPIO_PinSource13,
		.active_level = IO_PIN_ACTIVE_LOW,
		.initial_state = IO_PIN_ACTIVE,
		.mode = GPIO_Mode_OUT,
		.speed = GPIO_Medium_Speed,
		.output_type = GPIO_OType_PP,
		.pull = GPIO_PuPd_NOPULL,
	}};
	
	socket->implementation = &mfrc522_io_socket_implementation;
	socket->reset_pin = reset.io;

	return (io_socket_t*) socket;
}

//
//
//
static io_cpu_clock_pointer_t
io_device_get_core_clock (io_t *io) {
	return IO_CPU_CLOCK(&cpu_core_clock);
}

static io_socket_t*
io_device_get_socket (io_t *io,int32_t handle) {
	if (handle >= 0 && handle < NUMBER_OF_IO_SOCKETS) {
		device_io_t *this = (device_io_t*) io;
		return this->sockets[handle];
	} else {
		return NULL;
	}
}

void
device_log (io_t *io,char const *fmt,va_list va) {
	io_socket_t *print = io_get_socket (io,USART2_SOCKET);
	if (print) {
		io_encoding_t *msg = io_socket_new_message (print);
		if (msg) {
			io_encoding_print (msg,fmt,va);
			io_socket_send_message (print,msg);
		} else {
			io_panic (io,IO_PANIC_OUT_OF_MEMORY);
		}
	}
}

void
flush_device_log (io_t *io) {
	io_socket_t *print = io_get_socket (io,USART2_SOCKET);
	if (print) {
		io_socket_flush (print);
	}
}

#define SPECIALISE_IO_DEVICE_IMPLEMENTATION(S) \
    SPECIALISE_IO_BOARD_IMPLEMENTATION(S) \
    .get_core_clock = io_device_get_core_clock,\
	 .get_socket = io_device_get_socket,\
    /**/

static io_implementation_t io_i = {
	SPECIALISE_IO_DEVICE_IMPLEMENTATION(NULL)
	.log = device_log,
	.flush_log = flush_device_log,
};

device_io_t dev_io = {0};

const socket_builder_t my_sockets[] = {
	{USART2_SOCKET,		usart1_socket,io_invalid_address(),&usart1_settings,true,NULL},
	{MFRC522_SOCKET,		mfrc522_socket,io_invalid_address(),NULL,false,BINDINGS({MFRC522_SOCKET,SPI1_SOCKET_ID})},
	{SPI1_SOCKET_ID,		spi1_socket,io_invalid_address(),&spi1_settings,false,NULL},
	{SPI2_SOCKET_ID,		spi2_socket,io_invalid_address(),&spi2_settings,false,NULL},
};

io_t*
initialise_io_device (void) {
	io_t *io = (io_t*) &dev_io;

	initialise_io (io,&io_i);
	initialise_io_board (io);
	dma2_controller.io = io;

	dev_io.bm = initialise_io_byte_memory (io,&heap_byte_memory,UMM_BLOCK_SIZE_1N);
	dev_io.vm = mk_umm_io_value_memory (io,UMM_VALUE_MEMORY_HEAP_SIZE,STVM);
	register_io_value_memory (dev_io.vm);

	io_i.value_implementation_map = mk_string_hash_table (dev_io.bm,21);
	add_core_value_implementations_to_hash (io_i.value_implementation_map);
	dev_io.tasks = mk_io_value_pipe (dev_io.bm,3);

	memset (dev_io.sockets,0,sizeof(io_socket_t*) * NUMBER_OF_IO_SOCKETS);
	build_io_sockets (io,dev_io.sockets,my_sockets,SIZEOF(my_sockets));

	return io;
}

#endif /* IMPLEMENT_IO_DEVICE */
#endif /* io_device_H_ */
