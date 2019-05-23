# CLI-Minimal-Static

This repository contains a minimalistic Command Line Interface (CLI) for embedded devices. Initially I used the FreeRTOS Plus CLI library for a project, but wasn't very satisfied because this library registers all commands in RAM (which is was not desired).

The CLI itself is very basic and can be implemented without porting. The "cli_process_command" takes an input string and executes the given command if it is implemented inside the "cli_static_cmd.c" file.