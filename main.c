/* Includes */
#include <stdio.h>      /* For printf() */
#include <windows.h>    /* For exit() */
#include "FastSkyCPU.h" /* For SkyCPU routines */

/**
 * Interrupts callback
 *
 * @param icode Interrupt code
 */
void interrupts_callback_fnct(uint32_t icode) {

	/* Process interrupt */
	switch (icode) {
	/* TODO */

	default:
		printf("INTERRUPT: %lu\n", icode);
		break;
	}
}

/**
 * Breakpoints callback
 *
 * @param bcode Breakpoint code
 */
void breakpoints_callback_fnct(uint32_t bcode) {

	/* Break & Clean exit */
	printf("\n\nBREAK: exit code %lu ...", bcode);
	exit(0);
}

/**
 * Demo program for SkyCPU
 */
uint8_t demo_program[] = {
	0x15, 0x80, 0x2A /* BRK.b #42 */
};

/**
 * Host program entry point
 */
int main(void) {

	/* Runtime initialization */
	SkyCPU_runtime_t runtime;
	SkyCPU_runtime_init(&runtime);

	/* Callbacks initialization */
	SkyCPU_callback_setup(&runtime, &interrupts_callback_fnct,
			&breakpoints_callback_fnct);

	/* Bootload demo program */
	SkyCPU_memory_copy(&runtime, demo_program, sizeof(demo_program), 0);

	/* Run CPU core (endless loop) */
	for (;;) {

		/* Fetch and execute */
		SkyCPU_fetch_and_execute(&runtime);
	}

	/* Return without error */
	return 0;
}
