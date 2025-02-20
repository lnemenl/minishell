/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:34:41 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/20 15:35:21 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_builtin_cmd(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0 
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

int	run_builtins(char **args, t_mshell *obj, int is_quote_heredoc)
{
	if (!args || !*args)
		return (0);
	if (ft_strcmp(args[0], "echo") == 0)
		return(echo(args, obj, is_quote_heredoc));
	else if (ft_strcmp(args[0], "env") == 0)
	 	return (env(obj, args));
	else if (ft_strcmp(args[0], "cd") == 0)
		return (cd(args, obj));
	else if (ft_strcmp(args[0], "export") == 0)
		return (export(args, obj));
	else if (ft_strcmp(args[0], "unset") == 0)
		return (unset(args, obj));
	else if (ft_strcmp(args[0], "pwd") == 0)
		return(pwd(obj));
	else if (ft_strcmp(args[0], "exit") == 0)
	{
		check_and_handle_exit(args, obj);
		return (1);
	}
	return (0);
}
