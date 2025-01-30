/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:03:23 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/30 11:31:36 by msavelie         ###   ########.fr       */
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
	obj.executing_command = 0;
	(void) argv;
	return (obj);
}

int	main(int argc, char **argv, char **envp)
{
	t_mshell	obj;

	if (argc != 1)
		return (error_ret(1, NULL));
	obj = init_shell(argv, envp);
	
	//initializing shell's signal handling mode
	if (isatty(STDIN_FILENO))
		setup_shell_signals(&obj);
	
	while (1)
	{
		obj.cmd_line = readline(PROMPT);
		if (!obj.cmd_line)	// Handling CTRL+D (EOF)
		{
			// Printing newline for clean exit (CTRL+D should not exit on the same line as prompt)
			write(STDERR_FILENO, "\n", 1);
			break;			// Exit shell cleanly
		}
		parse(&obj);
		add_history(obj.cmd_line);
		free(obj.cmd_line);
		obj.cmd_line = NULL;
		choose_actions(&obj);
		close_fds(&obj);
		while (obj.exec_cmds > 0)
		{
			if (wait(&status) == obj.pids[obj.pipes_count] && WIFEXITED(status))
				obj.exit_code = WEXITSTATUS(status);
			obj.exec_cmds--;
		}
		clean_mshell(&obj);
		obj.paths = fetch_paths(obj.envp);
	}
	unlink(".heredoc_temp");
	clean_mshell(&obj);
	if (obj.envp)
		free(obj.envp);
	return (obj.exit_code);
}
