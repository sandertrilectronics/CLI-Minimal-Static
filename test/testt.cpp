
#include <iostream>
#include <cstdio>
#include <string>
#include "cli_static.h"

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

void cli_print_int_command(cli_channel_t* chn, char* command_str) {
    int i = 1;
    int value = 0;

    // get the first parameter
    int result = cli_get_parameter_int(command_str, i, &value);

    if (result == 0) {
        cli_printf(chn, "Int value: %d\r\n", value);
    }
    else {
        cli_printf(chn, "No integer parameter provided\r\n");
    }
}

void cli_print_float_command(cli_channel_t* chn, char* command_str) {
    int i = 1;
    float value = 0;

    // get the first parameter
    int result = cli_get_parameter_float(command_str, i, &value);

    if (result == 0) {
        cli_printf(chn, "Float value: %f\r\n", value);
    }
    else {
        cli_printf(chn, "No float parameter provided\r\n");
    }
}

// static command list
cli_command_definition_t cli_commands[] = {
    { "reboot", "Reboot entire system\r\n", cli_reboot_command, 0 },
    { "cmd-print-param", "Print all the given parameters\r\n", cli_print_command, 0 },
    { "cmd-print-buf", "Print all the given parameters\r\n", cli_print_buf_command, 0 },
#if CLI_CMD_LIST_STATIC == 1 && CLI_CMD_LIST_DYNAMIC == 0
    { "cmd-print-int", "Print integer value\r\n", cli_print_int_command, 0 },
    { "cmd-print-float", "Print float value\r\n", cli_print_float_command, 0 },
#endif
    { NULL, NULL, NULL, 0 }
};

void usart_out(char* str, int len, void* user) {
    printf("USART: %.*s", len, str);
}

void eth_out(char* str, int len, void* user) {
    printf("ETH: %.*s", len, str);
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
    cli_channel_t* cli_usart = cli_channel_register("USART", usart_out, NULL);
    cli_channel_t* cli_eth = cli_channel_register("ETH", eth_out, NULL);

#if CLI_CMD_LIST_DYNAMIC == 1
    // register callbacks
    cli_command_register_global("cmd-print-int", "Print integer value", cli_print_int_command, 0);
    cli_command_register_channel(cli_usart, "cmd-print-float", "Print float value", cli_print_float_command, 0);
    cli_command_register_channel(cli_eth, "cmd-print-float", "Print float value", cli_print_float_command, 0);
#endif

#if CLI_CMD_LIST_STATIC == 1
    // add static command list to channels
    cli_channel_add_static_commands(cli_usart, cli_commands);
    cli_channel_add_static_commands(cli_eth, cli_commands);
#endif

    // uart channel
    cmd_execute(cli_usart, (char*)"help\r\n");

    cmd_execute(cli_usart, (char*)"reboot\r\n");

    cmd_execute(cli_usart, (char*)"cmd-print-param test1 test2 test3 test4\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-param '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-param file '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-param '0:/test folder/file 1.txt' file\r\n");

    cmd_execute(cli_usart, (char*)"cmd-print-int 42\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-float 3.14\r\n");

    cmd_execute(cli_usart, (char*)"cmd-print-int -5\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-float -3.14\r\n");

    cmd_execute(cli_usart, (char*)"cmd-print-int 0\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-float 0.0\r\n");

    cmd_execute(cli_usart, (char*)"cmd-print-int 123456789\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-float 123456789.123456\r\n");

    // Add more test cases for invalid input
    cmd_execute(cli_usart, (char*)"cmd-print-int abc\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-float abc\r\n");

    cmd_execute(cli_usart, (char*)"cmd-print-int 1.23456789\r\n");
    cmd_execute(cli_usart, (char*)"cmd-print-float 123456789.\r\n");


#if CLI_CMD_LIST_STATIC == 1
    cmd_execute(cli_eth, (char*)"cmd-print-param test1 test2 test3 test4\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-param '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-param file '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-param '0:/test folder/file 1.txt' file\r\n");
#endif

    cmd_execute(cli_eth, (char*)"cmd-print-buf '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-buf file '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-buf '0:/test folder/file 1.txt' file\r\n");

    // ethernet channel
    cmd_execute(cli_eth, (char*)"help\r\n");

    cmd_execute(cli_eth, (char*)"reboot\r\n");

#if CLI_CMD_LIST_STATIC == 1
    cmd_execute(cli_eth, (char*)"cmd-print-param test1 test2 test3 test4\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-param '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-param file '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-param '0:/test folder/file 1.txt' file\r\n");
#endif

    cmd_execute(cli_eth, (char*)"cmd-print-buf '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-buf file '0:/test folder/file 1.txt'\r\n");
    cmd_execute(cli_eth, (char*)"cmd-print-buf '0:/test folder/file 1.txt' file\r\n");

    // don't close terminal
    (void)getchar();

    //
    return 0;
}
