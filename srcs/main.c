/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:17:46 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/11 12:37:07 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static t_mshell	init_shell(char **argv, char **envp)
{
	t_mshell	obj;

	obj.is_heredoc = 0;
	obj.allocated_pipes = 0;
	obj.cmd_line = NULL;
	//obj.cmds = NULL;
	obj.cur_path = NULL;
	obj.pipfd = NULL;
	obj.total_cmds = 0;
	obj.paths = fetch_paths(envp);
	(void) argv;
	return (obj);
}

int	main(int argc, char **argv, char **envp)
{
	t_mshell	obj;

	if (argc != 1)
		return (error_ret(1, NULL));
	obj = init_shell(argv, envp);
	ft_printf("mega-shell: ");
	obj.cmd_line = get_next_line(0);
	while (obj.cmd_line && 
		ft_strcmp(obj.cmd_line, "exit\n") != 0)
	{
		parse(&obj);
		if (obj.cmd_line)
			free(obj.cmd_line);
		ft_printf("mega-shell: ");
		obj.cmd_line = get_next_line(0);
	}
	if (obj.cmd_line)
		free(obj.cmd_line);
	clean_mshell(&obj);
	return (0);
}