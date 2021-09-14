/*
 * cli_static.c
 */

#include "cli_static.h"

// Standard includes
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#if CLI_CMD_LIST_STATIC == 1
// externally declared command table
extern const cli_command_definition_t cli_command_table[];
#else
static cli_command_definition_t cli_command_table[CLI_CMD_LIST_MAX] = { 0 };
#endif

// buffered channels
static cli_channel_t _channels[CLI_CHANNELS_MAX] = { 0 };

// static function which gets the number of parameters.
// This number depends on the amount of found spaces
static int8_t cli_get_number_of_parameters(const char *command_str);

// help command function
static void cli_help_command(void);

// trim command
static void cli_trim_lf_cr(char *str);

// register channel function
cli_channel_t *cli_channel_register(const char *name, channel_putc_f out) {
	// are there any spots free?
	for (int i = 0; i < CLI_CHANNELS_MAX; i++) {
		if (_channels[i].name == NULL && _channels[i].out == NULL) {
			_channels[i].name = name;
			_channels[i].out = out;
			return &_channels[i];
		}
	}

	// no more room
	return NULL;
}

#if CLI_CMD_LIST_STATIC == 0
int cli_command_register(const char *command_str, const char *help_str, const command_line_callback_f command_callback, uint8_t parameter_count) {
	// are there any spots free?
	for (int i = 0; i < CLI_CMD_LIST_MAX; i++) {
		if (cli_command_table[i].command_str == NULL && cli_command_table[i].help_str == NULL && cli_command_table[i].command_callback == NULL) {
			cli_command_table[i].command_str = command_str;
			cli_command_table[i].help_str = help_str;
			cli_command_table[i].command_callback = command_callback;
			cli_command_table[i].parameter_count = parameter_count;
			return 0;
		}
	}

	// no room
	return -1;
}
#endif

// process command function
int cli_process_command(cli_channel_t *chn, char *received_command_str) {
	// check parameters
	if (received_command_str == NULL) {
		return -1;
	}

	// trim the command string
	cli_trim_lf_cr(received_command_str);

	// return value
	int ret = 0;

	// check for help command
	if (strncmp(received_command_str, "help", 4) == 0 && received_command_str[4] == 0) {
		// execute help command
		cli_help_command(chn);
		// return ok
		return ret;
	}

	// pointer to first known command
	const cli_command_definition_t *ptr;

	// Search for the command string in the list of registered commands.
	for (ptr = cli_command_table; ptr->command_str != NULL; ptr++) {
		// get the string length from the current command
		int command_str_len = strlen(received_command_str);
		// check if the length from the recieved command is shorter than the command
		// string. This ensures that half a command doesn't respond
		if (command_str_len >= strlen(ptr->command_str)) {
			// check the byte after the expected end of the string is either the end of
			// the string or a space before a parameter. This ensures that the string is not
			// part of a longer command.
			if ((received_command_str[command_str_len] == ' ') || (received_command_str[command_str_len] == 0x00)) {
				// compare the string
				if (strncmp(ptr->command_str, received_command_str, strlen(ptr->command_str)) == 0) {
					// The command has been found.  Check it has the expected
					// number of parameters.  If cExpectedNumberOfParameters is -1,
					// then there could be a variable number of parameters and no
					// check is made.
					if (ptr->parameter_count >= 0) {
						if (cli_get_number_of_parameters(received_command_str) < ptr->parameter_count) {
							ret = -1;
						}
					}

					// done, break from this loop
					break;
				}
			}
		}
	}

	// check the result
	if ((ptr->command_str != NULL) && (ret == -1)) {
		// The command was found, but the number of parameters with the command was incorrect.
		cli_printf(chn, "Incorrect command parameter(s).  Enter 'help' to view a list of available commands.\r\n\0");
		// return error
		ret = -1;
	}
	else if (ptr->command_str != NULL) {
		// Call the callback function that is registered to this command.
		ptr->command_callback(chn, received_command_str);
		// return ok
		ret = 0;
	}
	else {
		// pxCommand was NULL, the command was not found.
		cli_printf(chn, "Command not recognized.  Enter 'help' to view a list of available commands.\r\n\0");
		// return error
		ret = -1;
	}

	// return the return value
	return ret;
}

// printf through the given channel
void cli_printf(cli_channel_t *chn, const char *fmt, ...) {
	// start list
	va_list args;
	va_start(args, fmt);

	// get length
	int len = vsnprintf(NULL, 0, fmt, args);

	// create buffer
	//char buf[len];
	char* buf = malloc(len);

	// print again
	vsnprintf(buf, len, fmt, args);

	// end
	va_end(args);

	// write it
	cli_write_data(chn, buf, len);

	free(buf);
}

// write a buffer of data through the given channel
void cli_write_data(cli_channel_t *chn, uint8_t *data, uint32_t len) {
	// write complete buffer to out function
	for (uint32_t i = 0; i < len; i++) {
		chn->out((char)data[i]);
	}
}

// Search for the next parameter. This is done by searching for
// the next space. However, there should also be acounted for
// apostrophes inside the string which surround long parameters.
static char *cli_find_next_parameter(char *start) {
	uint8_t long_parameter = 0;
	
	// check if the data before this is an apostrophe
	start--;
	if (*start == '\'')
		long_parameter = 1;
	start++;
	
	// Find the start of the next string.
	while (long_parameter || *start != ' ') {
		// is this a parameter that is surrounded by apostrophe?
		if (*start == '\'')
			long_parameter = !long_parameter;
		
		// have we reached the end of the string?
		if (*start == 0x00)
			return NULL;
			
		// increment index
		start++;
	}
	
	// move pointer behind the spaces
	while (*start == ' ')
		start++;
		
	// if this is a parameter with an apostrophe, skip the apostrophe
	if (*start == '\'')
		start++;
			
	// return the found parameter
	return start;
}

