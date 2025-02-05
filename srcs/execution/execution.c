/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:04:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/05 21:03:31 by rkhakimu         ###   ########.fr       */
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

// static void	handle_cat_redir(t_ast_node *node, char *redir_file, t_token_type type)
// {
// 	char	**new_args;

// 	if (ft_strcmp(node->args[0], "cat") != 0)
// 		return ;
// 	new_args = ft_calloc(3, sizeof(char *));
// 	if (!new_args)
// 		return ; //cleanup
// 	new_args[0] = ft_strdup(node->args[0]);
// 	if (type == TOKEN_HEREDOC)
// 		new_args[1] = ft_strdup(".heredoc_temp");
// 	else
// 		new_args[1] = ft_strdup(redir_file);
// 	new_args[2] = NULL;
// 	ft_free_strs(node->args, 1);
// 	node->args = new_args;
// }


static void	handle_cat_redir(t_ast_node *node, char *redir_file, t_token_type type)
{
	int		old_count;
	char	**new_args;

	if (ft_strcmp(node->args[0], "cat") != 0)
		return ;
	old_count = 0;
	while (node->args[old_count])
		old_count++;
	/* Only add the redir_file if user typed just "cat" alone. */
	if (old_count == 1)
	{
		new_args = ft_calloc(3, sizeof(char *));
		if (!new_args)
			return ; /* Could add cleanup if desired. */
		new_args[0] = ft_strdup("cat");
		if (type == TOKEN_HEREDOC)
			new_args[1] = ft_strdup(".heredoc_temp");
		else
			new_args[1] = ft_strdup(redir_file);
		new_args[2] = NULL;
		ft_free_strs(node->args, 1);
		node->args = new_args;
	}
	/* If old_count > 1, do nothing; let cat keep its user-supplied args. */
}


static t_ast_node *apply_all_redirections(t_mshell *obj, t_ast_node *node)
{
    t_ast_node *current = node;

    while (current && (current->type == TOKEN_REDIRECT_IN
        || current->type == TOKEN_HEREDOC
        || current->type == TOKEN_REDIRECT_OUT
        || current->type == TOKEN_REDIRECT_APPEND))
    {
        if (current->type == TOKEN_HEREDOC)
        {
            /* Force 'cat' argument if needed. */
            handle_cat_redir(current->left, current->args[0], current->type);

            /* Actually create and open the heredoc temp file. */
            handle_here_doc(obj, current);

            /* Then treat it as input redirection from .heredoc_temp. */
            redirection_input(obj, current);
        }
        else if (current->type == TOKEN_REDIRECT_IN)
        {
            handle_cat_redir(current->left, current->args[0], current->type);
            redirection_input(obj, current);
        }
        else if (current->type == TOKEN_REDIRECT_OUT
            || current->type == TOKEN_REDIRECT_APPEND)
        {
            redirection_output(obj, current);
        }
        current = current->left;
    }
    return current; /* Should be TOKEN_WORD or NULL. */
}

static void execute_cmd(t_mshell *obj, t_ast_node *top_node)
{
    t_ast_node *command_node;

    if (!top_node)
        return;
    if (obj->allocated_pipes == 0 && obj->redir_check == 0)
    {
        if (run_builtins(top_node->args, obj) == 1)
            return;
    }
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    obj->args_move = 0;
    obj->exec_cmds++;
    obj->pids[obj->cur_pid] = fork();
    if (obj->pids[obj->cur_pid] == -1)
    {
        clean_mshell(obj);
        return;
    }
    else if (obj->pids[obj->cur_pid] == 0)
    {
        /* Child process context */
        reset_signals();
        restore_terminal_settings(); 
        command_node = apply_all_redirections(obj, top_node);
        if (obj->allocated_pipes > 0 && command_node && command_node->type == TOKEN_WORD)
            pipe_redirection(obj);
        if (!command_node || command_node->type != TOKEN_WORD || g_signal_received)
            exit_child(obj, "(no-cmd)", 0, 0);

        close_fds(obj);

        /* Now, either run builtin or execve. */
        if (is_builtin_cmd(command_node->args[0]))
        {
            run_builtins(command_node->args, obj);
            exit_child(obj, command_node->args[0], obj->exit_code, 1);
        }
        else
        {
            obj->cur_path = check_paths_access(obj->paths, command_node, obj);
            execve(obj->cur_path, command_node->args + obj->args_move, obj->paths);
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

void    choose_actions(t_mshell *obj)
{
	t_ast_node    *temp;

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
		if (g_signal_received)
			break;

		if (temp->type == TOKEN_WORD && !temp->right)
			execute_cmd(obj, temp);
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
			execute_cmd(obj, temp->left->left);
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
			execute_cmd(obj, temp->left);
		}
		else if (temp->type == TOKEN_REDIRECT_OUT || temp->type == TOKEN_REDIRECT_APPEND)
		{
			if (temp->left && (temp->left->type == TOKEN_HEREDOC
				|| temp->left->type == TOKEN_REDIRECT_IN))
			{
				handle_cat_redir(temp->left->left, temp->left->args[0], temp->left->type);
				if (temp->left->type == TOKEN_HEREDOC)
					handle_here_doc(obj, temp->left);
				temp->left = temp->left->left;
			}
			execute_cmd(obj, temp->left);
		}
		else
			execute_cmd(obj, temp->left);

		temp = temp->right;
		obj->cur_pid++;
	}
}
