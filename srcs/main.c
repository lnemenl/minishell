/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:17:46 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/30 11:57:53 by msavelie         ###   ########.fr       */
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
	obj.total_cmds = 0;
	obj.paths = fetch_paths(envp);
	obj.envp = envp;
	(void) argv;
	return (obj);
}

static void	create_env_file(char **envp)
{
	int	fd;
	int	i;

	if (!envp)
		exit (1);
	fd = open(".env_temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		unlink(".env_temp.txt");
		exit(error_ret(6, NULL));
	}
	i = 0;
	while (envp[i])
	{
		if (write(fd, envp[i], ft_strlen(envp[i])) < 0
			|| write(fd, "\n", 1) < 0)
		{
			close(fd);
			unlink(".env_temp.txt");
			exit(error_ret(6, NULL));
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
		return (error_ret(1, NULL));
	create_env_file(envp);
	obj = init_shell(argv, envp);
	while (1)
	{
		obj.cmd_line = readline(PROMPT);
		if (ft_strcmp(obj.cmd_line, "exit") == 0)
		{
			free(obj.cmd_line);
			break;
		}
		parse(&obj);
		add_history(obj.cmd_line);
		free(obj.cmd_line);
		obj.cmd_line = NULL;
		pid_t p = execute_cmd(&obj);
		waitpid(p, &status, 0);
	}
	clean_mshell(&obj);
	return (0);
}
