/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 11:24:28 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 11:29:01 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	reset_stdin(t_mshell *obj)
{
	if (obj->stdin_fd != -1)
	{
		dup2(obj->stdin_fd, STDIN_FILENO);
		close(obj->stdin_fd);
		obj->stdin_fd = -1;
	}
}

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
				obj->exit_code = 128 + WTERMSIG(status);
			}
		}
		obj->exec_cmds--;
	}
}
