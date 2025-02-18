/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:03:23 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/17 17:23:47 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static char	**copy_envp(char **envp)
{
	char	**new_envp;
	size_t	envp_len;

	if (!envp)
		return (NULL);
	envp_len = get_envp_length(envp);
	new_envp = ft_calloc(envp_len + 1, sizeof(char *));
	if (!new_envp)
		return (NULL);
	envp_len = 0;
	while (envp[envp_len])
	{
		new_envp[envp_len] = ft_strdup(envp[envp_len]);
		if (!new_envp[envp_len])
		{
			ft_free_strs(new_envp, envp_len);
			return (NULL);
		}
		envp_len++;
	}
	new_envp[envp_len] = NULL;
	return (new_envp);
}

static t_mshell	init_shell(char **argv, char **envp)
{
	t_mshell	obj;

	obj.allocated_pipes = 0;
	obj.cmd_line = NULL;
	obj.cur_path = NULL;
	obj.pipfd = NULL;
	obj.exec_cmds = 0;
	obj.envp = copy_envp(envp);
	obj.paths = fetch_paths(obj.envp);
	obj.pipes_count = 0;
	obj.token = NULL;
	obj.pids = NULL;
	obj.cur_pid = 0;
	obj.fd_in = -1;
	obj.fd_out = -1;
	obj.exit_code = 0;
	obj.args_move = 0;
	obj.redir_check = 0;
	obj.heredoc_interrupted = 0;
	obj.stdin_fd = -1;
	obj.prev_path = get_env_var(obj.envp, "HOME");
	(void) argv;
	return (obj);
}

static void wait_for_children(t_mshell *obj)
{
	int   status;
	pid_t wpid;

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

static int	set_prompt(t_mshell *obj)
{
	char *line;

	if (isatty(fileno(stdin)))
		obj->cmd_line = readline(PROMPT);
	else
	{
		line = get_next_line(fileno(stdin));
		if (!line)
			return (0);
		obj->cmd_line = ft_strtrim(line, "\n");
		free(line);
	}
	return (1);
}

static void	listen_and_proceed_input(t_mshell *obj)
{
	while (1)
	{
		g_signal_received = 0;
		transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
		if (set_prompt(obj) == 0)
			break ;
		if (is_cmd_line_invalid(obj) == 1)  /* Handling Ctrl+D (EOF) */
			break ;
		else if (is_cmd_line_invalid(obj) == 2)
			continue ;
		add_history(obj->cmd_line);
		parse(obj);
		if (is_ast_invalid(obj) == 1)
			continue ;
		if (is_main_signaled(obj) == 1)
			continue ;
		free(obj->cmd_line);
		obj->cmd_line = NULL;
		choose_actions(obj);
		close_fds(obj);
		wait_for_children(obj);
		transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
		clean_mshell(obj);
		obj->paths = fetch_paths(obj->envp);
	}
}

int main(int argc, char **argv, char **envp)
{
	t_mshell    obj;

	if (argc != 1)
		return (error_ret(1, NULL));
	disable_echoctl();
	transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
	obj = init_shell(argv, envp);
	listen_and_proceed_input(&obj);
	clean_exit(&obj);
}
