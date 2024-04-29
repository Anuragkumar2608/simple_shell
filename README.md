SIMPLE UNIX SHELL

Implemented a simple shell for UNIX.
The shell is a loop which waits for input to arrive, once there is some input command, the shell forks a new process to run the command and waits for the command to complete execution.

The signal_handler for SIGINT i.e. ctrl + c has also been added to the shell which returns the shell back to its original configuration which is the start of the loop.

The shell can be compiled using gcc -o shell shell.c -lreadline

readline might not be present by default on the system - to install libreadline-dev use the command - sudo apt-get install libreadline-dev
