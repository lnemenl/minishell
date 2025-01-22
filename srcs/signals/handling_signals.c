/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/22 13:11:50 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t	g_signo;
void	handle_sigint(int sigint)
{
	g_signo = sigint;
	write(STDERR_FILENO, "\n", 1);	// Print newline (using write because it's async-safe)
	rl_on_new_line();				// Tell readline "cursor is at the start of new line now"
	rl_replace_line("", 0);			// Clear any partial command user was typing
	rl_redisplay();					// Show prompt again
}

void	handle_sigquit(int sig)
{
	g_signo = sig;
	//Do nothing for SIGQUIT in interactive mode
}

// void	setup_shell_signals(t_mshell *mshell)
// {
// 	(void)mshell;
// 	struct sigaction	sa_int;
// 	struct sigaction	sa_quit;
	
// 	//Initializing sigaction structs
// 	sigemptyset(&sa_int.sa_mask); // Initialize empty signal mask for SIGINT
// 	sigemptyset(&sa_quit.sa_mask); // Initialize empty signal mask for SIGQUIT
// 	sa_int.sa_flags = 0; // No special flags needed
// 	sa_quit.sa_flags = 0; // No special flags needed

// 	//Setting handlers
// 	sa_int.sa_handler = handle_sigint;
// 	sa_quit.sa_handler = handle_sigquit;

// 	//Applying signal handlers
// 	sigaction(SIGINT, &sa_int, NULL);
// 	sigaction(SIGQUIT, &sa_quit, NULL);
// }

void    setup_shell_signals(t_mshell *mshell)
{
    struct sigaction    sa_int;
    struct sigaction    sa_quit;
    struct termios      term;

    // Get current terminal attributes
    tcgetattr(STDIN_FILENO, &term);
    // Disable CTRL character display (^C, ^\)
    term.c_lflag &= ~ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    sigemptyset(&sa_int.sa_mask);
    sigemptyset(&sa_quit.sa_mask);
    sa_int.sa_flags = 0;
    sa_quit.sa_flags = 0;
    
    sa_int.sa_handler = handle_sigint;
    sa_quit.sa_handler = handle_sigquit;
    
    sigaction(SIGINT, &sa_int, NULL);
    sigaction(SIGQUIT, &sa_quit, NULL);
}

int	init_shell_mode(t_mshell *mshell)
{
	if (!mshell)
		return (0);
	
	mshell->interactive_mode = isatty(STDIN_FILENO);	// Check if input is from terminal
	if (mshell->interactive_mode)						// If terminal input
	{
		//setting up handling for interactive mode
		setup_shell_signals(mshell);
	}
	return (1);
}


// Resetting signals to default in child processes
/* As 

# In shell with default handlers:
$ cat longfile.txt
^C              # CTRL+C immediately stops cat
$ 

# With wrong handlers (if child inherited shell's):
$ cat longfile.txt
^C              # Would ignore or handle incorrectly
# cat might continue running or handle signals wrongly


$ sleep 100     # Start a sleep command
^C              # Press CTRL+C
# With default handlers: sleep terminates
# With shell handlers: sleep might continue or behave unexpectedly
*/

void    reset_signals_to_default(void)
{
    struct sigaction    sa;
    
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIG_DFL;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

/* During command execution, the parent process (shell) needs different signal handling than during interactive mode because:

It needs to wait for child processes
It shouldn't show a new prompt if CTRL+C happens during command execution
It should allow signals to reach the child process group

*/

void	setup_execution_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	
	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_flags = 0;
	
	// During execution we want to ignore signals in parent. This allows signals to be handled by child process group
	sa_int.sa_handler = SIG_IGN; // Ignore SIGINT
	sa_quit.sa_handler = SIG_IGN; // Ignore SIGQUIT
	
	// Applying execution signal handlers
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}
