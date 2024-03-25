# Rapid-UNIX-Shell---RUSH
Rush is a simple, efficient Unix shell built as a command line interpreter

Program Specification
Rush (Rapid Unix SHell) is an interactive loop shell that operates by continuously printing a prompt rush> , parsing user input, executing the entered command, and waiting for its completion. This cycle persists until the user types exit.

Structure
Basic Shell: The shell operates within a while loop, prompting for input, parsing commands, executing them, and repeating until exit is entered.

Paths: Rush utilizes a path variable to determine the directories to search for executables

Built-in Commands: Implements built-in commands exit, cd, and path

Redirection: Supports redirection of standard output

Parallel Commands: Allows running commands in parallel using the & operator
