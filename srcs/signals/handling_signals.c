/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/23 11:28:18 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t	g_signo = 0;

void	handle_sigint(int sigint)
{
	g_signo = sigint;
	if (!isatty(STDIN_FILENO))
		return ;
	write(STDERR_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

void	handle_sigquit(int sig)
{
	(void)sig;
	//Do nothing for SIGQUIT in interactive mode
}

void    setup_shell_signals(t_mshell *mshell)
{
    struct sigaction    sa_int;
    struct sigaction    sa_quit;
    struct termios      term;
	

	(void)mshell;
	// Get current terminal attributes
    tcgetattr(STDIN_FILENO, &term);
    // Disable CTRL character display (^C, ^\)
    term.c_lflag &= ~ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
	//Setting up SIGINT handler
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
	//Setting up SIGQUIT handler
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_handler = handle_sigquit;
    sa_quit.sa_flags = SA_RESTART;
    sigaction(SIGQUIT, &sa_quit, NULL);    
}

int	init_shell_mode(t_mshell *mshell)
{
	if (!mshell)
		return (0);
	
	mshell->interactive_mode = isatty(STDIN_FILENO);	// Check if input is from terminal
	if (mshell->interactive_mode)						// If terminal input
		//setting up handling for interactive mode
		setup_shell_signals(mshell);
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
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_RESTART;
    
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
	struct sigaction	sa;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = SA_RESTART;
	
	// Applying execution signal handlers
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
