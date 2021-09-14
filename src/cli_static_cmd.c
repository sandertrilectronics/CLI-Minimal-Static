/*
 * cli_static_cmd.c
 */

// main include
#include "cli_static.h"

// function prototypes
static void cli_reboot_command(cli_channel_t* chn, char *command_str);
static void cli_print_command(cli_channel_t* chn, char *command_str);

// command table
// structure: { "command", "help string", command_function, number_of_args }
cli_command_definition_t cli_command_table[] = {
		{ "reboot", "Reboot entire system\r\n", cli_reboot_command, 0 },
		{ "cmd-print", "Print all the given parameters\r\n", cli_print_command, 0 },
		{ "cmd-print-buf", "Print all the given parameters\r\n", cli_print_command, 0 },
		{ NULL, NULL, NULL, 0 }
};

// reboot function for STM32
static void cli_reboot_command(cli_channel_t* chn, char *command_str) {
	// send some feedback
	cli_printf(chn, "Rebooting command received\r\n");
}

static void cli_print_command(cli_channel_t* chn, char *command_str) {
	int i = 1;
	int str_len = 0;
	
	// get the first parameter
	char *param = cli_get_parameter(command_str, i, &str_len);
	
	// loop until there are no more valid parameters
	while (param != NULL) {
		// print the current command
		cli_printf(chn, "%d %.*s\r\n", i, str_len, param);
		
		// increment index
		i++;
		
		// get the next parameter
		param = cli_get_parameter(command_str, i, &str_len);
	}
}

static void cli_print_buf_command(cli_channel_t* chn, char *command_str) {
	int i = 1;
	char buf[128];
	
	// get the first parameter
	int ret = cli_get_parameter_buf(command_str, i, buf, 128);
	
	// loop until there are no more valid parameters
	while (ret == 0) {
		// print the current command
		cli_printf(chn, "%d %s\r\n", i, buf);
		
		// increment index
		i++;
		
		// get the next parameter
		ret = cli_get_parameter_buf(command_str, i, buf, 128);
	}
}
