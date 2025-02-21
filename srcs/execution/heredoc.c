/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 13:04:41 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 11:46:17 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static t_heredoc	init_heredoc(t_mshell *obj)
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
		// clean_mshell(obj);
		error_ret(3, NULL);
		obj->exit_code = 1;
		clean_exit(obj);
	}
	return (heredoc_obj);
}

static void	cleanup_heredoc(t_heredoc *doc)
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

static int	process_heredoc_line(t_heredoc *doc, int is_heredoc_quoted)
{
	write(STDOUT_FILENO, "> ", 2);
	doc->str = get_next_line(STDIN_FILENO);
	if (!doc->str || g_signal_received == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		return (0);
	}
	if (is_heredoc_quoted == 0)
		doc->expanded = expand_env_vars(doc->str, doc->obj);
	else
		doc->expanded = ft_strdup(doc->str);
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

int	handle_here_doc(t_mshell *obj, t_ast_node *node, int last_fd)
{
	int			ret_fd;
	t_heredoc	heredoc;

	if (node->type != TOKEN_HEREDOC)
		return (last_fd);
	if (g_signal_received == SIGINT)
		return (last_fd);
	heredoc = init_heredoc(obj);
	transition_signal_handlers(SIGNAL_STATE_HEREDOC);
	while (process_heredoc_line(&heredoc, node->is_quote_heredoc))
	{
		if (!ft_strcmp(node->args[0], heredoc.trimmed) || 
			!ft_strcmp(node->args[0], heredoc.expanded))
			break;
		write_heredoc_line(&heredoc);
		cleanup_heredoc(&heredoc);
	}
	cleanup_heredoc(&heredoc);
	close(heredoc.pipe_fd[1]);
	ret_fd = heredoc.pipe_fd[0];
	if (g_signal_received == SIGINT)
	{
		obj->heredoc_interrupted = 1;
		close(heredoc.pipe_fd[0]);
		ret_fd = -1;
		return (ret_fd);
	}
	if (isatty(STDIN_FILENO))
		transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
	return (ret_fd);
}
