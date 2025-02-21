/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:04:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 11:05:07 by msavelie         ###   ########.fr       */
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

void execute_cmd(t_mshell *obj, t_ast_node *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return;
	if (obj->allocated_pipes == 0 && obj->redir_check == 0 && run_builtins(cmd->args, obj, cmd->is_quote_heredoc) == 1) 
		return;
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

void choose_actions(t_mshell *obj)
{
	t_ast_node *temp;

	if (!obj)
		return;

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
			break;
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
		obj->heredoc_interrupted = 0;
		if (obj->stdin_fd != -1)
		{
			dup2(obj->stdin_fd, STDIN_FILENO);
			close(obj->stdin_fd);
			obj->stdin_fd = -1;
		}
		temp = temp->right;
		obj->cur_pid++;
	}
}

void	wait_for_children(t_mshell *obj)
{
	int		status;
	pid_t	wpid;

	while (obj->exec_cmds > 0)
	{
		wpid = wait(&status);
		if (wpid == obj->pids[obj->pipes_count])
		{
			if (WIFEXITED(status))
				obj->exit_code = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				if (WTERMSIG(status) == SIGINT)
					write(STDOUT_FILENO, "\n", 1);
				if (WTERMSIG(status) == SIGQUIT)
					ft_putendl_fd("Quit: (core dumped)", STDERR_FILENO);
				obj->exit_code = 128 + WTERMSIG(status);
			}
		}
		obj->exec_cmds--;
	}
}
