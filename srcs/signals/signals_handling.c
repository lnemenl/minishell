/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 17:36:54 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

void	disable_echoctl(void)
{
	struct termios	term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void	interactive_sigint_handler(int signum)
{
	disable_echoctl();
	(void)signum;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

void	heredoc_sigint_handler(int signum)
{
	g_signal_received = signum;
}

void	transition_signal_handlers(t_signal_state new_state)
{
	g_signal_received = 0;
	if (new_state == SIGNAL_STATE_INTERACTIVE)
		setup_interactive_signals();
	else if (new_state == SIGNAL_STATE_HEREDOC)
		setup_heredoc_signals();
	else
		setup_exec_signals();
}
