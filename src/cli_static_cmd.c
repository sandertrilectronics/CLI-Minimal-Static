/*
 * cli_static_cmd.c
 *
 * Created on: 	May 22, 2019
 * Author: 		Sande
 * Project:		OXI010_SLT_Controller
 * 
 */

// main include
#include "cli_static.h"

// function prototypes
static void cli_reboot_command(char *command_str);

// command table
cli_command_definition_t cli_command_table[] = {
		{ "reboot", "Reboot entire system\r\n", cli_reboot_command, 0 },
		{ NULL, NULL, NULL, 0 }
};

// reboot function
static void cli_reboot_command(char *command_str) {
	usb_printf("Rebooting system in 1 second...\r\n");

	osDelay(1000);

	NVIC_SystemReset();

	return;
}