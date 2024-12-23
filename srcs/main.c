/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:17:46 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/23 11:25:14 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static t_mshell	init_shell(char **argv, char **envp)
{
	t_mshell	obj;

	obj.is_heredoc = 0;
	obj.allocated_pipes = 0;
	obj.cmd_line = NULL;
	obj.cmds = NULL;
	obj.cur_path = NULL;
	obj.pipfd = NULL;
	obj.total_cmds = 0;
	obj.paths = fetch_paths(envp);
	(void) argv;
	return (obj);
}

// int	main(int argc, char **argv, char **envp)
// {
// 	t_mshell	obj;

// 	if (argc != 1)
// 		return (error_ret(1, NULL));
// 	obj = init_shell(argv, envp);
// 	ft_printf("mega-shell: ");
// 	obj.cmd_line = get_next_line(0);
// 	while (obj.cmd_line && 
// 		ft_strcmp(obj.cmd_line, "exit\n") != 0)
// 	{
// 		parse(&obj);
// 		if (obj.cmd_line)
// 			free(obj.cmd_line);
// 		ft_printf("mega-shell: ");
// 		obj.cmd_line = get_next_line(0);
// 	}
// 	if (obj.cmd_line)
// 		free(obj.cmd_line);
// 	clean_mshell(&obj);
// 	return (0);
// }

int	main(int argc, char **argv, char **envp)
{
	t_mshell	obj;

	if (argc != 1)
		return (error_ret(1, NULL));
	obj = init_shell(argv, envp);
	while (1)
	{
		//ft_printf("mega-shell: "); // Prompt display
		//obj.cmd_line = get_next_line(0); // Read user input
		//if (!obj.cmd_line)
		//	break; // Exit loop on EOF (Ctrl+D)
		obj.cmd_line = readline(PROMPT);
		if (ft_strcmp(obj.cmd_line, "exit\n") == 0)
		{
			free(obj.cmd_line);
			break;
		}
		parse(&obj);
		free(obj.cmd_line);
		obj.cmd_line = NULL;
	}
	clean_mshell(&obj);
	return (0);
}