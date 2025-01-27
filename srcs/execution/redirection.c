/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 15:41:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/27 08:31:59 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// ORIGINAL

// void	handle_here_doc(t_mshell *obj, t_ast_node *node)
// {
// 	char	*str;

// 	if (node->type != TOKEN_HEREDOC)
// 		return ;
// 	obj->fd_in = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 	if (obj->fd_in < 0)
// 	{
// 		clean_mshell(obj);
// 		error_ret(6, NULL);
// 	}
// 	str = get_next_line(STDIN_FILENO);
// 	while (str && ft_strncmp(str, node->args[0], ft_strlen(str) - 1) != 0)
// 	{
// 		ft_putstr_fd(str, obj->fd_in);
// 		free(str);
// 		str = get_next_line(STDIN_FILENO);
// 	}
// 	if (str)
// 		free(str);
// 	close(obj->fd_in);
// }

/* ONE WAY leaving get_next_line
void    handle_here_doc(t_mshell *obj, t_ast_node *node)
{
    char    *str;

    if (node->type != TOKEN_HEREDOC)
        return ;
    obj->fd_in = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (obj->fd_in < 0)
    {
        clean_mshell(obj);
        error_ret(6, NULL);
    }
    write(STDOUT_FILENO, "> ", 2);
    while ((str = get_next_line(STDIN_FILENO)))
    {
        if (g_signo == SIGINT)
        {
            free(str);
            close(obj->fd_in);
            unlink(".heredoc_temp");
            return ;
        }
        if (ft_strncmp(str, node->args[0], ft_strlen(str) - 1) == 0)
        {
            free(str);
            break;
        }
        ft_putstr_fd(str, obj->fd_in);
        free(str);
        if (g_signo != SIGINT)
            write(STDOUT_FILENO, "> ", 2);
    }
    close(obj->fd_in);
}
*/

void    handle_here_doc(t_mshell *obj, t_ast_node *node)
{
    char    *line;
    struct sigaction    sa_int;

    if (node->type != TOKEN_HEREDOC)
        return;
    
    // Setup heredoc-specific signal handling
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0;  // No SA_RESTART to allow readline interruption
    sigaction(SIGINT, &sa_int, NULL);

    obj->is_heredoc = 1;
    obj->fd_in = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (obj->fd_in < 0)
    {
        clean_mshell(obj);
        error_ret(6, NULL);
    }

    while (1)
    {
        line = readline("> ");
        if (!line || g_signo == SIGINT)  // Handle EOF or interrupt
        {
            if (g_signo == SIGINT)
            {
                free(line);
                close(obj->fd_in);
                unlink(".heredoc_temp");
                obj->is_heredoc = 0;
                g_signo = 0;
                return;
            }
            break;  // EOF case
        }
        if (!ft_strcmp(line, node->args[0]))  // Exact delimiter match
        {
            free(line);
            break;
        }
        ft_putstr_fd(line, obj->fd_in);
        ft_putstr_fd("\n", obj->fd_in);
        free(line);
    }

    obj->is_heredoc = 0;
    close(obj->fd_in);
    setup_shell_signals(obj);
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
