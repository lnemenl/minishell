# Minishell Project Plan

## Overview of Minishell Requirements
- **Features**:
  1. Interactive shell with a prompt.
  2. Command execution (absolute, relative paths, and `PATH` lookup).
  3. Handle built-ins (`cd`, `echo`, `pwd`, etc.).
  4. Pipes (`|`) and redirections (`<`, `>`, `>>`, `<<`).
  5. Environment variables (`$VAR`) and `$?` handling.
  6. Signal handling (`Ctrl+C`, `Ctrl+D`, `Ctrl+\`).
  7. Memory management: No leaks.

- **Constraints**:
  - Limited to one global variable (for signal handling).
  - No undefined behavior (segfaults, double frees, etc.).

---

## Work Distribution
### Person A (Pipes and Redirection Expert)
- Pipes (`|`) and redirections (`<`, `>`, `>>`, `<<`).
- Command execution via `fork`, `execve`, and `dup2`.
- File descriptor management and cleanup.

### Person B (Signals and Built-ins Expert)
- Signal handling (`Ctrl+C`, `Ctrl+D`, `Ctrl+\`).
- Parsing environment variables (`$VAR` and `$?`).
- Implementation of built-ins (`cd`, `echo`, etc.).

---

## Step-by-Step Plan

### Phase 1: Foundation
**(Pair Work)**  
**Goal**: Set up the basic shell loop and minimal command execution.

1. **Basic Shell Loop**:
   - Display a prompt and wait for user input using `readline`.
   - Split the input into commands and arguments.
   - Execute a single command using `fork` and `execve`.

2. **Error Handling**:
   - Handle invalid commands (e.g., `command not found`).
   - Add a minimal error message for debugging.

3. **Testing**:
   - Test simple commands like `ls`, `pwd`, and `echo`.

---

### Phase 2: Parsing and Built-ins
**Person A**:
- Implement a tokenizer to split the input into commands and arguments.
- Handle special characters (e.g., `|`, `<`, `>`, etc.), marking tokens for later processing.

**Person B**:
- Implement built-ins:
  - `cd`: Change directories (`chdir`).
  - `pwd`: Print the current working directory (`getcwd`).
  - `echo`: Print arguments (support `-n` option).
  - `exit`: Exit the shell.
- Implement `$?` handling to store and display the last command's exit status.

**Integration**:
- Test built-ins and basic parsing together.
- Verify that commands and built-ins execute in the same shell loop.

---

### Phase 3: Pipes and Redirections
**Person A**:
- Implement pipes (`|`):
  - Create pipes using `pipe()`.
  - Redirect input/output between processes using `dup2`.
  - Chain multiple commands (`cmd1 | cmd2 | cmd3`).

- Implement redirections:
  - Input (`<`) and output (`>`, `>>`) redirection using `open` and `dup2`.
  - Heredoc (`<<`) implementation with delimiter reading.

**Person B**:
- Manage edge cases:
  - Test combinations of pipes and redirections (`cmd1 < infile | cmd2 > outfile`).
  - Handle invalid syntax (e.g., unmatched quotes, incomplete commands).
- Assist with file descriptor cleanup and ensure no memory leaks.

---

### Phase 4: Environment Variables and Expansions
**Person A**:
- Parse and expand environment variables (`$VAR`) in commands.
- Add support for exporting and unsetting variables via `export` and `unset`.

**Person B**:
- Manage special variables:
  - `$?`: Track the last command's exit status.
  - Handle `$VAR` expansion in quotes (`"text $VAR text"`).

---

### Phase 5: Signal Handling and Interactive Features
**Person B**:
- Implement signal handling:
  - `Ctrl+C`: Interrupts the current command and displays a new prompt.
  - `Ctrl+D`: Exits the shell.
  - `Ctrl+\`: Does nothing in interactive mode.
- Use `signal` or `sigaction` for proper handling.

**Person A**:
- Ensure signals don’t interfere with pipes or redirections.
- Test signal handling during active processes (e.g., while waiting for input or executing a pipeline).

---

### Phase 6: Debugging and Testing
**(Pair Work)**  
- Test all features together:
  - Pipes and redirections (`cmd1 | cmd2 > file`).
  - Environment variables (`echo $HOME`).
  - Built-ins (`cd`, `pwd`, etc.).
  - Signal handling during interactive and command execution modes.
- Use tools like `valgrind` to ensure no memory leaks.
- Stress-test the shell with edge cases and complex command combinations.

---

## Timeline
| **Day** | **Task**                     | **Person A**                        | **Person B**                        |
|---------|------------------------------|-------------------------------------|-------------------------------------|
| 1       | Basic shell loop             | Parser and tokenization setup       | Built-ins (`cd`, `pwd`, `exit`)     |
| 2       | Command execution            | Pipes and `execve`                  | Signal handling basics              |
| 3       | Pipes and redirection        | Advanced pipes and file descriptors | Error handling for syntax/commands  |
| 4       | Heredoc and variables        | Redirections and heredoc (`<<`)     | Environment variables and `$?`      |
| 5       | Signals and built-ins        | Debug pipes and redirections        | Handle signals (`Ctrl+C`, `Ctrl+D`) |
| 6       | Testing and debugging        | Shared                              | Shared                              |

---

## Final Notes
- **Integration Points**:
  - Regularly integrate and test work to avoid conflicts.
  - Ensure consistent handling of edge cases across parsing, execution, and cleanup.

- **Leverage Past Experience**:
  - Use file descriptor management skills from **Pipex** for pipes and redirections.
  - Apply **Minitalk's** signal expertise to handle signals smoothly.

____________________________________________________________________________________


Signal Handling Reference Guide for Minishell
Overview
This document serves as a comprehensive reference for implementing signal handling in minishell, based on bash behavior. All behaviors have been tested and documented for exact replication.

Table of Contents
Interactive Mode
Empty Prompt
Command Execution
Pipe Operations
Redirections
Heredoc
Non-Interactive Mode
Via Pipe
Via Script
Built-in Commands
Special Cases
Implementation Notes
Interactive Mode
Empty Prompt
Signal	Behavior	Output	Exit Status
ctrl-C	Shows new prompt	^C\n<prompt>	130
ctrl-D	Exits shell	exit\n	Previous status
ctrl-\	No effect	None	Unchanged
Command Execution
Simple Commands
# Example with 'cat' (waiting for input)

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command, new prompt	^C\n<prompt>	130
ctrl-D	Sends EOF to command	None	0
ctrl-\	Terminates command	Quit (core dumped)	131
# Example with 'sleep 5'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command, new prompt	^C\n<prompt>	130
ctrl-D	No effect	None	N/A
ctrl-\	Terminates command	Quit (core dumped)	131
Pipe Operations
Simple Pipe
# Example with 'cat | grep test'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates both commands, new prompt	^C\n<prompt>	130
ctrl-D	Closes input to first command	None	0
ctrl-\	Terminates both commands	Quit (core dumped)	131
Multiple Pipes
# Example with 'cat | grep test | wc -l'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates entire pipeline, new prompt	^C\n<prompt>	130
ctrl-D	Closes input to first command	None	0
ctrl-\	Terminates entire pipeline	Quit (core dumped)	131
Redirections
Input Redirection (<)
# Example with 'cat < existingfile'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command, new prompt	^C\n<prompt>	130
ctrl-D	No effect (file is input)	None	N/A
ctrl-\	Terminates command	Quit (core dumped)	131
# Example with 'cat < nonexistentfile'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates error message, new prompt	^C\n<prompt>	130
ctrl-D	No effect	None	N/A
ctrl-\	No effect (error already occurred)	None	1
Output Redirection (>)
# Example with 'cat > outputfile'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command, creates empty file	^C\n<prompt>	130
ctrl-D	Closes file, returns to prompt	None	0
ctrl-\	Terminates command, creates empty file	Quit (core dumped)	131
Append Redirection (>>)
# Example with 'cat >> appendfile'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command, preserves existing file	^C\n<prompt>	130
ctrl-D	Closes file, returns to prompt	None	0
ctrl-\	Terminates command, preserves existing file	Quit (core dumped)	131
Heredoc
During Heredoc Input
# Example with 'cat << EOF'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates heredoc, returns to prompt	^C\n<prompt>	130
ctrl-D	Does nothing (must use delimiter)	None	N/A
ctrl-\	No effect	None	N/A
Special Cases in Heredoc
Before entering any content:
ctrl-C: Cancels heredoc, shows new prompt
ctrl-D: No effect (must use delimiter)
ctrl-: No effect
While typing content:
ctrl-C: Cancels heredoc, shows new prompt
ctrl-D: No effect (must use delimiter)
ctrl-: No effect
At delimiter line:
ctrl-C: Cancels heredoc
ctrl-D: No effect (must use delimiter)
ctrl-: No effect
Non-Interactive Mode
Via Pipe
# Example with: echo "ls" | ./bash

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates execution	None	130
ctrl-D	No effect (input already provided)	None	N/A
ctrl-\	Terminates execution	Quit	131
Via Script
# Example with: bash < scriptfile.sh

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates script execution	None	130
ctrl-D	No effect (input from file)	None	N/A
ctrl-\	Terminates script execution	Quit	131
Built-in Commands
echo
# Example with 'echo hello'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command	^C\n<prompt>	130
ctrl-D	No effect	None	N/A
ctrl-\	No effect	None	N/A
cd
# Example with 'cd directory'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command	^C\n<prompt>	130
ctrl-D	No effect	None	N/A
ctrl-\	No effect	None	N/A
Special Cases
Environment Variables and Quotes
# Example with 'echo $HOME'

Signal	Behavior	Output	Exit Status
ctrl-C	Terminates command	^C\n<prompt>	130
ctrl-D	No effect	None	N/A
ctrl-\	No effect	None	N/A
Empty Command with Spaces
# Example with '   '

Signal	Behavior	Output	Exit Status
ctrl-C	Shows new prompt	^C\n<prompt>	130
ctrl-D	Exits shell if line empty	exit\n	Previous status
ctrl-\	No effect	None	N/A
Implementation Notes
Global Variable Usage
Use only one global variable for signal handling
Use volatile sig_atomic_t type
Only store signal number information
Key Exit Status Codes
130: SIGINT (ctrl-C)
131: SIGQUIT (ctrl-)
0: Normal termination
Important Considerations
Interactive Mode Detection
Always check isatty() for interactive mode
Handle signals differently in parent and child processes
Signal Handler Implementation
Restore default signal handling in child processes
Use write() instead of printf() in signal handlers
Keep signal handlers as simple as possible
Readline Management
Use rl_replace_line("", 0) to clear line
Use rl_on_new_line() for prompt positioning
Use rl_redisplay() to show changes
Clear history before exiting
Process Management
Handle signals in both parent and child processes
Properly clean up child processes on termination
Reset signal handlers in child processes