/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/06 20:05:03 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t g_signal_received = 0;
static struct termios original_term;
static struct termios shell_term;

static void interactive_signal_handler(int signum)
{
    g_signal_received = signum;
    
    if (signum == SIGINT)
    {
        write(STDOUT_FILENO, "\n", 1);
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
        write(STDERR_FILENO, "Quit: (core dumped)\n", 21);
}

void init_terminal_settings(void)
{
    tcgetattr(STDIN_FILENO, &original_term);
    shell_term = original_term;
}

void setup_interactive_signals(void)
{
    struct sigaction sa;

    tcsetattr(STDIN_FILENO, TCSANOW, &shell_term);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = interactive_signal_handler;
    
    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

void setup_exec_signals(void)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = exec_signal_handler;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

void setup_heredoc_signals(void)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIG_DFL;

    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

void reset_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}


void restore_terminal_settings(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_term);
}


void    transition_signal_handlers(t_signal_state new_state)
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