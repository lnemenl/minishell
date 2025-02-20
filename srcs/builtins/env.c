/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/20 13:46:42 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	env(t_mshell *obj, char **args)
{
	size_t	i;

	if (!obj || !obj->envp)
		return (0);
	if (*(args + 1) != NULL)
	{
		ft_fprintf(STDERR_FILENO, "env: %s: too many arguments\n", *(args + 1));
		obj->exit_code = 1;
		return (1);
	}
	i = 0;
	while (obj->envp[i])
	{
		printf("%s\n", obj->envp[i]);
		i++;
	}
	obj->exit_code = 0;
	return (1);
}
