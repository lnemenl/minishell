/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/13 17:50:28 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	init_shell_mode(t_mshell *mshell)
{
	if (!mshell)
		return (0);
	
	mshell->interactive_mode = isatty(STDIN_FILENO);
	if (mshell->interactive_mode)
	{
		//setting up handling for interactive mode
		setup_shell_signals(mshell);
	}
	return (1);
}

