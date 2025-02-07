/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 15:41:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/06 20:40:33 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

typedef struct s_heredoc
{
    char    *line;
    char    *expanded;
    char    *trimmed;
    t_mshell *obj;
}   t_heredoc;

static void    cleanup_heredoc(t_heredoc *doc)
{
    if (doc->line)
        free(doc->line);
    if (doc->expanded)
        free(doc->expanded);
    if (doc->trimmed)
        free(doc->trimmed);
    doc->line = NULL;
    doc->expanded = NULL;
    doc->trimmed = NULL;
}

static int    process_heredoc_line(t_heredoc *doc)
{
    doc->line = get_next_line(STDIN_FILENO);
    if (!doc->line || g_signal_received == SIGINT)
        return (0);
    doc->expanded = expand_env_vars(doc->line, doc->obj);
    doc->trimmed = ft_strtrim(doc->expanded, "\n");
    return (1);
}

void    handle_here_doc(t_mshell *obj, t_ast_node *node)
{
    t_heredoc   doc;
    int         fd;
    
    if (node->type != TOKEN_HEREDOC)
        return;
    fd = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        clean_mshell(obj);
        error_ret(6, NULL);
    }
    obj->is_heredoc = 1;
    doc.obj = obj;
    doc.line = NULL;
    doc.expanded = NULL;
    doc.trimmed = NULL;
    while (process_heredoc_line(&doc))
    {
        /* If line equals the delimiter (checking both raw and expanded), stop */
        if (!ft_strcmp(node->args[0], doc.trimmed) ||
            !ft_strcmp(node->args[0], doc.expanded))
        {
            cleanup_heredoc(&doc);
            break;
        }
        ft_putstr_fd(doc.line, fd);
        cleanup_heredoc(&doc);
    }
    close(fd);
}


void pipe_redirection(t_mshell *obj)
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

void    apply_redirections(t_mshell *obj, t_ast_node **redirs, int count)
{
    int     fd_in = -1;
    int     fd_out = -1;
    int     i;
    int     tmp_fd;

    i = 0;
    while (i < count)
    {
        if (redirs[i]->type == TOKEN_REDIRECT_IN)
        {
            if (access(redirs[i]->args[0], F_OK) != 0)
                exit_child(obj, redirs[i]->args[0], 1, 0);
            tmp_fd = open(redirs[i]->args[0], O_RDONLY);
            if (tmp_fd == -1)
                exit_child(obj, redirs[i]->args[0], 1, 0);
            if (fd_in != -1)
                close(fd_in);
            fd_in = tmp_fd;
        }
        else if (redirs[i]->type == TOKEN_HEREDOC)
        {
            /* Build the heredoc content into .heredoc_temp */
            handle_here_doc(obj, redirs[i]);
            tmp_fd = open(".heredoc_temp", O_RDONLY);
            if (tmp_fd == -1)
                exit_child(obj, ".heredoc_temp", 1, 0);
            if (fd_in != -1)
                close(fd_in);
            fd_in = tmp_fd;
        }
        else if (redirs[i]->type == TOKEN_REDIRECT_OUT)
        {
            tmp_fd = open(redirs[i]->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (tmp_fd == -1)
                exit_child(obj, redirs[i]->args[0], 1, 0);
            if (fd_out != -1)
                close(fd_out);
            fd_out = tmp_fd;
        }
        else if (redirs[i]->type == TOKEN_REDIRECT_APPEND)
        {
            tmp_fd = open(redirs[i]->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (tmp_fd == -1)
                exit_child(obj, redirs[i]->args[0], 1, 0);
            if (fd_out != -1)
                close(fd_out);
            fd_out = tmp_fd;
        }
        i++;
    }
    if (fd_in != -1)
    {
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
    if (fd_out != -1)
    {
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
}