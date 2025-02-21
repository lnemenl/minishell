/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_settingup.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:23:18 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 15:17:44 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	set_sigaction(int signum, void (*handler)(int), int flags)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = flags;
	sa.sa_handler = handler;
	sigaction(signum, &sa, NULL);
}

void	setup_interactive_signals(void)
{
	set_sigaction(SIGINT, interactive_sigint_handler, SA_RESTART);
	set_sigaction(SIGQUIT, SIG_IGN, 0);
}

void	setup_exec_signals(void)
{
	set_sigaction(SIGINT, SIG_DFL, 0);
	set_sigaction(SIGQUIT, SIG_DFL, 0);
}

void	setup_heredoc_signals(void)
{
	set_sigaction(SIGINT, heredoc_sigint_handler, 0);
	set_sigaction(SIGQUIT, SIG_IGN, 0);
}
