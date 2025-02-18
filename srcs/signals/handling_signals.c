/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/18 13:18:31 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


/**
 * Global signal status variable for tracking received signals
 * 
 * This global variable is used to safely communicate signal states between
 * the signal handler and the main program. Its key characteristics:
 * 
 * Implementation details:
 * 1. volatile: Prevents compiler optimization, ensuring the variable is
 *    always read from memory rather than cached registers
 * 2. sig_atomic_t: Guarantees atomic (uninterruptible) access to the variable,
 *    making it safe for use in signal handlers
 * 3. Initialized to 0: Indicates no signal has been received initially
 * 
 * The variable is modified by signal handlers and checked by the main program
 * to coordinate responses to received signals like SIGINT (Ctrl+C) or
 * SIGQUIT (Ctrl+\).
 * 
 */
volatile sig_atomic_t g_signal_received = 0;

/**
 * Disables the ECHOCTL terminal attribute to prevent control character echoing
 * 
 * This function modifies the terminal attributes to disable the ECHOCTL flag, which
 * prevents the terminal from displaying control characters (like ^C) when they are
 * typed. This creates a cleaner user experience in shell implementations.
 * 
 * Implementation steps:
 * 1. Gets current terminal settings
 * 2. Disables ECHOCTL using bitwise operation (term.c_lflag &= ~ECHOCTL):
 *    - Takes current flags and turns off only the ECHOCTL bit
 *    - Keeps all other terminal settings unchanged
 * 3. Applies new settings immediately using TCSANOW flag
 * 
 * The TCSANOW flag ensures changes are applied right away without waiting
 * for any input/output operations to complete.
 * 
 */
