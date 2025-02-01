/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 15:41:46 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/01 16:01:51 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	set_heredoc_strings(char **str_trim_expand,
	t_mshell *obj, struct sigaction *old_int, struct sigaction *old_quit)
{
	str_trim_expand[0] = get_next_line(STDIN_FILENO);
	if (!str_trim_expand[0] || g_signal_received == SIGINT)
    {
        if (g_signal_received == SIGINT)
        {
            close(obj->fd_in);
            unlink(".heredoc_temp");  // Remove temporary file
            obj->is_heredoc = 0;
            // Restore signals before breaking
            sigaction(SIGINT, old_int, NULL);
            sigaction(SIGQUIT, old_quit, NULL);
            return;
        }
      // do break;
    }
	str_trim_expand[2] = expand_env_vars(str_trim_expand[0], obj);
	str_trim_expand[1] = ft_strtrim(str_trim_expand[2], "\n");
}

static void	free_heredoc_strings(char **str_trim_expand)
{
	if (str_trim_expand[0])
	{
		free(str_trim_expand[0]);
		str_trim_expand[0] = NULL;
	}
	if (str_trim_expand[1])
	{
		free(str_trim_expand[1]);
		str_trim_expand[1] = NULL;
	}
	if (str_trim_expand[2])
	{
		free(str_trim_expand[2]);
		str_trim_expand[2] = NULL;
	}
}

void	handle_here_doc(t_mshell *obj, t_ast_node *node)
{
	char	*str_trim_expand[3];
    struct sigaction    old_int, old_quit;
    struct sigaction    sa;

	if (node->type != TOKEN_HEREDOC)
		return ;
  // Save current signal handlers
    sigaction(SIGINT, NULL, &old_int);
    sigaction(SIGQUIT, NULL, &old_quit);

    // Set heredoc-specific signal handling
    sa.sa_handler = handle_signal;  // Use our custom handler
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
  
	obj->fd_in = open(".heredoc_temp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (obj->fd_in < 0)
	{
		clean_mshell(obj);
    // Restore signals before error return
    sigaction(SIGINT, &old_int, NULL);
    sigaction(SIGQUIT, &old_quit, NULL);
		error_ret(6, NULL);
	}
	obj->is_heredoc = 1;  // Set heredoc flag
	set_heredoc_strings(str_trim_expand, obj, &old_int, &old_quit);
	while ((str_trim_expand[1] && ft_strcmp(node->args[0], str_trim_expand[1]) != 0)
		&& str_trim_expand[2] && (ft_strcmp(node->args[0], str_trim_expand[2]) != 0))
	{
		if (str_trim_expand[0][0] == '$')
			ft_putstr_fd(str_trim_expand[2], obj->fd_in);
		else
			ft_putstr_fd(str_trim_expand[0], obj->fd_in);
		free_heredoc_strings(str_trim_expand);
		set_heredoc_strings(str_trim_expand, obj, &old_int, &old_quit);
	}
	free_heredoc_strings(str_trim_expand);
	close(obj->fd_in);
  
   sigaction(SIGINT, &old_int, NULL);
    sigaction(SIGQUIT, &old_quit, NULL);
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
		obj->fd_in = open(".heredoc_temp", O_RDONLY);
			if (obj->fd_in == -1)
				exit_child(obj, ".heredoc_temp", 1, 0);
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
		exit_child(obj, node->args[0], 1, 0);
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