/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:04:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/22 16:52:01 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	run_child_process(t_mshell *obj, t_ast_node *cmd)
{
	setup_exec_signals();
	apply_redirections(obj, cmd);
	if (obj->stdin_fd != -1)
		close(obj->stdin_fd);
	if (obj->heredoc_interrupted)
		exit_child(obj, "", 130, 0);
	if (obj->allocated_pipes >= 1)
		pipe_redirection(obj, cmd);
	close_fds(obj);
	if (!cmd->args || !*cmd->args)
		exit_child(obj, *(*cmd->redirs)->args, obj->exit_code, 0);
	else if (is_builtin_cmd(cmd->args[0]) == 1)
	{
		run_builtins(cmd->args, obj, cmd->is_quote_heredoc);
		exit_child(obj, cmd->args[0], obj->exit_code, 1);
	}
	else
	{
		obj->cur_path = check_paths_access(obj->paths, cmd, obj);
		execve(obj->cur_path, cmd->args + obj->args_move, obj->paths);
		exit_child(obj, cmd->args[0], 127, 0);
	}
}

void	execute_cmd(t_mshell *obj, t_ast_node *cmd)
{
	if (!cmd)
		return ;
	if (obj->allocated_pipes == 0 && obj->redir_check == 0
		&& run_builtins(cmd->args, obj, cmd->is_quote_heredoc) == 1)
		return ;
	obj->args_move = 0;
	obj->exec_cmds++;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	obj->pids[obj->cur_pid] = fork();
	if (obj->pids[obj->cur_pid] == -1)
	{
		clean_mshell(obj);
		return ;
	}
	else if (obj->pids[obj->cur_pid] == 0)
		run_child_process(obj, cmd);
}

static void	execute_ast(t_mshell *obj, t_ast_node *temp)
{
	if (temp->left)
	{
		execute_cmd(obj, temp->left);
		if (temp->left->redirs &&(*temp->left->redirs)->type == TOKEN_HEREDOC)
			obj->current_heredoc++;
	}
	else
	{
		execute_cmd(obj, temp);
		if (temp->redirs && (*temp->redirs)->type == TOKEN_HEREDOC)
			obj->current_heredoc++;
	}
}

static void	choose_heredoc_cmd(t_mshell *obj, t_ast_node *node)
{
	if (node->left)
		run_heredoc(obj, node->left);
	else
		run_heredoc(obj, node);
}

static void	alloc_run_heredoc(t_mshell *obj, t_ast_node *node)
{
	t_ast_node	*temp;

	if (!obj || !node)
		return ;
	temp = node;
	obj->heredocs_count = 0;
	obj->current_heredoc = 0;
	while (temp)
	{
		if ((temp->redirs && (*temp->redirs)->type == TOKEN_HEREDOC)
			|| (temp->left && temp->left->redirs && (*temp->left->redirs)->type == TOKEN_HEREDOC))
			obj->heredocs_count++;
		temp = temp->right;
	}
	if (obj->heredocs_count == 0)
		return ;
	obj->heredoc_fds = ft_calloc(obj->heredocs_count, sizeof(int));
	if (!obj->heredoc_fds)
		print_exit("Malloc error\n", NULL, obj);
	while (obj->current_heredoc < obj->heredocs_count)
	{
		obj->heredoc_fds[obj->current_heredoc] = -1;
		obj->current_heredoc++;
	}
	obj->current_heredoc = 0;
	temp = node;
	while (temp && obj->heredoc_interrupted == 0)
	{
		choose_heredoc_cmd(obj, temp);
		temp = temp->right;
	}
}

void	choose_actions(t_mshell *obj)
{
	t_ast_node	*temp;

	if (!obj)
		return ;
	alloc_pipes(obj);
	check_redirections(obj);
	obj->pids = ft_calloc(obj->allocated_pipes + 1, sizeof(pid_t));
	if (!obj->pids)
	{
		clean_mshell(obj);
		error_ret(5, NULL);
	}
	temp = obj->ast;
	alloc_run_heredoc(obj, temp);
	obj->current_heredoc = 0;
	while (temp && obj->heredoc_interrupted == 0)
	{
		if (g_signal_received != 0)
			break ;
		execute_ast(obj, temp);
		//reset_stdin(obj);
		temp = temp->right;
		obj->cur_pid++;
	}
}
