#include <stdio.h>
#include <string.h>
#include "../src/cli_static.h"

void cmd_execute(char *cmd) {
	static char buffer[512];
	
	// copy command to ram buffer
	snprintf(buffer, 512, cmd);
	
	// print command to terminal
	printf("PARSE: %s", buffer);
	
	// process command in CLI
	cli_process_command(buffer);
}

int main(int argc, char** argv) {
	printf("Tests starting...\r\n");
	
	cmd_execute("help\r\n");
	
	cmd_execute("reboot\r\n");
	
	cmd_execute("cmd-print test1 test2 test3 test4\r\n");
	
	cmd_execute("cmd-print '0:/test folder/file 1.txt'\r\n");
	cmd_execute("cmd-print file '0:/test folder/file 1.txt'\r\n");
	cmd_execute("cmd-print '0:/test folder/file 1.txt' file\r\n");
	
	cmd_execute("cmd-print-buf '0:/test folder/file 1.txt'\r\n");
	cmd_execute("cmd-print-buf file '0:/test folder/file 1.txt'\r\n");
	cmd_execute("cmd-print-buf '0:/test folder/file 1.txt' file\r\n");
	
	return 0;
}
