#include <stdio.h>
#include <string.h>
#include "../src/cli_static.h"

#if CLI_CMD_LIST_STATIC == 0
extern void cli_reboot_command(cli_channel_t* chn, char *command_str);
extern void cli_print_command(cli_channel_t* chn, char *command_str);
extern void cli_print_buf_command(cli_channel_t* chn, char *command_str);
#endif

void usart_out(char ch) {
	printf("%c", ch);
}

void eth_out(char ch) {
	printf("%c", ch);
}

void cmd_execute(cli_channel_t *chn, char *cmd) {
	static char buffer[512];
	
	// copy command to ram buffer
	snprintf(buffer, 512, cmd);
	
	// print command to terminal
	printf("PARSE [%s]: %s", chn->name, buffer);
	
	// process command in CLI
	cli_process_command(chn, buffer);
}

int main(int argc, char** argv) {
	printf("Tests starting...\r\n");
	
	// register channels
	cli_channel_t *cli_usart = cli_channel_register("USART", usart_out);
	cli_channel_t *cli_eth = cli_channel_register("ETH", eth_out);
	
#if CLI_CMD_LIST_STATIC == 0
	cli_command_register_global("reboot", "Reboot entire system\r\n", cli_reboot_command, 0);
	//cli_command_register_global("cmd-print", "Print all the given parameters\r\n", cli_print_command, 0),
	//cli_command_register_global("cmd-print-buf", "Print all the given parameters\r\n", cli_print_command, 0),
	//cli_command_register_channel(cli_usart, )
#endif
	
	cmd_execute(cli_usart, "help\r\n");
	
	cmd_execute(cli_usart, "reboot\r\n");
	
	cmd_execute(cli_usart, "cmd-print test1 test2 test3 test4\r\n");
	
	cmd_execute(cli_usart, "cmd-print '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, "cmd-print file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, "cmd-print '0:/test folder/file 1.txt' file\r\n");
	
	cmd_execute(cli_usart, "cmd-print-buf '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, "cmd-print-buf file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_usart, "cmd-print-buf '0:/test folder/file 1.txt' file\r\n");
	
	cmd_execute(cli_eth, "help\r\n");
	
	cmd_execute(cli_eth, "reboot\r\n");
	
	cmd_execute(cli_eth, "cmd-print test1 test2 test3 test4\r\n");
	
	cmd_execute(cli_eth, "cmd-print '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, "cmd-print file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, "cmd-print '0:/test folder/file 1.txt' file\r\n");
	
	cmd_execute(cli_eth, "cmd-print-buf '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, "cmd-print-buf file '0:/test folder/file 1.txt'\r\n");
	cmd_execute(cli_eth, "cmd-print-buf '0:/test folder/file 1.txt' file\r\n");
	
	return 0;
}
