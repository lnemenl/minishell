/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 12:29:21 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/27 14:51:20 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static int	is_builtin_cmd(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0 
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0)
		return (1);
	return (0);
}

static void	run_builtins(char **args, t_mshell *obj)
{
	if (ft_strcmp(args[0], "echo") == 0)
		echo(args);
	else if (ft_strcmp(args[0], "cd") == 0)
		open_dir(args[1]);
	else if (ft_strcmp(args[0], "pwd"))
		pwd();
	else if (ft_strcmp(args[0], "export"))
		export(args, obj->envp);
	else if (ft_strcmp(args[0], "env"))
		env(args, obj->envp);
}

void	execute_cmd(t_mshell *obj)
{
	// count number of commands
	// allocate memory for pids
	// count pipes and allocate memory for them
	pid_t p;

	// fork process
	p = fork();
	if (p == -1)
	{
		clean_mshell(obj);
		return ;
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
		}
	}
}
