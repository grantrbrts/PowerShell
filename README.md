# PowerShell
A C program designed for linux users that adds flexibility to shell commands

This program gives more power/flexibility to the user when invoking shell commands such as allowing them to open files
with special flags such as O_DSYNC. It also provides the ability to generate arbitrary graphs of processes instead of being
forced into pipelines. 

Example: 
Shell command:
  $cd data
  $(sort < a | cat b - | tr A-Z a-z > c) 2>> d
  
PowerShell command:
  PowerShell \
  --chdir data \
  --rdonly a \
  --pipe \
  --pipe \
  --creat --trunc --wronly c \
  --creat --append --wronly d \
  --command 3 5 6 tr A-Z a-z \
  --command 0 2 6 sort \
  --command 1 4 6 cat b - \
  --close 2 \
  --close 4 \
  --wait
  
  ## File Flags
--append
O_APPEND
--cloexec
O_CLOEXEC
--creat
O_CREAT
--directory
O_DIRECTORY
--dsync
O_DSYNC
--excl
O_EXCL
--nofollow
O_NOFOLLOW
--nonblock
O_NONBLOCK
--rsync
O_RSYNC
--sync
O_SYNC
--trunc
O_TRUNC

## Options
--rdonly f
O_RDONLY. Open the file f for reading only.
--rdwr f
O_RDWR. Open the file f for reading and writing.
--wronly f
O_WRONLY. Open the file f for writing only.
--pipe
Open a pipe. Unlike the other file options, this option does not take an argument.

## Subcommand options
--command i o e cmd args
Execute a command with standard input i, standard output o and standard error e; these values should correspond to earlier file or pipe options.
The executable for the command is cmd and it has zero or more arguments args. None of the cmd and args operands begin with the two characters "--".

--wait
Wait for all commands to finish. As each finishes, output its exit status or signal number as described above, and a copy of the command (with spaces separating arguments) to standard output.

## Other options
--chdir dir
Change the working directory to dir. This affects the interpretation of all later options, including later --chdir options. For example, if foo is a directory (not a symbolic link), the two successive options "--chdir foo --chdir .." taken together are a no-op if successful.

--close N
Close the Nth file that was opened by a file-opening option. For a pipe, this closes just one end of the pipe. Once file N is closed, it is an error to access it, just as it is an error to access any file number that has never been opened. File numbers are not reused by later file-opening options.

--verbose
Just before executing an option, output a line to standard output containing the option. If the option has operands, list them separated by spaces. Ensure that the line is actually output, and is not merely sitting in a buffer somewhere.

--profile
Just after executing an option, output a line to standard output containing the resources used. Use getrusage and output a line containing as much useful information as you can glean from it.

--abort
Crash the shell. The shell itself should immediately dump core, via a segmentation violation.

--catch N
Arrange for signal N, where N is a decimal integer, to be caught by a handler that outputs the diagnostic N caught to stderr, and exits with status N (i.e., exiting the entire shell). N uses the same numbering as your system; for example, on GNU/Linux, a segmentation violation is signal 11.

--ignore N
Arrange for signal N to be ignored.

--default N
Arrange for the default behavior to occur if signal N arrives.

--pause
Pause, waiting for a signal to arrive.
  
