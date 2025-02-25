/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 16:14:58 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/25 17:22:23 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	clean_pipes(t_mshell *obj)
{
	int	i;

	if (!obj->pipfd)
		return ;
	if (obj->allocated_pipes == 0)
	{
		obj->allocated_pipes = 0;
		free(obj->pipfd);
		obj->pipfd = NULL;
		return ;
	}
	i = 0;
	while (i < obj->allocated_pipes)
	{
		if (obj->pipfd[i])
			free(obj->pipfd[i]);
		obj->pipfd[i] = NULL;
		i++;
	}
	obj->allocated_pipes = 0;
	free(obj->pipfd);
	obj->pipfd = NULL;
}

void	clean_mshell(t_mshell *obj)
{
	if (!obj)
		return ;
	ft_clean_strs(obj->paths);
	obj->paths = NULL;
	clean_tokens(obj->token);
	obj->token = NULL;
	clean_pipes(obj);
	if (obj->pids)
	{
		free(obj->pids);
		obj->pids = NULL;
	}
	free_ast_return_null(&obj->ast);
	obj->exec_cmds = 0;
	obj->cur_pid = 0;
	obj->pipes_count = 0;
	obj->heredoc_interrupted = 0;
}

void	clean_exit(t_mshell *obj)
{
	clean_mshell(obj);
	if (obj->envp)
		ft_free_strs(obj->envp, get_envp_length(obj->envp));
	if (obj->exp_args)
		ft_free_strs(obj->exp_args, get_envp_length(obj->exp_args));
	check_free_str(&obj->prev_path);
	exit(obj->exit_code);
}

void	check_free_str(char **path)
{
	if (!path || !*path)
		return ;
	free(*path);
	*path = NULL;
}
