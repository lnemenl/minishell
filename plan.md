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
