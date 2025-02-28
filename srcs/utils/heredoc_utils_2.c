/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 17:18:18 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/28 17:18:31 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	check_heredoc_count(t_mshell *obj)
{
	if (obj->heredocs_count == 0)
		return (0);
	else if (obj->heredocs_count > 16)
	{
		obj->exit_code = 2;
		print_exit("maximum here-document count exceeded\n", "minishell", obj);
	}
	return (1);
}
