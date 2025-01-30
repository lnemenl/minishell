/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 16:14:58 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/29 15:59:40 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	clean_pipes(t_mshell *obj)
{
	int	i;

	if (!obj->pipfd)
		return ;
	if (obj->pipfd && !*obj->pipfd)
	{
		obj->allocated_pipes = 0;
		free(obj->pipfd);
		obj->pipfd = NULL;
		return ;
	}
	i = 0;
	while (i < obj->allocated_pipes)
	{
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
		if (obj->pipfd[i][1] != -1)
			close(obj->pipfd[i][1]);
		obj->pipfd[i][0] = -1;
		obj->pipfd[i][1] = -1;
		i++;
	}
	if (obj->fd_in != -1)
		close(obj->fd_in);
	if (obj->fd_out != -1)
		close(obj->fd_out);
	obj->fd_in = -1;
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
	if (obj->pids)
	{
		free(obj->pids);
		obj->pids = NULL;
	}
	obj->exec_cmds = 0;
	obj->cur_pid = 0;
	obj->pipes_count = 0;
}
