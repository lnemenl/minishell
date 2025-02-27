/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 13:04:41 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/27 12:21:23 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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
		doc->expanded = expand_env_vars(doc->str, doc->obj, 0);
	else
		doc->expanded = ft_strdup(doc->str);
	doc->trimmed = ft_strtrim(doc->expanded, "\n");
	return (1);
}

static int	handle_multiple_heredocs(t_mshell *obj, t_ast_node *node,
	int *is_last_heredoc, int *last_fd)
{
	int	i;

	i = 0;
	while (node->redirs[i])
	{
		if (*last_fd != -1
			&& (node->redirs[i]->type == TOKEN_REDIRECT_IN
				|| node->redirs[i]->type == TOKEN_HEREDOC))
		{
			close(*last_fd);
			*last_fd = -1;
		}
		if (node->redirs[i]->type == TOKEN_REDIRECT_IN)
			*is_last_heredoc = 0;
		else if (node->redirs[i]->type == TOKEN_HEREDOC)
			*is_last_heredoc = 1;
		*last_fd = handle_here_doc(obj, node->redirs[i], *last_fd);
		i++;
	}
	return (i);
}

void	run_heredoc(t_mshell *obj, t_ast_node *node)
{
	int	last_fd;
	int	is_last_heredoc;
	int	i;

	if (!node || !node->redirs || !*node->redirs)
		return ;
	is_last_heredoc = 0;
	last_fd = -1;
	i = handle_multiple_heredocs(obj, node, &is_last_heredoc, &last_fd);
	if (last_fd != -1 && i > 0 && is_last_heredoc == 1)
	{
		obj->heredoc_fds[obj->current_heredoc] = last_fd;
		obj->current_heredoc++;
	}
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
		if (ft_strcmp(node->args[0], heredoc.trimmed) == 0
			|| ft_strcmp(node->args[0], heredoc.expanded) == 0)
			break ;
		write_heredoc_line(&heredoc);
		cleanup_heredoc(&heredoc);
	}
	cleanup_heredoc(&heredoc);
	close(heredoc.pipe_fd[1]);
	ret_fd = heredoc.pipe_fd[0];
	ret_fd = handle_heredoc_sigint(obj, ret_fd, &heredoc);
	if (isatty(STDIN_FILENO))
		transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
	return (ret_fd);
}
