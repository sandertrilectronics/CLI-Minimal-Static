# CLI-Minimal-Static

This repository contains a minimalistic Command Line Interface (CLI) for embedded devices. Initially I used the FreeRTOS Plus CLI library for a project, but wasn't very satisfied because this library registers all commands in dynamic RAM using malloc. This was not desired for a small CLI on an embedded device, I wanted all CLI data to be in ROM.

The CLI itself is very basic and can be implemented with little porting. The "cli_process_command" takes an input character array and executes the given command if it is implemented inside the "cli_static_cmd.c" file. The only thing that has to be changed by the user is the CLI_PUTS macro. This macro is used for sending feedback when a command is given but not known.

A couple of example commands are shown in the "cli_static_cmd.c" file. Implementing commands is done in two steps: writing the function that needs to be executed when the command is given and placing this function with the command and a help string inside the "cli_command_table".

The CLI parameters should be splitted with spaces. For example the following is valid:

```
cmd-print param1 param2
```

However, this may cause trouble when we are parsing (for example) file names:

```
cmd-print file A:/example folder/example file.txt
```

This causes the string to be splitr into four parameters. This behaviour is not wanted. To overcome this, the apostrophe can be used to indicate that this is indeed a complete string:

```
cmd-print file 'A:/example folder/example file.txt'
```
