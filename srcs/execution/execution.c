/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:04:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 11:31:48 by msavelie         ###   ########.fr       */
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
	if (is_builtin_cmd(cmd->args[0]) == 1)
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
	if (!cmd || !cmd->args || !cmd->args[0])
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
		run_heredoc(obj, temp->left);
		if (obj->heredoc_interrupted == 0)
			execute_cmd(obj, temp->left);
	}
	else
	{
		run_heredoc(obj, temp);
		if (obj->heredoc_interrupted == 0)
			execute_cmd(obj, temp);
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
	while (temp)
	{
		if (obj->heredoc_interrupted || g_signal_received)
			break ;
		execute_ast(obj, temp);
		obj->heredoc_interrupted = 0;
		reset_stdin(obj);
		temp = temp->right;
		obj->cur_pid++;
	}
}