// Get the parameter length.
static int cli_get_parameter_len(char *start) {
	uint8_t long_parameter = 0;
	int len = 0;
	
	// check if the data before the current pointer is an apstrophe
	start--;
	if (*start == '\'')
		// this parameter is a long parameter
		long_parameter = 1;
	start++;
	
	// Find the next space or end of string
	while (long_parameter || (*start != ' ' && *start != 0)) {
		// is this a parameter that is surrounded by apostrophe?
		if (*start == '\'')
			long_parameter = !long_parameter;
		
		// increment index
		start++;
		len++;
	}
	
	// check if the data before the current pointer is an apstrophe
	start--;
	if (*start == '\'')
		// decrement length
		len--;
	start++;
	
	// return the found length
	return len;
}

// get parameter function
char *cli_get_parameter(char *command_string, int wanted_parameter, int *parameter_str_len) {
	// create variables
	int parameters_found = 0;
	char *return_str = NULL;
	
	// clear the string length
	*parameter_str_len = 0;
	
	// loop until the found parameter is the same as the wanted parameter
	while (1) {
		// Find the start of the next string. If this is the first loop,
		// the command will be skipped and the first parameter is found.
		command_string = cli_find_next_parameter(command_string);
		
		// is it valid?
		if (command_string == NULL)
			return NULL;
			
		// increment the found index
		parameters_found++;

		// Is this the start of the required parameter?
		if (parameters_found == wanted_parameter) {
			// How long is the parameter?
			return_str = command_string;
			
			// get the length of this parameter
			*parameter_str_len = cli_get_parameter_len(command_string);

			// set string length to zero
			if (*parameter_str_len == 0)
				return_str = NULL;
				
			// done
			break;
		}
	}

	// return the string pointer
	return return_str;
}

// get parameter function
int cli_get_parameter_buf(char *command_string, int wanted_parameter, char *buf, int buf_len) {
	// create variables
	int parameters_found = 0;
	int parameter_str_len = 0;
	int ret = 0;

	// clear buffer first
	memset(buf, 0, buf_len);

	// loop until the found parameter is the same as the wanted parameter
	while (1) {
		// Find the start of the next string. If this is the first loop,
		// the command will be skipped and the first parameter is found.
		command_string = cli_find_next_parameter(command_string);
		
		// is it valid?
		if (command_string == NULL)
			return -1;
			
		// increment the found index
		parameters_found++;

		// Is this the start of the required parameter?
		if (parameters_found == wanted_parameter) {			
			// get the length of this parameter
			parameter_str_len = cli_get_parameter_len(command_string);

			// set string length to zero
			if (parameter_str_len == 0)
				ret = -1;
			else if (parameter_str_len > buf_len)
				ret = -1;
			else
				memcpy(buf, command_string, parameter_str_len);
			
			// done
			break;
		}
	}

	// return the string pointer
	return ret;
}

int cli_get_parameter_int(char *cmd, int index, int *ret) {
	char int_buf[32] = { 0 };

	// get parameter
	if (cli_get_parameter_buf(cmd, index, int_buf, 32) != 0)
		return -1;

	// all characters in the parameter should be a number, is this the case?
	for (uint8_t i = 0; i < strlen(int_buf); i++) {
		if (!isdigit(int_buf[i]))
			return -1;
	}

	// convert to integer
	*ret = atoi(int_buf);

	// all is good
	return 0;
}

int cli_get_parameter_float(char *cmd, int index, float *ret) {
	char float_buf[32] = { 0 };

	// get parameter
	if (cli_get_parameter_buf(cmd, index, float_buf, 32) != 0)
		return -1;

	// all characters in the parameter should be a number or a dot, is this the case?
	for (uint8_t i = 0; i < strlen(float_buf); i++) {
		if (!isdigit(float_buf[i]) && float_buf[i] != '.')
			return -1;
	}

	// convert to float
	*ret = atof(float_buf);

	// all is good
	return 0;
}

// help command
static void cli_help_command(cli_channel_t *chn) {
	// pointer to first known command
	const cli_command_definition_t *ptr;

	// first line on terminal
	cli_printf(chn, "Known commands:\r\n");

	// Search for the command string in the list of registered commands.
	for (ptr = cli_command_table; ptr->command_str != NULL; ptr++) {
		// print the command and help string
		cli_printf(chn, "%s -> %s\r\n", ptr->command_str, ptr->help_str);
	}
}

// get number of parameters
static int8_t cli_get_number_of_parameters(const char *command_str) {
	// create variables
	int8_t parameters = 0;
	int last_character_was_space = 0;

	// Count the number of space delimited words in pcCommandString.
	while (*command_str != 0x00) {
		if ((*command_str) == ' ') {
			if (last_character_was_space != 1) {
				parameters++;
				last_character_was_space = 1;
			}
		}
		else {
			last_character_was_space = 0;
		}
		command_str++;
	}

	// If the command string ended with spaces, then there will have been too
	// many parameters counted.
	if (last_character_was_space == 1) {
		parameters--;
	}

	// The value returned is one less than the number of space delimited words,
	// as the first word should be the command itself.
	return parameters;
}

static void cli_trim_lf_cr(char *str) {
	// sanity check parameter
	if (str == NULL) {
		return;
	}

	// get string length
	int str_end = strlen(str);

	// sanity check string length
	if (str_end == 0) {
		return;
	}

	// one step back, to the last character
	str_end--;

	// keep removing CR and LF characters
	while (str[str_end] == '\r' || str[str_end] == '\n') {
		str[str_end] = 0;
		str_end--;
	}
}
