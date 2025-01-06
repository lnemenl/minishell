/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 15:41:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/06 14:57:07 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	handle_here_doc(t_mshell *obj, t_ast_node *node)
{
	char	*str;

	printf("args in heredoc: %s\n", node->args[0]);
	if (node->type != TOKEN_HEREDOC)
		return ;
	obj->fd_in = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (obj->fd_in < 0)
	{
		clean_mshell(obj);
		error_ret(6, NULL);
	}
	str = get_next_line(STDIN_FILENO);
	while (str && ft_strncmp(str, node->args[0], ft_strlen(str) - 1) != 0)
	{
		ft_putstr_fd(str, obj->fd_in);
		free(str);
		str = get_next_line(STDIN_FILENO);
	}
	if (str)
		free(str);
	close(obj->fd_in);
}

void	redirection_input(t_mshell *obj, t_ast_node *node)
{
	if (node->type == TOKEN_REDIRECT_IN)
	{
		if (access(node->args[0], F_OK) != 0)
			exit_child(obj, node->args[0], 1);
		obj->fd_in = open(node->args[0], O_RDONLY);
		if (obj->fd_in == -1)
			exit_child(obj, node->args[0], 1);
	}
	else
	{
		obj->fd_in = open(".heredoc_temp", O_RDONLY);
			if (obj->fd_in == -1)
				exit_child(obj, ".heredoc_temp", 1);
	}
	dup2(obj->fd_in, STDIN_FILENO);
	close(obj->fd_in);
}

void	redirection_output(t_mshell *obj, t_ast_node *node)
{
	printf("node-type: %d\n", node->type);
	if (node->type == TOKEN_REDIRECT_APPEND)
		obj->fd_out = open(node->args[0],
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		obj->fd_out = open(node->args[0],
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (obj->fd_out == -1)
		exit_child(obj, node->args[0], 1);
	dup2(obj->fd_out, STDOUT_FILENO);
	close(obj->fd_out);
}

void	pipe_redirection(t_mshell *obj)
{
	if (obj->cur_pid == 0)
	{
		dup2(obj->pipfd[obj->cur_pid][1], STDOUT_FILENO);
		close(obj->pipfd[obj->cur_pid][1]);
	}
	else if (obj->cur_pid == obj->allocated_pipes)
	{
		dup2(obj->pipfd[obj->cur_pid - 1][0], STDIN_FILENO);
		close(obj->pipfd[obj->cur_pid - 1][0]);
	}
	else
	{
		dup2(obj->pipfd[obj->cur_pid - 1][0], STDIN_FILENO);
		close(obj->pipfd[obj->cur_pid - 1][0]);
		dup2(obj->pipfd[obj->cur_pid][1], STDOUT_FILENO);
		close(obj->pipfd[obj->cur_pid][1]);
	}
}
