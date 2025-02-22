/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_fds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 17:22:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/22 17:22:53 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	close_heredoc_fds(t_mshell *obj)
{
	int	i;

	if (!obj || !obj->heredoc_fds)
		return ;
	i = 0;
	while (i < obj->heredocs_count)
	{
		if (obj->heredoc_fds[i] != -1)
		{
			close(obj->heredoc_fds[i]);
			obj->heredoc_fds[i] = -1;
		}
		i++;
	}
	free(obj->heredoc_fds);
	obj->heredoc_fds = NULL;
	obj->heredocs_count = 0;
	obj->current_heredoc = 0;
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
	close_heredoc_fds(obj);
}
