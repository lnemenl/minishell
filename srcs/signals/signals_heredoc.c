/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:30:12 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 17:52:25 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	handle_heredoc_sigint(t_mshell *obj, int ret_fd, t_heredoc *heredoc)
{
	if (g_signal_received == SIGINT)
	{
		obj->heredoc_interrupted = 1;
		close(heredoc->pipe_fd[0]);
		ret_fd = -1;
		return (ret_fd);
	}
	return (ret_fd);
}
