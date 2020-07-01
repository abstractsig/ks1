/*
 *
 * verify hub75 functionality
 *
 */
#define IMPLEMENT_IO_VERIFY
#define IMPLEMENT_VERIFY_IO_CORE_CONTAINERS
#define IMPLEMENT_VERIFY_IO_CORE_VALUES
#define IMPLEMENT_VERIFY_IO_CORE_GRAPHICS
#define IMPLEMENT_VERIFY_IO_CORE_GRAPHICS_FONT_ROBOTO
#define IMPLEMENT_VERIFY_IO_CORE_SOCKETS
#define IMPLEMENT_VERIFY_IO_CORE_TWI_LAYER
#define IMPLEMENT_VERIFY_QUICKJS
#define IMPLEMENT_VERIFY_IO_CPU
#define IMPLEMENT_VERIFY_IO_DEVICE
#include <io_verify_core_math.h>
#include <io_verify_core_containers.h>
#include <io_verify_core_security.h>
#include <io_verify_core_sockets.h>
#include <io_verify_core_values.h>
#include <io_verify_core_graphics.h>
#include <io_device_verify.h>
#include <io_cpu_verify.h>
#include <io_js_verify.h>

/*
 *-----------------------------------------------------------------------------
 *
 * test_device --
 *
 *-----------------------------------------------------------------------------
 */
bool
test_device (io_t *io,vref_t r_shell) {
	if (io_is_first_run(io)) {
		V_runner_t runner = {
			.user_value = r_shell,
			.io = io,
		};
		V_start_tests(&runner);

		run_ut_io_device (&runner);
		run_ut_io_cpu (&runner);
		run_ut_io_core_containers (&runner);
		run_ut_io_core_values (&runner);
		run_ut_io_core_graphics (&runner);
		run_ut_io_core_sockets (&runner);
		run_ut_quickjs (&runner);

		print_unit_test_report (&runner);
		
		if (runner.total_failed == 0) {
			io_clear_first_run (io);
			return true;
		} else {
			return false;
		}
	} else {
		return true;
	}
}


