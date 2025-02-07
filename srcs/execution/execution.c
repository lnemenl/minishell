/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:04:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/06 21:00:04 by rkhakimu         ###   ########.fr       */
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
		//free(obj->envp);
		exit(obj->exit_code);
	}
}

static int	is_builtin_cmd(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0 
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

void	exit_child(t_mshell *obj, char *arg, int exit_code, int is_builtin)
{
	obj->exit_code = exit_code;
	clean_mshell(obj);
	if (!*arg)
		ft_putstr_fd(": ", 2);
	if (obj->exit_code != 0 && is_builtin == 0)
		perror(arg);
	if (errno == EACCES && obj->exit_code != 1)
		obj->exit_code = 126;
	exit(obj->exit_code);
}

static int	run_builtins(char **args, t_mshell *obj)
{
	if (!args || !*args)
		return (0);
	if (ft_strcmp(args[0], "echo") == 0)
		return(echo(args));
	else if (ft_strcmp(args[0], "env") == 0)
	 	return (env(obj));
	else if (ft_strcmp(args[0], "cd") == 0)
		return (cd(args, obj));
	else if (ft_strcmp(args[0], "export") == 0)
		return (export(args, obj));
	else if (ft_strcmp(args[0], "unset") == 0)
		return (unset(args, obj));
	else if (ft_strcmp(args[0], "pwd") == 0)
		return(pwd());
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

static void    execute_cmd(t_mshell *obj, t_ast_node *top_node)
{
    t_ast_node *command_node;
    int         redir_count;
    t_ast_node *current_redir;
    t_ast_node **redir_nodes;
    int         j;

    if (!top_node)
        return ;
    if (obj->allocated_pipes == 0 && obj->redir_check == 0)
    {
        if (run_builtins(top_node->args, obj) == 1)
            return ;
    }
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    obj->exec_cmds++;
    obj->pids[obj->cur_pid] = fork();
    if (obj->pids[obj->cur_pid] == -1)
    {
        clean_mshell(obj);
        return ;
    }
    else if (obj->pids[obj->cur_pid] == 0)
    {
        /* Child process context */
        reset_signals();
        restore_terminal_settings();
        redir_count = 0;
        current_redir = top_node->left;
        while (current_redir)
        {
            if (current_redir->type == TOKEN_REDIRECT_IN ||
                current_redir->type == TOKEN_HEREDOC ||
                current_redir->type == TOKEN_REDIRECT_OUT ||
                current_redir->type == TOKEN_REDIRECT_APPEND)
            {
                redir_count++;
            }
            current_redir = current_redir->left;
        }
        if (redir_count > 0)
        {
            redir_nodes = ft_calloc(redir_count, sizeof(t_ast_node *));
            if (!redir_nodes)
                error_ret(5, NULL);
            j = 0;
            current_redir = top_node->left;
            while (current_redir)
            {
                if (current_redir->type == TOKEN_REDIRECT_IN ||
                    current_redir->type == TOKEN_HEREDOC ||
                    current_redir->type == TOKEN_REDIRECT_OUT ||
                    current_redir->type == TOKEN_REDIRECT_APPEND)
                {
                    redir_nodes[j] = current_redir;
                    j++;
                }
                current_redir = current_redir->left;
            }
            /* Apply all redirections as per Bash behavior (last redirection wins) */
            apply_redirections(obj, redir_nodes, redir_count);
            free(redir_nodes);
        }
        /* After applying redirections, the command node remains in top_node */
        command_node = top_node;
        if (obj->allocated_pipes > 0 && command_node && command_node->type == TOKEN_WORD)
            pipe_redirection(obj);
        if (!command_node || command_node->type != TOKEN_WORD || g_signal_received)
            exit_child(obj, "(no-cmd)", 0, 0);
        close_fds(obj);
        if (is_builtin_cmd(command_node->args[0]))
        {
            run_builtins(command_node->args, obj);
            exit_child(obj, command_node->args[0], obj->exit_code, 1);
        }
        else
        {
            obj->cur_path = check_paths_access(obj->paths, command_node, obj);
            execve(obj->cur_path, command_node->args, obj->paths);
            exit_child(obj, command_node->args[0], 127, 0);
        }
    }
}

static void	check_redirections(t_mshell *obj)
{
	t_token	*temp;

	temp = obj->token;
	obj->redir_check = 0;
	while (temp)
	{
		if (temp->type == TOKEN_HEREDOC
			|| temp->type == TOKEN_REDIRECT_IN
			|| temp->type == TOKEN_REDIRECT_OUT
			|| temp->type == TOKEN_REDIRECT_APPEND)
		{
			obj->redir_check = 1;
			return ;
		}
		temp = temp->next;
	}
}

void choose_actions(t_mshell *obj)
{
    t_ast_node *curr;

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
    curr = obj->ast;
    while (curr && !g_signal_received)
    {
        execute_cmd(obj, curr);
        curr = curr->right;
        obj->cur_pid++;
    }
}
