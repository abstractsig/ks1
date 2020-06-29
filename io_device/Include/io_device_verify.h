/*
 *
 *
 */
#ifndef io_device_verify_H_
#define io_device_verify_H_
#include <io_verify.h>

void	run_ut_io_device (V_runner_t*);

#ifdef IMPLEMENT_VERIFY_IO_DEVICE
#include <io_device.h>
#include <io_verify.h>

TEST_BEGIN(test_io_device_event_loop_1) {

	io_cpu_clock_get_current_frequency (io_get_core_clock(TEST_IO));

}
TEST_END

TEST_BEGIN(test_stm32f4_random1) {
	uint32_t rand[3];

	rand[0] = io_get_random_u32(TEST_IO);
	rand[1] = io_get_random_u32(TEST_IO);
	rand[2] = io_get_random_u32(TEST_IO);

	if (rand[0] == rand[1])	rand[1] = io_get_random_u32(TEST_IO);
	if (rand[0] == rand[1]) rand[1] = io_get_random_u32(TEST_IO);
	if (rand[0] == rand[1]) rand[1] = io_get_random_u32(TEST_IO);

	if (rand[1] == rand[2]) rand[2] = io_get_random_u32(TEST_IO);
	if (rand[1] == rand[2]) rand[2] = io_get_random_u32(TEST_IO);
	if (rand[1] == rand[2]) rand[2] = io_get_random_u32(TEST_IO);

	VERIFY(rand[0] != rand[1],NULL);
	VERIFY(rand[1] != rand[2],NULL);

	rand[0] = io_get_next_prbs_u32(TEST_IO);
	rand[1] = io_get_next_prbs_u32(TEST_IO);
	rand[2] = io_get_next_prbs_u32(TEST_IO);

	if (rand[0] == rand[1])	rand[1] = io_get_next_prbs_u32(TEST_IO);
	if (rand[0] == rand[1]) rand[1] = io_get_next_prbs_u32(TEST_IO);
	if (rand[0] == rand[1]) rand[1] = io_get_next_prbs_u32(TEST_IO);

	if (rand[1] == rand[2]) rand[2] = io_get_next_prbs_u32(TEST_IO);
	if (rand[1] == rand[2]) rand[2] = io_get_next_prbs_u32(TEST_IO);
	if (rand[1] == rand[2]) rand[2] = io_get_next_prbs_u32(TEST_IO);

	VERIFY(rand[0] != rand[1],NULL);
	VERIFY(rand[1] != rand[2],NULL);

}
TEST_END

UNIT_SETUP(setup_io_device_unit_test) {
	io_value_memory_get_info (io_get_short_term_value_memory (TEST_IO),TEST_MEMORY_INFO);
	io_byte_memory_get_info (io_get_byte_memory (TEST_IO),TEST_MEMORY_INFO + 1);
	return VERIFY_UNIT_CONTINUE;
}

UNIT_TEARDOWN(teardown_io_device_unit_test) {
	io_value_memory_t *vm = io_get_short_term_value_memory (TEST_IO);
	memory_info_t bm_end,vm_end;
	io_value_memory_do_gc (vm,-1);
	io_value_memory_get_info (vm,&vm_end);
	VERIFY (vm_end.used_bytes == TEST_MEMORY_INFO[0].used_bytes,NULL);
	io_byte_memory_get_info (io_get_byte_memory (TEST_IO),&bm_end);
	VERIFY (bm_end.used_bytes == TEST_MEMORY_INFO[1].used_bytes,NULL);
}

static void
io_device_unit_test (V_unit_test_t *unit) {
	static V_test_t const tests[] = {
		test_io_device_event_loop_1,
		test_stm32f4_random1,
		0
	};
	unit->name = "io device";
	unit->description = "io device unit test";
	unit->tests = tests;
	unit->setup = setup_io_device_unit_test;
	unit->teardown = teardown_io_device_unit_test;
}

void
run_ut_io_device (V_runner_t *runner) {
	static const unit_test_t test_set[] = {
		io_device_unit_test,
		0
	};
	V_run_unit_tests(runner,test_set);
}

#endif /* IMPLEMENT_VERIFY_IO_DEVICE */
#endif
/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2020 Gregor Bruce
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
