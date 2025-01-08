/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 12:29:21 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/08 14:54:29 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	is_builtin_cmd(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0 
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "env") == 0)
		return (1);
	return (0);
}

void	exit_child(t_mshell *obj, char *arg, int exit_code)
{
	obj->exit_code = exit_code;
	clean_mshell(obj);
	if (!*arg)
		ft_putstr_fd(": ", 2);
	if (exit_code != 0)
		perror(arg);
	if (errno == EACCES && exit_code != 1)
		exit_code = 126;
	exit(exit_code);
}

static void	run_builtins_execve(char **args, t_mshell *obj)
{
	if (ft_strcmp(args[0], "echo") == 0)
		echo(args);
	else if (ft_strcmp(args[0], "pwd") == 0)
		pwd();
	else if (ft_strcmp(args[0], "env") == 0)
	{
	 	env();
		exit_child(obj, args[0], 0);
	}
	exit_child(obj, args[0], 127);
}

static int	run_bultins(char **args)
{
	if (ft_strcmp(args[0], "cd") == 0)
		return (open_dir(args[1]));
	else if (ft_strcmp(args[0], "export") == 0)
		return (export(args));
	else if (ft_strcmp(args[0], "unset") == 0)
		return (unset(args));
	// else if (ft_strcmp(args[0], "env") == 0)
	// 	return (env());
	return (0);
}

void	alloc_pipes(t_mshell *obj)
{
	t_token *temp;
	int		i;

	if (!obj)
		return ;
	temp = obj->token;
	while (temp)
	{
		if (temp->type == TOKEN_PIPE)
			obj->pipes_count++;
		temp = temp->next;
	}
	obj->pipfd = ft_calloc(obj->pipes_count, sizeof(int *));
	if (!obj->pipfd)
	{
		clean_mshell(obj);
		error_ret(5, NULL);
	}
	i = 0;
	while (i < obj->pipes_count)
	{
		obj->pipfd[i] = ft_calloc(2, sizeof(int));
		if (!obj->pipfd[i])
		{
			clean_mshell(obj);
			error_ret(5, NULL);
		}
		if (pipe(obj->pipfd[i]) == -1)
		{
			clean_mshell(obj);
			error_ret(3, NULL);
		}
		obj->allocated_pipes++;
		i++;
	}
}

void	execute_cmd(t_mshell *obj, t_ast_node *left, t_ast_node *right)
{
	if (!left)
		return ;
	if (run_bultins(left->args) == 1)
		return ;
	obj->exec_cmds++;
	obj->pids[obj->cur_pid] = fork();
	if (obj->pids[obj->cur_pid] == -1)
	{
		clean_mshell(obj);
		return ;
	}
	if (obj->pids[obj->cur_pid] == 0)
	{
		// redirection
		if (left && (left->type == TOKEN_HEREDOC || left->type == TOKEN_REDIRECT_IN))
			redirection_input(obj, left);
		if (obj->allocated_pipes >= 1)
		{
			if (left && left->type == TOKEN_WORD)
				pipe_redirection(obj);
		}
		if (right && (right->type == TOKEN_REDIRECT_APPEND || right->type == TOKEN_REDIRECT_OUT))
			redirection_output(obj, right);
		close_fds(obj);
		// choose cmd (built-ins or common ones)
		// execute
		if (is_builtin_cmd(left->args[0]))
		{
			run_builtins_execve(left->args, obj);
			exit_child(obj, left->args[0], 127);
		}
		else
		{
			obj->cur_path = check_paths_access(obj->paths, left->args, obj);
			execve(obj->cur_path, left->args, obj->paths);
			exit_child(obj, left->args[0], 127);
		}
	}
}

static void	handle_cat_redir(t_ast_node *node, char *redir_file, t_token_type type)
{
	char	**new_args;

	if (ft_strcmp(node->args[0], "cat") != 0)
		return ;
	new_args = ft_calloc(3, sizeof(char *));
	if (!new_args)
		return ; //cleanup
	new_args[0] = ft_strdup(node->args[0]);
	if (type == TOKEN_HEREDOC)
		new_args[1] = ft_strdup(".heredoc_temp");
	else
		new_args[1] = ft_strdup(redir_file);
	new_args[2] = NULL;
	ft_free_strs(node->args, 1);
	node->args = new_args;
}

void	choose_actions(t_mshell *obj)
{
	t_ast_node	*temp;

	if (!obj)
		return ;
	alloc_pipes(obj);
	obj->pids = ft_calloc(obj->allocated_pipes + 1, sizeof(pid_t));
	if (!obj->pids)
	{
		clean_mshell(obj);
		error_ret(5, NULL);
	}
	temp = obj->ast;
	while (temp)
	{
		if (temp->type == TOKEN_WORD)
			execute_cmd(obj, temp, NULL);
		else if (temp->type == TOKEN_PIPE && temp->left &&
			(temp->left->type == TOKEN_HEREDOC || temp->left->type == TOKEN_REDIRECT_IN))
		{
			handle_cat_redir(temp->left->left, temp->left->args[0], temp->left->type);
			if (temp->left->type == TOKEN_HEREDOC)
				handle_here_doc(obj, temp->left);
			execute_cmd(obj, temp->left->left, NULL);
		}
		else if (temp->type == TOKEN_HEREDOC || temp->type == TOKEN_REDIRECT_IN)
		{
			handle_cat_redir(temp->left, temp->args[0], temp->type);
			if (temp->type == TOKEN_HEREDOC)
				handle_here_doc(obj, temp);
			execute_cmd(obj, temp->left, NULL);
		}
		else if (temp->type == TOKEN_REDIRECT_OUT || temp->type == TOKEN_REDIRECT_APPEND)
			execute_cmd(obj, temp->left, temp);
		else
			execute_cmd(obj, temp->left, NULL);
		temp = temp->right;
		obj->cur_pid++;
	}
}
