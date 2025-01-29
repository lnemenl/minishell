/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/29 14:27:23 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


volatile sig_atomic_t g_exit_code = 0;

void	sigint_handler(int sig, siginfo_t *info, void *context)
{
	(void)sig;
	(void)info;
	(void)context;
	
	if (isatty(STDIN_FILENO))
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_exit_code = 130;
	}
	else
	{
		g_exit_code = 130;
	}
}

void	sigquit_handler(int sig, siginfo_t *info, void *context)
{
	(void)sig;
	(void)info;
	(void)context;
	if (!isatty(STDIN_FILENO)) // Only handle SIGQUIT in non-interactive mode
	{
		ft_putstr_fd("Quit: 3\n", 2); // Print "Quit: 3" and exit
		g_exit_code = 131;
		exit(g_exit_code); // Exit immediately in non-interactive mode
	}
}


void	handle_eof(void)
{
	if (isatty(STDIN_FILENO))
	{
		write(1, "exit\n", 5);	// Simulate "exit" command on Ctrl+D
		rl_clear_history();
		g_exit_code = 0;
		exit(g_exit_code);		// Exit the shell gracefully
	}
	exit(g_exit_code);			// Exit in both interactive and non-interactive
}

void	setup_signal_handlers(void)
{
	struct sigaction    sa_int;
	struct sigaction    sa_quit;

	sa_int.sa_sigaction = sigint_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &sa_int, NULL);

	sa_quit.sa_sigaction = sigquit_handler;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = SA_SIGINFO;
	sigaction(SIGQUIT, &sa_quit, NULL);
}