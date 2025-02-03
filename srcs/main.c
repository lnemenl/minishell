/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:03:23 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/01 20:33:57 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	**copy_envp(char **envp)
{
	char	**new_envp;
	size_t	envp_len;

	if (!envp)
		return (NULL);
	envp_len = get_envp_length(envp);
	new_envp = ft_calloc(envp_len, sizeof(char *));
	if (!new_envp)
		return (NULL);
	envp_len = 0;
	while (envp[envp_len])
	{
		new_envp[envp_len] = ft_strdup(envp[envp_len]);
		if (!new_envp[envp_len])
		{
			ft_free_strs(new_envp, envp_len);
			return (NULL);
		}
		envp_len++;
	}
	new_envp[envp_len] = NULL;
	return (new_envp);
}

static t_mshell	init_shell(char **argv, char **envp)
{
	t_mshell	obj;

	obj.is_heredoc = 0;
	obj.allocated_pipes = 0;
	obj.cmd_line = NULL;
	obj.cmds = NULL;
	obj.cur_path = NULL;
	obj.pipfd = NULL;
	obj.exec_cmds = 0;
	obj.envp = copy_envp(envp);
	obj.paths = fetch_paths(obj.envp);
	obj.pipes_count = 0;
	obj.token = NULL;
	obj.pids = NULL;
	obj.cur_pid = 0;
	obj.fd_in = -1;
	obj.fd_out = -1;
	(void) argv;
	return (obj);
}

int	main(int argc, char **argv, char **envp)
{
	t_mshell	obj;
  	int status;
	struct sigaction	old_handlers[2];
	

	if (argc != 1)
		return (error_ret(1, NULL));
	save_signal_handlers(&old_handlers[0], &old_handlers[1]);
	init_terminal_settings();
	transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
	obj = init_shell(argv, envp);
	while (1)
	{
    	g_signal_received = 0;
		obj.cmd_line = readline(PROMPT);
		if (!obj.cmd_line)	// Handling CTRL+D (EOF)
		{
			write(STDOUT_FILENO, "\n", 1);
			break;
		}
		parse(&obj);
		add_history(obj.cmd_line);
		free(obj.cmd_line);
		obj.cmd_line = NULL;
		choose_actions(&obj);
		close_fds(&obj);
		while (obj.exec_cmds > 0)
		{
			if (wait(&status) == obj.pids[obj.pipes_count])
			{
				if (WIFEXITED(status))
					obj.exit_code = WEXITSTATUS(status);
				if (WIFSIGNALED(status))
				{
					//if (WTERMSIG(status) == SIGINT)
					//	write(STDERR_FILENO, "^C\n", 3);
					//else if (WTERMSIG(status) == SIGQUIT)
					//	write(STDERR_FILENO, "^\\Quit: 3\n", 10);
					obj.exit_code = 128 + WTERMSIG(status);
				}
			}
			obj.exec_cmds--;
		}
		clean_mshell(&obj);
		obj.paths = fetch_paths(obj.envp);
	}
	unlink(".heredoc_temp");
	clean_mshell(&obj);
	if (obj.envp)
		free(obj.envp);
	restore_terminal_settings();
	restore_signal_handlers(&old_handlers[0], &old_handlers[1]);
	return (obj.exit_code);
}
