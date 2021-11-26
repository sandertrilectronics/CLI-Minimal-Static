// testt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "cli_static.h"

#if CLI_CMD_LIST_STATIC == 0
// reboot function for STM32
void cli_reboot_command(cli_channel_t* chn, char* command_str) {
	// send some feedback
	cli_printf(chn, "Rebooting command received\r\n");
}

void cli_print_command(cli_channel_t* chn, char* command_str) {
	int i = 1;
	int str_len = 0;

	// get the first parameter
	char* param = cli_get_parameter(command_str, i, &str_len);

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

void cli_print_buf_command(cli_channel_t* chn, char* command_str) {
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
#endif

void usart_out(char ch) {
	printf("%c", ch);
}

void eth_out(char ch) {
	printf("%c", ch);
}

void cmd_execute(cli_channel_t* chn, char* cmd) {
	static char buffer[512];

	// copy command to ram buffer
	snprintf(buffer, 512, cmd);

	// print command to terminal
	printf("PARSE [%s]: %s", chn->name, buffer);

	// process command in CLI
	cli_process_command(chn, buffer);
}

int main() {
	printf("Tests starting...\r\n");

	// register channels
	cli_channel_t* cli_usart = cli_channel_register("USART", usart_out);
	cli_channel_t* cli_eth = cli_channel_register("ETH", eth_out);

#if CLI_CMD_LIST_STATIC == 0
	// register callbacks
	cli_command_register_global("reboot", "Reboot entire system", cli_reboot_command, 0);
	cli_command_register_channel(cli_usart, "cmd-print", "Print all the given parameters", cli_print_command, 0);
	cli_command_register_channel(cli_eth, "cmd-print-buf", "Print all the given parametersn", cli_print_command, 0);
#endif

	// uart channel
	cmd_execute(cli_usart, (char *)"help\r\n");

	cmd_execute(cli_usart, (char*)"reboot\r\n");

	cmd_execute(cli_usart, (char*)"cmd-print test1 test2 test3 test4\r\n");
	cmd_execute(cli_usart, (char*)"cmd-print '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, (char*)"cmd-print file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, (char*)"cmd-print '0:/test folder/file 1.txt' file\r\n");

#if CLI_CMD_LIST_STATIC == 1
	cmd_execute(cli_usart, (char*)"cmd-print-buf '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, (char*)"cmd-print-buf file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, (char*)"cmd-print-buf '0:/test folder/file 1.txt' file\r\n");
#endif

	// ethernet channel
	cmd_execute(cli_eth, (char*)"help\r\n");

	cmd_execute(cli_eth, (char*)"reboot\r\n");

#if CLI_CMD_LIST_STATIC == 1
	cmd_execute(cli_eth, (char*)"cmd-print test1 test2 test3 test4\r\n");
	cmd_execute(cli_eth, (char*)"cmd-print '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, (char*)"cmd-print file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, (char*)"cmd-print '0:/test folder/file 1.txt' file\r\n");
#endif

	cmd_execute(cli_eth, (char*)"cmd-print-buf '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, (char*)"cmd-print-buf file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, (char*)"cmd-print-buf '0:/test folder/file 1.txt' file\r\n");

	// don't close terminal
	(void)getchar();

	//
	return 0;
}

