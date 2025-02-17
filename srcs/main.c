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
	obj.cmds = NULL;
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

int main(int argc, char **argv, char **envp)
{
	t_mshell    obj;
	char        *line;

	if (argc != 1)
		return (error_ret(1, NULL));
	disable_echoctl();
	transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);

	/* Initialize shell data structures (copies envp, fetches PATH, etc.). */
	obj = init_shell(argv, envp);

	while (1)
	{
		g_signal_received = 0;
		rl_catch_signals = 0;
		transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
		if (isatty(fileno(stdin)))
			obj.cmd_line = readline(PROMPT);
		else
		{
			//ft_putstr_fd(PROMPT, STDOUT_FILENO);
			line = get_next_line(fileno(stdin));
			if (!line)
				break;
			obj.cmd_line = ft_strtrim(line, "\n");
			free(line);
		}
		if (!obj.cmd_line)  /* Handling Ctrl+D (EOF) */
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break;
		}
		if (!*obj.cmd_line)
		{
			clean_mshell(&obj);
			obj.paths = fetch_paths(obj.envp);
			continue ;
		}
		add_history(obj.cmd_line);

		parse(&obj);       /* Tokenize / build AST */
		if (!obj.ast)
		{
			free(obj.cmd_line);
			obj.cmd_line = NULL;
			continue;
		}
		if (g_signal_received == SIGINT || obj.heredoc_interrupted)
		{
			free(obj.cmd_line);
			obj.cmd_line = NULL;
			free_ast(obj.ast);
			obj.ast = NULL;
			obj.heredoc_interrupted = 0;
			continue;
		}

		free(obj.cmd_line);
		obj.cmd_line = NULL;

		/* Actually run the commands built by parse() */
		choose_actions(&obj);
		close_fds(&obj);

		/* Wait for all child processes to finish */
		wait_for_children(&obj);

		/* Reset signals to ignore or catch again for new interactive input */
		transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);

		/* Clean up AST, pids, etc., then refresh PATH in case environment changed */
		clean_mshell(&obj);
		obj.paths = fetch_paths(obj.envp);
	}

	/* Cleanup when user types 'exit' or Ctrl+D breaks from loop */
	clean_mshell(&obj);
	free(obj.prev_path);

	/* Free envp only once */
	if (obj.envp)
		ft_free_strs(obj.envp, get_envp_length(obj.envp));
	return (obj.exit_code);
}
