/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 12:29:21 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/27 16:38:43 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	is_builtin_cmd(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0 
		// || ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0)
		return (1);
	return (0);
}

static void	exit_child(t_mshell *obj, char *arg, int exit_code)
{
	obj->exit_code = exit_code;
	clean_mshell(obj);
	if (!*arg)
		ft_putstr_fd(": ", 2);
	perror(arg);
	if (errno == EACCES && exit_code != 1)
		exit_code = 126;
	exit(exit_code);
}

static void	run_builtins(char **args, t_mshell *obj)
{
	if (ft_strcmp(args[0], "echo") == 0)
		echo(args);
	// else if (ft_strcmp(args[0], "cd") == 0)
	// 	open_dir(args[1]);
	else if (ft_strcmp(args[0], "pwd") == 0)
		pwd();
	else if (ft_strcmp(args[0], "export") == 0)
		export(args, obj->envp);
	else if (ft_strcmp(args[0], "env") == 0)
		env(args, obj->envp);
	exit_child(obj, args[0], 127);
}

pid_t	execute_cmd(t_mshell *obj)
{
	// count number of commands
	// allocate memory for pids
	// count pipes and allocate memory for them
	pid_t p;

	// fork process
	p = -1;
	if (ft_strcmp(obj->ast->args[0], "cd") == 0)
	{
		open_dir(obj->ast->args[1]);
		return (p);
	}
	p = fork();
	if (p == -1)
	{
		clean_mshell(obj);
		return (p);
	} // cleanup here
	if (p == 0)
	{
		// choose cmd (built-ins or common ones)
		// execute
		if (is_builtin_cmd(obj->ast->args[0]))
			run_builtins(obj->ast->args, obj);
		else
		{
			obj->cur_path = check_paths_access(obj->paths, obj->ast->args, obj);
			execve(obj->cur_path, obj->ast->args, obj->paths);
			exit_child(obj, obj->ast->args[0], 127);
		}
	}
	return (p);
}
