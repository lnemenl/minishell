/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 15:41:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/01 17:53:53 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

typedef struct s_heredoc
{
    char    *str;
    char    *trimmed;
    char    *expanded;
    t_mshell *obj;
}   t_heredoc;

static void    cleanup_heredoc(t_heredoc *doc)
{
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

static int    process_heredoc_line(t_heredoc *doc)
{
    doc->str = get_next_line(STDIN_FILENO);
    if (!doc->str || g_signal_received == SIGINT)
        return (0);
    doc->trimmed = ft_strtrim(doc->str, "\n");
    doc->expanded = expand_env_vars(doc->str, doc->obj);
    return (1);
}

static void    write_heredoc_line(t_heredoc *doc)
{
    if (doc->str[0] == '$')
        ft_putstr_fd(doc->expanded, doc->obj->fd_in);
    else
        ft_putstr_fd(doc->str, doc->obj->fd_in);
}

void    handle_here_doc(t_mshell *obj, t_ast_node *node)
{
    t_heredoc    doc;
    struct sigaction    old_handlers[2];

    save_signal_handlers(&old_handlers[0], &old_handlers[1]);
    if (node->type != TOKEN_HEREDOC)
    {
        restore_signal_handlers(&old_handlers[0], &old_handlers[1]);
        return;
    }
    obj->fd_in = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (obj->fd_in < 0)
    {
        clean_mshell(obj);
        restore_signal_handlers(&old_handlers[0], &old_handlers[1]);
        error_ret(6, NULL);
    }
    doc = (t_heredoc){NULL, NULL, NULL, obj};
    obj->is_heredoc = 1;
    while (process_heredoc_line(&doc))
    {
        if (!ft_strcmp(node->args[0], doc.trimmed) || 
            !ft_strcmp(node->args[0], doc.expanded))
            break;
        write_heredoc_line(&doc);
        cleanup_heredoc(&doc);
    }
    cleanup_heredoc(&doc);
    close(obj->fd_in);
    restore_signal_handlers(&old_handlers[0], &old_handlers[1]);
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