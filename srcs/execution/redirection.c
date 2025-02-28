/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 15:41:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/28 15:54:11 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	has_redirection(t_ast_node *cmd, int redir_in, int redir_out)
{
	int	i;

	if (!cmd->redirs)
		return (0);
	i = 0;
	while (cmd->redirs[i])
	{
		if (redir_out == 1)
		{
			if (cmd->redirs[i]->type == TOKEN_REDIRECT_OUT
				|| cmd->redirs[i]->type == TOKEN_REDIRECT_APPEND)
				return (1);
		}
		else if (redir_in == 1)
		{
			if (cmd->redirs[i]->type == TOKEN_REDIRECT_IN
				|| cmd->redirs[i]->type == TOKEN_HEREDOC)
				return (1);
		}
		i++;
	}
	return (0);
}

void	redirection_input(t_mshell *obj, t_ast_node *node)
{
	if (node->type == TOKEN_REDIRECT_IN)
	{
		if (access(node->args[0], F_OK) != 0)
			exit_child(obj, node->args[0], 1, 0);
		obj->fd_in = open(node->args[0], O_RDONLY);
		if (obj->fd_in == -1)
			exit_child(obj, node->args[0], 1, 0);
	}
	else
	{
		if (obj->heredoc_interrupted || !obj->heredoc_fds)
			return ;
		obj->fd_in = obj->heredoc_fds[obj->current_heredoc];
		obj->heredoc_fds[obj->current_heredoc] = -1;
	}
	if (obj->fd_in > 0)
	{
		dup2(obj->fd_in, STDIN_FILENO);
		close(obj->fd_in);
	}
	obj->fd_in = -1;
}

void	redirection_output(t_mshell *obj, t_ast_node *node)
{
	if (node->type == TOKEN_REDIRECT_APPEND)
		obj->fd_out = open(node->args[0],
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		obj->fd_out = open(node->args[0],
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (obj->fd_out == -1)
		exit_child(obj, node->args[0], 1, 0);
	dup2(obj->fd_out, STDOUT_FILENO);
	close(obj->fd_out);
	obj->fd_out = -1;
}

static void	handle_middle_redirs(t_mshell *obj, t_ast_node *cmd)
{
	if (!has_redirection(cmd, 1, 0))
	{
		dup2(obj->pipfd[obj->cur_pid - 1][0], STDIN_FILENO);
		close(obj->pipfd[obj->cur_pid - 1][0]);
		obj->pipfd[obj->cur_pid - 1][0] = -1;
	}
	if (!has_redirection(cmd, 0, 1))
	{
		dup2(obj->pipfd[obj->cur_pid][1], STDOUT_FILENO);
		close(obj->pipfd[obj->cur_pid][1]);
		obj->pipfd[obj->cur_pid][1] = -1;
	}
}

void	pipe_redirection(t_mshell *obj, t_ast_node *cmd)
{
	if (obj->cur_pid == 0)
	{
		if (!has_redirection(cmd, 0, 1))
		{
			dup2(obj->pipfd[obj->cur_pid][1], STDOUT_FILENO);
			close(obj->pipfd[obj->cur_pid][1]);
			obj->pipfd[obj->cur_pid][1] = -1;
		}
	}
	else if (obj->cur_pid == obj->allocated_pipes)
	{
		if (!has_redirection(cmd, 1, 0))
		{
			dup2(obj->pipfd[obj->cur_pid - 1][0], STDIN_FILENO);
			close(obj->pipfd[obj->cur_pid - 1][0]);
			obj->pipfd[obj->cur_pid - 1][0] = -1;
		}
	}
	else
		handle_middle_redirs(obj, cmd);
}
