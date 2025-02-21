/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_invalid_cases.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 11:59:52 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 15:17:57 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_cmd_line_invalid(t_mshell *obj)
{
	if (!obj->cmd_line)
	{
		write(STDOUT_FILENO, "exit\n", 5);
		return (1);
	}
	else if (!*obj->cmd_line)
	{
		clean_mshell(obj);
		obj->paths = fetch_paths(obj->envp);
		return (2);
	}
	return (0);
}

int	is_ast_invalid(t_mshell *obj)
{
	if (!obj->ast)
	{
		free(obj->cmd_line);
		obj->cmd_line = NULL;
		clean_mshell(obj);
		obj->paths = fetch_paths(obj->envp);
		return (1);
	}
	return (0);
}

int	is_main_signaled(t_mshell *obj)
{
	if (g_signal_received == SIGINT || obj->heredoc_interrupted)
	{
		free(obj->cmd_line);
		obj->cmd_line = NULL;
		free_ast(obj->ast);
		obj->ast = NULL;
		obj->heredoc_interrupted = 0;
		clean_mshell(obj);
		obj->paths = fetch_paths(obj->envp);
		return (1);
	}
	return (0);
}
