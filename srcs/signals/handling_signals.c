/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/22 22:38:00 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t	g_signo;
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
	rl_redisplay();
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

	ft_memset(&sa_int, 0, sizeof(sa_int));
	ft_memset(&sa_quit, 0, sizeof(sa_quit));
    sigemptyset(&sa_int.sa_mask);
    sigemptyset(&sa_quit.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sa_quit.sa_flags = SA_RESTART;
    
    sa_int.sa_handler = handle_sigint;
    sa_quit.sa_handler = SIG_IGN;
    
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
    
	ft_memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
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
	
	ft_memset(&sa_int, 0, sizeof(sa_int));
	ft_memset(&sa_quit, 0, sizeof(sa_quit));
	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sa_quit.sa_flags = SA_RESTART;
	
	// During execution we want to ignore signals in parent. This allows signals to be handled by child process group
	sa_int.sa_handler = SIG_IGN; // Ignore SIGINT
	sa_quit.sa_handler = SIG_IGN; // Ignore SIGQUIT
	
	// Applying execution signal handlers
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/*void    handle_heredoc(t_mshell *obj, t_ast_node *node)
{
    // ... existing code ...
    
    // Before starting heredoc input
    signal(SIGINT, handle_sigint);  // Temporary switch for heredoc
    signal(SIGQUIT, SIG_IGN);
    
    // ... heredoc processing ...
    
    // After heredoc completes or is interrupted
    setup_shell_signals(obj);  // Restore normal shell signals
}*/