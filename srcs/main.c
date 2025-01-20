/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:17:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/20 19:27:25 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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
	obj.paths = fetch_paths(envp, 1);
	obj.envp = envp;
	obj.pipes_count = 0;
	obj.token = NULL;
	obj.pids = NULL;
	obj.cur_pid = 0;
	obj.fd_in = -1;
	obj.fd_out = -1;
	(void) argv;
	return (obj);
}

static void	create_env_file(char **envp)
{
	int			fd;
	int			i;
	t_mshell	*obj;

	obj = NULL;
	if (!envp)
		exit (1);
	fd = open(".env_temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		unlink(".env_temp.txt");
		exit(error_ret(6, NULL, obj));
	}
	i = 0;
	while (envp[i])
	{
		if (write(fd, envp[i], ft_strlen(envp[i])) < 0
			|| write(fd, "\n", 1) < 0)
		{
			close(fd);
			unlink(".env_temp.txt");
			exit(error_ret(6, NULL, obj));
		}
		i++;
	}
	close(fd);
}

int	main(int argc, char **argv, char **envp)
{
	t_mshell	obj;
	int 		status;

	if (argc != 1)
		return (error_ret(1, NULL, NULL));
	create_env_file(envp);
	obj = init_shell(argv, envp);
	
	//initializing shell's signal handling mode
	init_shell_mode(&obj); // This sets up interactive mode signals
	
	while (1)
	{
		obj.cmd_line = readline(PROMPT);
		if (!obj.cmd_line)	// Handling CTRL+D (EOF)
		{
			// Printing newline for clean exit (CTRL+D should not exit on the same line as prompt)
			write(STDERR_FILENO, "\n", 1);
			break;			// Exit shell cleanly
		}
		if (ft_strcmp(obj.cmd_line, "exit") == 0)
		{
			free(obj.cmd_line);
			break;
		}
		if (parse(&obj))
		{
			free(obj.cmd_line);
			obj.cmd_line = NULL;
			continue;
		}
		add_history(obj.cmd_line);
		free(obj.cmd_line);
		obj.cmd_line = NULL;
		choose_actions(&obj);
		close_fds(&obj);
		while (obj.exec_cmds > 0)
		{
			wait(&status);
			obj.exec_cmds--;
		}
		clean_mshell(&obj);
		obj.paths = fetch_paths(envp, 0);
	}
	unlink(".heredoc_temp");
	clean_mshell(&obj);
	return (0);
}
