/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 13:22:30 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/09 11:52:57 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	**delete_env(char *arg, t_mshell *obj)
{
	size_t	i;
	size_t	envp_len;
	char	**new_envp;
	int		skip;

	if (!arg)
		return (obj->envp);
	if (is_env_created(arg, obj->envp) == -1)
		return (obj->envp);
	envp_len = get_envp_length(obj->envp);
	new_envp = ft_calloc(envp_len, sizeof(char *));
	i = 0;
	skip = 0;
	while (obj->envp[i])
	{
		if (ft_strncmp(obj->envp[i], arg, ft_strlen(arg)) == 0)
			skip = 1;
		if (obj->envp[i + skip])
		{
			new_envp[i] = ft_strdup(obj->envp[i + skip]);
			i++;
		}
		else
			break ;
	}
	new_envp[i] = NULL;
	ft_clean_strs(obj->envp);
	return (new_envp);
}

int	unset(char **args, t_mshell *obj)
{
	if (!args || !args[1] || !*args[1])
		return (1);
	if (ft_strcmp(args[1], "PATH") == 0)
	{
		clean_strs(obj->paths);
		obj->paths = NULL;
	}
	obj->envp = delete_env(args[1], obj);
	return (1);
}
