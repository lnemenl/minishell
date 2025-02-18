/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 16:14:58 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/13 10:58:09 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	clean_pipes(t_mshell *obj)
{
	int	i;

	if (!obj->pipfd)
		return ;
	if (!obj->allocated_pipes == 0)
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

void	close_fds(t_mshell *obj)
{
	int	i;

	if (!obj)
		return ;
	i = 0;
	while (obj->pipfd && i < obj->allocated_pipes)
	{
		if (obj->pipfd[i][0] != -1)
			close(obj->pipfd[i][0]);
		obj->pipfd[i][0] = -1;
		if (obj->pipfd[i][1] != -1)
			close(obj->pipfd[i][1]);
		obj->pipfd[i][1] = -1;
		i++;
	}
	if (obj->fd_in != -1)
		close(obj->fd_in);
	obj->fd_in = -1;
	if (obj->fd_out != -1)
		close(obj->fd_out);
	obj->fd_out = -1;
}

void	clean_mshell(t_mshell *obj)
{
	if (!obj)
		return ;
	ft_clean_strs(obj->paths);
	clean_tokens(obj->token);
	obj->token = NULL;
	clean_pipes(obj);
	//free_ast(obj->ast);
	if (obj->pids)
	{
		free(obj->pids);
		obj->pids = NULL;
	}
	obj->exec_cmds = 0;
	obj->cur_pid = 0;
	obj->pipes_count = 0;
}

void	clean_exit(t_mshell *obj)
{
	clean_mshell(obj);
	if (obj->envp)
		ft_free_strs(obj->envp, get_envp_length(obj->envp));
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