void disable_echoctl(void)
{
	struct termios term;
	
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

/**
 * Sets up signal handling configuration for specified signals
 * 
 * This function configures how the program responds to specific signals by
 * setting up a sigaction structure. It provides a clean way to register
 * signal handlers with specific flags and behaviors.
 * 
 * Signal Masks and Their Purpose:
 * - Block mask: Determines which signals are blocked during handler execution
 * - sigemptyset(): Creates an empty signal mask, meaning no signals blocked
 * 
 * Signal Flags (Common options):
 * - SA_RESTART: Automatically restart interrupted system calls
 * - SA_SIGINFO: Provides extended signal handling information
 * - SA_RESETHAND: Reset handler to default after first signal
 * - 0: Default behavior with no special flags
 * 
 * Implementation details:
 * 1. Creates a sigaction structure to store signal handling settings
 * 2. Initializes an empty signal mask using sigemptyset
 *    - Ensures predictable signal handling
 *    - Prevents unwanted signal blocking inheritance
 * 3. Sets specified flags for signal handling behavior
 * 4. Assigns the signal handler function
 * 5. Applies the configuration using sigaction system call
 * 
 * Parameters:
 * - signum: Signal number to handle (e.g., SIGINT, SIGQUIT)
 * - handler: Function pointer to the signal handling routine
 * - flags: Special flags to modify signal handling behavior
 * 
 */
static void	set_sigaction(int signum, void (*handler)(int), int flags)
{
	struct sigaction sa;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = flags;
	sa.sa_handler = handler;
	sigaction(signum, &sa, NULL);
}

/**
 * Handles SIGINT (Ctrl+C) signal in interactive shell mode
 * 
 * This function provides a custom response to Ctrl+C in interactive mode,
 * creating a clean shell experience by moving to a new prompt line without
 * terminating the program.
 * 
 * Implementation details:
 * 1. Disables control character echoing (^C)
 * 2. Casts signum to void to prevent unused parameter warnings
 * 3. Writes newline to create visual separation
 * 4. Manages readline library behavior:
 *    - rl_replace_line: Clears current input line content
 *    - rl_on_new_line: Informs readline about line position
 *    - rl_redisplay: Redraws the prompt and empty input line
 * 
 * The function ensures:
 * - Clean visual feedback when Ctrl+C is pressed
 * - Preservation of shell session
 * - Proper readline library state management
 * - No display of ^C in the terminal
 * 
 */
static void	interactive_sigint_handler(int signum)
{
	disable_echoctl();
	(void)signum;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

/**
 * Configures signal handling for interactive shell mode
 * 
 * This function sets up custom signal handling for interactive shell operations,
 * specifically managing Ctrl+C (SIGINT) and Ctrl+\ (SIGQUIT) behaviors.
 * 
 * Implementation details:
 * 1. SIGINT (Ctrl+C) configuration:
 *    - Uses custom handler for clean prompt management
 *    - SA_RESTART flag ensures system calls resume after interruption
 *    - Without SA_RESTART, functions like read() would return EINTR error
 *    - Prevents terminal disruption during user input
 * 
 * 2. SIGQUIT (Ctrl+\) configuration:
 *    - SIG_IGN completely ignores the signal
 *    - No flags needed (0) for simple signal ignoring
 *    - Prevents core dumps in interactive mode
 * 
 */
void	setup_interactive_signals(void)
{
	set_sigaction(SIGINT, interactive_sigint_handler, SA_RESTART);
	set_sigaction(SIGQUIT, SIG_IGN, 0);
}
/**
 * Configures signal handling for command execution mode
 * 
 * This function restores default signal handling behavior when the shell
 * executes child processes. This is crucial for proper signal propagation
 * to child processes.
 * 
 * Implementation details:
 * 1. SIGINT (Ctrl+C) configuration:
 *    - Sets to SIG_DFL (default behavior)
 *    - Allows child processes to receive and handle Ctrl+C normally
 *    - Important for stopping hung or long-running commands
 *    - No flags needed (0) for default handling
 * 
 * 2. SIGQUIT (Ctrl+\) configuration:
 *    - Sets to SIG_DFL (default behavior)
 *    - Enables core dump generation
 *    - Provides standard Unix behavior for executed programs
 *    - No flags needed (0) for default handling
 * 
 * Why SIG_DFL is necessary:
 * - Child processes should handle signals independently
 * - Interactive shell handlers shouldn't interfere with executed programs
 * - Allows proper signal propagation in process groups
 * 
 */
void	setup_exec_signals(void)
{
		set_sigaction(SIGINT, SIG_DFL, 0);
		set_sigaction(SIGQUIT, SIG_DFL, 0);
}

/**
 * Handles SIGINT (Ctrl+C) during heredoc input processing
 * 
 * This function manages signal handling during heredoc operations, ensuring
 * proper cleanup and state management when user interrupts heredoc input.
 * 
 * Implementation details:
 * 1. Sets global signal flag:
 *    - g_signal_received = signum stores interrupt status
 *    - Necessary because signal handlers can't safely return values
 *    - Main program checks this flag to know heredoc was interrupted
 *    - Without this flag, program wouldn't know to clean up heredoc state
 * 
 * 2. Resets SIGINT to default:
 *    - set_sigaction(SIGINT, SIG_DFL, 0) restores default handler
 *    - Ensures clean program termination after heredoc interruption
 *    - Prevents hanging in heredoc input state
 * 
 * Why global flag is crucial:
 * - Signal handlers run asynchronously, meaning:
 *   • They interrupt main program execution at any point
 *   • Can't directly return values to the interrupted code
 *   • Need safe way to communicate between handler and main program
 *   • Global volatile variables are one of few safe methods
 * - Can't use return values from handlers
 * - Need way to communicate interrupt status to main program
 * - Without it, heredoc state would be stuck
 * 
 * Why reset to SIG_DFL:
 * - Ensures clean exit after heredoc interruption
 * - Prevents recursive signal handling
 * - Returns terminal to normal state
 * 
 */
static void	heredoc_sigint_handler(int signum)
{
	g_signal_received = signum;
	set_sigaction(SIGINT, SIG_DFL, 0);
}

/**
 * Configures signal handling specifically for heredoc input processing
 * 
 * This function sets up special signal handling during heredoc operations,
 * ensuring proper input management and cleanup during interruptions.
 * 
 * Implementation details:
 * 1. SIGINT (Ctrl+C) configuration:
 *    - Uses custom heredoc_sigint_handler
 *    - No SA_RESTART flag because:
 *      • Want to interrupt read() operations immediately
 *      • Need to detect interruption in heredoc input
 *      • Allow immediate response to user's Ctrl+C
 *      • Enable proper cleanup of heredoc state
 * 
 * 2. SIGQUIT (Ctrl+\) configuration:
 *    - Completely ignored using SIG_IGN
 *    - No flags needed (0)
 *    - Prevents accidental heredoc termination
 *    - Maintains consistent behavior with interactive mode
 * 
 */
void	setup_heredoc_signals(void)
{
	set_sigaction(SIGINT, heredoc_sigint_handler, 0);
	set_sigaction(SIGQUIT, SIG_IGN, 0);
}

/**
 * Manages signal handler transitions between different shell states
 * 
 * This function coordinates signal handling based on shell operation mode.
 * 
 * Implementation details:
 * 1. Uses enum for clear state representation
 * 2. Consolidates similar signal handling states
 * 3. Provides clear transition paths between states
 */
void	transition_signal_handlers(t_signal_state new_state)
{
	if (new_state == SIGNAL_STATE_INTERACTIVE)
		setup_interactive_signals();
	else if (new_state == SIGNAL_STATE_HEREDOC)
		setup_heredoc_signals();
	else
		setup_exec_signals();
}
