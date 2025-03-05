# Minishell
<p align="center">
  <img src="https://raw.githubusercontent.com/ayogun/42-project-badges/refs/heads/main/covers/cover-minishell.png" alt="Minitalk Cover">
</p>

---

<p align="center">
  <img src="https://raw.githubusercontent.com/ayogun/42-project-badges/refs/heads/main/badges/minishelle.png" alt="Minitalk Badge">
  <img src="https://img.shields.io/badge/Score-100%2F100-brightgreen" alt="Score Badge">
</p>

*A 42-born shell that gets the job done.*

Welcome to **Minishell**, a lightweight shell crafted for the 42 school curriculum. Think of it as a bash mini-me: it handles commands, pipes, redirections, and builtins—all while dodging Norminette’s wrath and sporting a cheeky `shell: ` prompt.

---

## Features
- Shows a prompt (`shell: `) and remembers your command history.
- Runs executables via `PATH`, relative, or absolute paths.
- Groks quotes: `'single'` stops meta-characters; "double" allows $ expansion.
- Redirections: < (input), > (output), >> (append), << (heredoc, no history).
- Pipes (|) to chain commands smoothly.
- Expands env vars like $HOME and $? (last exit status).
- Signals: ctrl-C (new prompt), ctrl-D (exit), ctrl-\ (nada).
- Builtins: `echo -n`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`.

---

## Installation
1. Clone the repo:
   ```bash
   git clone ...
   cd minishell
   ```
2. Compile it (needs `libft` and `readline`):
   ```bash
   make
   ```
3. Launch it:
   ```bash
   ./minishell
   ```

**Note**: macOS users, grab `readline` with `brew install readline`—it links to `/opt/homebrew/opt/readline/lib`.

---

## Usage
Fire these up at the `shit-shell: ` prompt:

- Pipe it:
  ```bash
  ls -l | grep txt
  ```
- Redirect it:
  ```bash
  echo "Hello $USER" > file.txt
  ```
- Heredoc it:
  ```bash
  cat << EOF
  Type stuff here
  EOF
  ```
- Builtin it:
  ```bash
  echo -n "no newline" && pwd
  ```

### Check for Leaks
To test for memory leaks (and ignore `readline`’s noise), use Valgrind with our suppression file:
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --track-fds=yes --suppressions=./test_scripts/short.supp ./minishell
  ```

Since `readline` leaks like a sieve (not our fault!), we’ve included a suppression file at `test_scripts/short.supp` to quiet those errors. It filters out leaks from `readline` and `add_history` calls, letting you focus on our code’s cleanliness. Future debuggers: feel free to use or tweak it!

---

## Dependencies
- **libft**: Included in `./libft_updated/`, built with `make`.
- **readline**: For fancy input and history (install it if you don’t have it).
- Standard C libs (GCC or CC required).

---

## Contributing
Born at 42, but free to fork! Got a tweak? Clone it, hack it, send a pull request.

---

## License
Use it, mod it, share it—just keep the copyright notice intact.
Copyright © 2025 [Nick & Rus].

---

Built with 42 grit and a dash of chaos. Have fun!
