/*
 *
 * ks1
 *
 */
#define IMPLEMENT_IO_CORE
#define IMPLEMENT_IO_GRAPHICS
#define 	WITH_IO_GRAPHICS_FONT_ROBOTO_BOLD
#define IMPLEMENT_IO_SHELL
#define IMPLEMENT_IO_CPU
#define IMPLEMENT_STM32F4_IO_CPU
#define IMPLEMENT_IO_DEVICE
#define IMPLEMENT_IO_BOARD
#include <io_device.h>


bool
build_js (io_t *io,JSRuntime **rt) {
	static const js_io_socket_def_t sockets[] = {
		{"console",IO_LOG_SOCKET,js_io_socket_constructor,""},
		END_OF_JS_IO_SOCKETS
	};
	
	static const io_js_device_configuration_t io_resources = {
		.filesystems = NULL,
		.pins = NULL,
		.sockets = sockets,
	};
	JSContext *ctx;

	*rt = JS_NewRuntime(io);
	ctx = JS_NewContextRaw(*rt);

	JS_SetGCThreshold (*rt,-1);
	
	JS_AddIntrinsicBaseObjects (ctx);
	JS_AddIntrinsicJSON (ctx);
	JS_AddIntrinsicRegExpCompiler (ctx);
	JS_AddIntrinsicRegExp (ctx);
	JS_AddIntrinsicPromise (ctx);
	JS_AddIntrinsicTypedArrays (ctx);
	JS_AddIntrinsicProxy (ctx);
	JS_AddIntrinsicEval (ctx);

	io_js_add_helpers (ctx);
	io_js_standard_module (ctx);
	io_js_io_namespace (ctx,&io_resources);

	return true;
}

int
main(void) {
	io_t *io = initialise_io_device ();
	if (io != NULL) {

		io->log_level = IO_INFO_LOG_LEVEL;

		if (test_device (io,cr_NIL)) {
			JSRuntime *rt;
			
			build_js (io,&rt);
			
			while (1) {
				io_js_do_tasks (rt);
				io_wait_for_event (io);
				io_do_gc (io,-1);
			}
			
		} else {
			io_printf(io,"\n"DEVICE_NAME" device test failed\n");
		}
	}

	return 0;
}

