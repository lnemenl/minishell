/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 13:04:41 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/13 12:12:54 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static t_heredoc	*init_heredoc(t_mshell *obj)
{
	t_heredoc	*heredoc_obj;

	heredoc_obj = ft_calloc(1, sizeof(t_heredoc));
	if (!heredoc_obj)
	{
		clean_mshell(obj);
		error_ret(5, NULL);
	}
	heredoc_obj->str = NULL;
	heredoc_obj->trimmed = NULL;
	heredoc_obj->expanded = NULL;
	heredoc_obj->obj = obj;
	heredoc_obj->pipe_fd[0] = -1;
	heredoc_obj->pipe_fd[1] = -1;
	if (pipe(heredoc_obj->pipe_fd) == -1)
	{
		clean_mshell(obj);
		error_ret(3, NULL);
	}
	return (heredoc_obj);
}

static void	cleanup_heredoc(t_heredoc *doc)
{
	if (!doc)
		return ;
	if (doc->str)
		free(doc->str);
	if (doc->trimmed)
		free(doc->trimmed);
	if (doc->expanded)
		free(doc->expanded);
	doc->str = NULL;
	doc->trimmed = NULL;
	doc->expanded = NULL;
}

static int	process_heredoc_line(t_heredoc *doc)
{
	write(STDOUT_FILENO, "> ", 2);
	doc->str = get_next_line(STDIN_FILENO);
	if (!doc->str || g_signal_received == SIGINT)
		return (0);
	doc->expanded = expand_env_vars(doc->str, doc->obj);
	doc->trimmed = ft_strtrim(doc->expanded, "\n");
	return (1);
}

static void	write_heredoc_line(t_heredoc *doc)
{
	if (doc->str[0] == '$')
		ft_fprintf(doc->pipe_fd[1], "%s", doc->expanded);
	else
		ft_fprintf(doc->pipe_fd[1], "%s", doc->str);
}

void	handle_here_doc(t_mshell *obj, t_ast_node *node)
{
	if (node->type != TOKEN_HEREDOC)
		return;
	obj->heredoc = init_heredoc(obj);
	obj->stdin_fd = dup(STDIN_FILENO);
	g_signal_received = 0;
	while (process_heredoc_line(obj->heredoc))
	{
		if (!ft_strcmp(node->args[0], obj->heredoc->trimmed) || 
			!ft_strcmp(node->args[0], obj->heredoc->expanded))
			break;
		write_heredoc_line(obj->heredoc);
		cleanup_heredoc(obj->heredoc);
	}
	cleanup_heredoc(obj->heredoc);
	dup2(obj->heredoc->pipe_fd[0], STDIN_FILENO);
	close(obj->heredoc->pipe_fd[0]);
	close(obj->heredoc->pipe_fd[1]);
	if (g_signal_received == SIGINT)
	{
		obj->heredoc_interrupted = 1;
		return ;
	}
	free(obj->heredoc);
    obj->heredoc = NULL;
}
