/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/01 18:21:13 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t g_signal_received = 0;

// Signal handler for interactive mode
static void interactive_signal_handler(int signum)
{
    g_signal_received = signum;
    if (signum == SIGINT)
    {
        write(STDERR_FILENO, "\n", 1);
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

static void exec_signal_handler(int signum)
{
    g_signal_received = signum;
    if (signum == SIGINT)
        write(STDERR_FILENO, "\n", 1);
    else if (signum == SIGQUIT)
        write(STDERR_FILENO, "Quit: 3\n", 8);
}

void setup_interactive_signals(void)
{
    struct sigaction	sa;
	struct termios		term;
	
	//Getting current terminal attributes
	tcgetattr(STDIN_FILENO, &term);
	
	//Disabling ECHOCTL flag to remove ^C
	term.c_lflag &= ~ECHOCTL;
	
	//Setting new attributes
	tcsetattr(STDIN_FILENO, TCSANOW, &term);

    sa.sa_handler = interactive_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

void setup_exec_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = exec_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

void setup_heredoc_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

void reset_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

void    save_signal_handlers(struct sigaction *old_int, struct sigaction *old_quit)
{
    sigaction(SIGINT, NULL, old_int);
    sigaction(SIGQUIT, NULL, old_quit);
}

void    restore_signal_handlers(struct sigaction *old_int, struct sigaction *old_quit)
{
    sigaction(SIGINT, old_int, NULL);
    sigaction(SIGQUIT, old_quit, NULL);
}

void    transition_signal_handlers(t_signal_state new_state)
{
    static struct sigaction    old_handlers[2];

    save_signal_handlers(&old_handlers[0], &old_handlers[1]);

    if (new_state == SIGNAL_STATE_INTERACTIVE)
        setup_interactive_signals();
    else if (new_state == SIGNAL_STATE_EXEC)
        setup_exec_signals();
    else if (new_state == SIGNAL_STATE_HEREDOC)
        setup_heredoc_signals();
    else if (new_state == SIGNAL_STATE_RESET)
        reset_signals();
}