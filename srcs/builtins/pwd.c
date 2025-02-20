/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 13:46:02 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/20 14:17:40 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	getcwd_and_check(t_mshell *obj, char *buf)
{
	if (!getcwd(buf, PATH_BUFFER_SIZE))
	{
		perror("getcwd");
		obj->exit_code = 1;
	}
}

int	pwd(t_mshell *obj)
{
	char		buf[PATH_BUFFER_SIZE];
	static char	*error_args[] = {"cd", NULL};

	if (!getcwd(buf, PATH_BUFFER_SIZE))
	{
		perror("getcwd");
		cd(error_args, obj);
	}
	else
		printf("%s\n", buf);
	obj->exit_code = 0;
	return (1);
}
