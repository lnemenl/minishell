/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 11:06:53 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 11:07:29 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	write_heredoc_line(t_heredoc *doc)
{
	if (doc->str[0] == '$')
		ft_fprintf(doc->pipe_fd[1], "%s", doc->expanded);
	else
		ft_fprintf(doc->pipe_fd[1], "%s", doc->str);
}

t_heredoc	init_heredoc(t_mshell *obj)
{
	t_heredoc	heredoc_obj;

	heredoc_obj.str = NULL;
	heredoc_obj.trimmed = NULL;
	heredoc_obj.expanded = NULL;
	heredoc_obj.obj = obj;
	heredoc_obj.pipe_fd[0] = -1;
	heredoc_obj.pipe_fd[1] = -1;
	if (pipe(heredoc_obj.pipe_fd) == -1)
	{
		obj->exit_code = 1;
		print_exit("Pipe error\n", NULL, obj);
	}
	return (heredoc_obj);
}

void	cleanup_heredoc(t_heredoc *doc)
{
	if (!doc)
		return ;
	if (doc->str)
		free(doc->str);
	doc->str = NULL;
	if (doc->trimmed)
		free(doc->trimmed);
	doc->trimmed = NULL;
	if (doc->expanded)
		free(doc->expanded);
	doc->expanded = NULL;
}
