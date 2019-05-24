# CLI-Minimal-Static

This repository contains a minimalistic Command Line Interface (CLI) for embedded devices. Initially I used the FreeRTOS Plus CLI library for a project, but wasn't very satisfied because this library registers all commands in RAM. This was not desired for a small CLI on an embedded device, I wanted all data to be in ROM.

The CLI itself is very basic and can be implemented with little porting. The "cli_process_command" takes an input character array and executes the given command if it is implemented inside the "cli_static_cmd.c" file. The only thing that has to be changed by the user is the CLI_PUTS macro. This macro is used for sending feedback when a command is given but not known.

One example command is shown in this file, the STM32 NVIC reset command. Implementing commands is done in two steps: writing the function that needs to be executed when the command is given and placing this function with the command and a help string inside the "cli_command_table".
