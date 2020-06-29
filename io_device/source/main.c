/*
 *
 * the Hub75 starts here
 *
 */
#define IMPLEMENT_IO_CORE
#define IMPLEMENT_IO_GRAPHICS
#define 	WITH_IO_GRAPHICS_FONT_ROBOTO_BOLD
#define IMPLEMENT_IO_SHELL
#define IMPLEMENT_STM32F4_IO_CPU
#define IMPLEMENT_IO_DEVICE
#define IMPLEMENT_IO_BOARD
#include <io_device.h>

int
main(void) {
	io_t *io = initialise_io_device ();

	if (io != NULL) {
		if (test_device (io,cr_NIL)) {

			while (1) {
				io_wait_for_event (io);
				io_do_gc (io,-1);
			}
		} else {
			io_printf(io,"\n"DEVICE_NAME" device test failed\n");
		}
	}

	return 0;
}

