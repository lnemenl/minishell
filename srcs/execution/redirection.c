/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 15:41:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/30 14:01:00 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	set_heredoc_strings(char **str, char **trimmed_str,
	char **expanded_str, t_mshell *obj)
{
	*str = get_next_line(STDIN_FILENO);
	*trimmed_str = ft_strtrim(*str, "\n");
	*expanded_str = expand_env_vars(*str, obj);
}

static void	free_heredoc_strings(char **str, char **trimmed_str,
	char **expanded_str)
{
	if (*str)
	{
		free(*str);
		*str = NULL;
	}
	if (*trimmed_str)
	{
		free(*trimmed_str);
		*trimmed_str = NULL;
	}
	if (*expanded_str)
	{
		free(*expanded_str);
		*expanded_str = NULL;
	}
}

void	handle_here_doc(t_mshell *obj, t_ast_node *node)
{
	char	*str;
	char	*trimmed_str;
	char	*expanded_str;

	if (node->type != TOKEN_HEREDOC)
		return ;
	obj->fd_in = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (obj->fd_in < 0)
	{
		clean_mshell(obj);
		error_ret(6, NULL);
	}
	set_heredoc_strings(&str, &trimmed_str, &expanded_str, obj);
	while ((trimmed_str && ft_strcmp(node->args[0], trimmed_str) != 0)
		&& expanded_str && (ft_strcmp(node->args[0], expanded_str) != 0))
	{
		if (str[0] == '$')
			ft_putstr_fd(expanded_str, obj->fd_in);
		else
			ft_putstr_fd(str, obj->fd_in);
		free_heredoc_strings(&str, &trimmed_str, &expanded_str);
		set_heredoc_strings(&str, &trimmed_str, &expanded_str, obj);
	}
	free_heredoc_strings(&str, &trimmed_str, &expanded_str);
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
