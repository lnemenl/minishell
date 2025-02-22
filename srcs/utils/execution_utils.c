/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 11:24:28 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/22 17:25:27 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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
				if (WTERMSIG(status) == SIGSEGV)
					ft_putendl_fd("Segmentation fault (core dumped)",
						STDERR_FILENO);
				obj->exit_code = 128 + WTERMSIG(status);
			}
		}
		obj->exec_cmds--;
	}
}
