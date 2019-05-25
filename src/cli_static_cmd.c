/*
 * cli_static_cmd.c
 */

// main include
#include "cli_static.h"

// function prototypes
static void cli_reboot_command(char *command_str);

// command table
// structure: { "command", "help string", command_function, number_of_args }
cli_command_definition_t cli_command_table[] = {
		{ "reboot", "Reboot entire system\r\n", cli_reboot_command, 0 },
		{ NULL, NULL, NULL, 0 }
};

// reboot function for STM32
static void cli_reboot_command(char *command_str) {
	// send some feedback
	CLI_PUTS("Rebooting system in 1 second...\r\n");
	// wait a second
	HAL_Delay(1000);
	// reset system
	NVIC_SystemReset();
}
