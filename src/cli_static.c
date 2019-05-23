/*
 * cli_static.c
 */

#include "cli_static.h"

// Standard includes
#include <string.h>

// externally declared command table
extern const cli_command_definition_t cli_command_table[];

// static function which gets the number of parameters.
// This number depends on the amount of found spaces
static int8_t cli_get_number_of_parameters(const char *command_str);

// help command function
static void cli_help_command(void);

// process command function
int cli_process_command(char *received_command_str) {
	// check parameters
	if (received_command_str == NULL) {
		return -1;
	}

	// return value
	int ret = 0;

	// check for help command
	if (strncmp(received_command_str, "help", 4) == 0 && received_command_str[4] == 0) {
		// execute help command
		cli_help_command();
		// return ok
		return ret;
	}

	// pointer to first known command
	const cli_command_definition_t *ptr;

	// Search for the command string in the list of registered commands.
	for (ptr = cli_command_table; ptr->command_str != NULL; ptr++) {
		// get the string length from the current command
		int command_str_len = strlen(received_command_str);
		// check the byte after the expected end of the string is either the end of 
		// the string or a space before a parameter. This ensures that the string is not
		// part of a longer command.
		if ((received_command_str[command_str_len] == ' ') || (received_command_str[command_str_len] == 0x00)) {
			// compare the string
			if (strncmp(ptr->command_str, received_command_str, command_str_len) == 0) {
				// The command has been found.  Check it has the expected
				// number of parameters.  If cExpectedNumberOfParameters is -1,
				// then there could be a variable number of parameters and no
				// check is made. 
				if (ptr->parameter_count >= 0) {
					if (cli_get_number_of_parameters(received_command_str) != ptr->parameter_count) {
						ret = -1;
					}
				}

				// done, break from this loop
				break;
			}
		}
	}

	// check the result
	if ((ptr->command_str != NULL) && (ret == -1)) {
		// The command was found, but the number of parameters with the command was incorrect.
		CLI_PUTS("Incorrect command parameter(s).  Enter 'help' to view a list of available commands.\r\n\0");
		// return error
		ret = -1;
	}
	else if (ptr->command_str != NULL) {
		// Call the callback function that is registered to this command.
		ptr->command_callback(received_command_str);
		// return ok
		ret = 0;
	}
	else {
		// pxCommand was NULL, the command was not found.
		CLI_PUTS("Command not recognized.  Enter 'help' to view a list of available commands.\r\n\0");
		// return error
		ret = -1;
	}

	// return the return value
	return ret;
}

// get parameter function
char *cli_get_parameter(char *command_string, int wanted_parameter, int *parameter_str_len) {
	// create variables
	int parameters_found = 0;
	char *return_str = NULL;
	*parameter_str_len = 0;

	while (parameters_found < wanted_parameter) {
		// Index the character pointer past the current word.  If this is the start
		// of the command string then the first word is the command itself.
		while (((*command_string) != 0x00) && ((*command_string) != ' ')) {
			command_string++;
		}

		// Find the start of the next string.
		while (((*command_string) != 0x00) && ((*command_string) == ' ')) {
			command_string++;
		}

		// Was a string found?
		if (*command_string != 0x00) {
			// Is this the start of the required parameter?
			parameters_found++;

			if (parameters_found == wanted_parameter) {
				// How long is the parameter?
				return_str = command_string;
				while (((*command_string) != 0x00) && ((*command_string) != ' ')) {
					(*parameter_str_len)++;
					command_string++;
				}

				// set string length to zero
				if (*parameter_str_len == 0) {
					return_str = NULL;
				}

				break;
			}
		}
		else {
			break;
		}
	}

	return return_str;
}

// help command
static void cli_help_command(void) {
	// pointer to first known command
	const cli_command_definition_t *ptr;

	// first line on terminal
	CLI_PUTS("Known commands:\r\n\0");

	// Search for the command string in the list of registered commands.
	for (ptr = cli_command_table; ptr->command_str != NULL; ptr++) {
		// print the help string
		CLI_PUTS(ptr->help_str);
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

