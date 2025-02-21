/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 12:03:23 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 17:52:32 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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
	obj.ast = NULL;
	(void) argv;
	return (obj);
}

static int	set_prompt(t_mshell *obj)
{
	char	*line;

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
		if (is_cmd_line_invalid(obj) == 1)
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

int	main(int argc, char **argv, char **envp)
{
	t_mshell	obj;

	if (argc != 1)
		return (error_ret(1, NULL));
	disable_echoctl();
	transition_signal_handlers(SIGNAL_STATE_INTERACTIVE);
	obj = init_shell(argv, envp);
	listen_and_proceed_input(&obj);
	clean_exit(&obj);
}
