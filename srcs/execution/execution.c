/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:04:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/01 17:54:31 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	check_and_handle_exit(char **args, t_mshell *obj)
{
	int	i;
	int	args_len;

	if (!args || !*args)
		return ;
	args_len = 0;
	while (args[args_len])
		args_len++;
	if (args_len == 1)
	{
		printf("exit\n");
		clean_mshell(obj);
		free(obj->envp);
		exit(obj->exit_code);
	}
	else if (args_len >= 2)
	{
		printf("exit\n");
		i = 0;
		while (args[1][i])
		{
			if (ft_isdigit(args[1][i]) == 0 && args[1][i] != '-' && args[1][i] != '+')
			{
				obj->exit_code = 2;
				ft_fprintf(2, "minishell: exit: %s: numeric argument required\n", args[1]);
				clean_mshell(obj);
				free(obj->envp);
				exit(obj->exit_code);
			}
			i++;
		}
		if (args_len > 2)
		{
			obj->exit_code = 1;
			ft_fprintf(2, "minishell: exit: too many arguments\n");
			clean_mshell(obj);
			free(obj->envp);
			exit(obj->exit_code);
		}
		if (obj->exit_code == 0)
		{
			obj->exit_code = ft_atoi(args[1]);
			if (obj->exit_code < 0)
				obj->exit_code = 156;
		}
		clean_mshell(obj);
		free(obj->envp);
		exit(obj->exit_code);
	}
}

static int	is_builtin_cmd(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0 
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
	if (obj->exit_code != 0)
		perror(arg);
	if (errno == EACCES && obj->exit_code != 1)
		obj->exit_code = 126;
	exit(obj->exit_code);
}

static void	run_builtins_exec(char **args, t_mshell *obj)
{
	if (ft_strcmp(args[0], "echo") == 0)
		echo(args);
	else if (ft_strcmp(args[0], "env") == 0)
	 	env(obj);
	exit_child(obj, args[0], 0);
}

static int	run_bultins(char **args, t_mshell *obj)
{
	if (!args || !*args)
		return (0);
	if (ft_strcmp(args[0], "cd") == 0)
		return (cd(args, obj));
	else if (ft_strcmp(args[0], "export") == 0)
		return (export(args, obj));
	else if (ft_strcmp(args[0], "unset") == 0)
		return (unset(args, obj));
	else if (ft_strcmp(args[0], "pwd") == 0)
	{
		pwd();
		return (1);
	}
	else if (ft_strcmp(args[0], "exit") == 0)
	{
		check_and_handle_exit(args, obj);
		return (1);
	}
	return (0);
}

void	alloc_pipes(t_mshell *obj)
{
	int		i;

	if (!obj)
		return ;
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

void execute_cmd(t_mshell *obj, t_ast_node *left, t_ast_node *right)
{
    if (!left)
        return;
    if (run_bultins(left->args, obj) == 1)
        return;

    obj->exec_cmds++;
    obj->pids[obj->cur_pid] = fork();
    if (obj->pids[obj->cur_pid] == -1)
    {
        clean_mshell(obj);
        return;
    }
    if (obj->pids[obj->cur_pid] == 0)
    {
        transition_signal_handlers(SIGNAL_STATE_RESET);

        // Handle all input redirections first
        t_ast_node *temp = left;
        while (temp)
        {
            if (temp->type == TOKEN_HEREDOC || temp->type == TOKEN_REDIRECT_IN)
                redirection_input(obj, temp);
            temp = temp->left;
        }

        // Handle pipe redirection
        if (obj->allocated_pipes >= 1 && left->type == TOKEN_WORD)
            pipe_redirection(obj);

        // Handle output redirections
        if (right && (right->type == TOKEN_REDIRECT_APPEND || right->type == TOKEN_REDIRECT_OUT))
            redirection_output(obj, right);

        close_fds(obj);

        // Execute command
        if (is_builtin_cmd(left->args[0]) == 1)
            run_builtins_exec(left->args, obj);
        else
        {
            obj->cur_path = check_paths_access(obj->paths, left, obj);
            execve(obj->cur_path, left->args, obj->envp);
            exit_child(obj, left->args[0], 127);
        }
    }
    else
        transition_signal_handlers(SIGNAL_STATE_EXEC);
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

void    choose_actions(t_mshell *obj)
{
    t_ast_node    *temp;
    struct sigaction    old_handlers[2];

    save_signal_handlers(&old_handlers[0], &old_handlers[1]);
    if (!obj)
        return;
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
        if (g_signal_received)
            break;

        if (temp->type == TOKEN_WORD)
            execute_cmd(obj, temp, NULL);
        else if (temp->type == TOKEN_PIPE && temp->left &&
            (temp->left->type == TOKEN_HEREDOC || temp->left->type == TOKEN_REDIRECT_IN))
        {
            handle_cat_redir(temp->left->left, temp->left->args[0], temp->left->type);
            if (temp->left->type == TOKEN_HEREDOC)
            {
                transition_signal_handlers(SIGNAL_STATE_HEREDOC);
                handle_here_doc(obj, temp->left);
                transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
            }
            execute_cmd(obj, temp->left->left, NULL);
        }
        else if (temp->type == TOKEN_HEREDOC || temp->type == TOKEN_REDIRECT_IN)
        {
            handle_cat_redir(temp->left, temp->args[0], temp->type);
            if (temp->type == TOKEN_HEREDOC)
            {
                transition_signal_handlers(SIGNAL_STATE_HEREDOC);
                handle_here_doc(obj, temp);
                transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
            }
            execute_cmd(obj, temp->left, NULL);
        }
        else if (temp->type == TOKEN_REDIRECT_OUT || 
                 temp->type == TOKEN_REDIRECT_APPEND)
            execute_cmd(obj, temp->left, temp);
        else
            execute_cmd(obj, temp->left, NULL);

        temp = temp->right;
        obj->cur_pid++;
    }

    restore_signal_handlers(&old_handlers[0], &old_handlers[1]);
}
