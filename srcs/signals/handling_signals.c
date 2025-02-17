/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/17 11:38:49 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t g_signal_received = 0;

void disable_echoctl(void)
{
    struct termios term;
    
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void set_sigaction(int signum, void (*handler)(int), int flags)
{
	struct sigaction sa;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = flags;
	sa.sa_handler = handler;
	sigaction(signum, &sa, NULL);
}

//Interactive part
static void interactive_sigint_handler(int signum)
{
	disable_echoctl();
    (void)signum;
    /* Write a newline to ensure the prompt appears on a new line */
    write(STDOUT_FILENO, "\n", 1);
    /* Using GNU Readline functions to clear any partially entered text and redisplay the prompt */
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
}

void setup_interactive_signals(void)
{
    set_sigaction(SIGINT, interactive_sigint_handler, SA_RESTART);
    set_sigaction(SIGQUIT, SIG_IGN, 0);
}


//Execution part
void setup_exec_signals(void)
{
    // Set SIGINT (Ctrl-C) to its default action.
    	set_sigaction(SIGINT, SIG_DFL, 0);
    // Set SIGQUIT (Ctrl-\) to its default action.
    	set_sigaction(SIGQUIT, SIG_DFL, 0);
}

//Heredoc
static void heredoc_sigint_handler(int signum)
{
    g_signal_received = signum;
	set_sigaction(SIGINT, SIG_DFL, 0);
    // write(STDOUT_FILENO, "\n", 1);
}

void setup_heredoc_signals(void)
{
    set_sigaction(SIGINT, heredoc_sigint_handler, 0);
    set_sigaction(SIGQUIT, SIG_IGN, 0);
}

void reset_signals(void)
{
    // Reset SIGINT (Ctrl-C) to the system's default behavior.
    set_sigaction(SIGINT, SIG_DFL, 0);
    // Reset SIGQUIT (Ctrl-\) to the system's default behavior.
    set_sigaction(SIGQUIT, SIG_DFL, 0);
}

void transition_signal_handlers(t_signal_state new_state)
{

	if (new_state == SIGNAL_STATE_INTERACTIVE)
		setup_interactive_signals();
	else if (new_state == SIGNAL_STATE_EXEC)
		setup_exec_signals();
	else if (new_state == SIGNAL_STATE_HEREDOC)
		setup_heredoc_signals();
	else
		reset_signals();
}
