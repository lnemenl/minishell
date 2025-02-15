/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 13:22:30 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/13 15:14:40 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	**delete_env(char *arg, t_mshell *obj)
{
	size_t	i;
	size_t	envp_len;
	char	**new_envp;
	int		skip;
	size_t	arg_len;

	if (!arg)
		return (obj->envp);
	if (is_env_created(arg, obj->envp) == -1)
		return (obj->envp);
	envp_len = get_envp_length(obj->envp);
	new_envp = ft_calloc(envp_len + 1, sizeof(char *));
	if (!new_envp)
		return (obj->envp);
	i = 0;
	skip = 0;
	arg_len = ft_strlen(arg);
	while (obj->envp[i])
	{
		if (ft_strncmp(obj->envp[i], arg, arg_len) == 0)
			skip = 1;
		if (obj->envp[i + skip])
		{
			new_envp[i] = ft_strdup(obj->envp[i + skip]);
			//TODO: add malloc checker
			i++;
		}
		else
			break ;
	}
	new_envp[i] = NULL;
	//TODO: fix free issue
	//ft_clean_strs(obj->envp);
	//obj->envp = NULL;
	return (new_envp);
}

int	unset(char **args, t_mshell *obj)
{
	obj->exit_code = 0;
	if (!args || !args[1] || !*args[1])
		return (1);
	if (ft_strcmp(args[1], "PATH") == 0)
	{
		ft_clean_strs(obj->paths);
		obj->paths = NULL;
	}
	obj->envp = delete_env(args[1], obj);
	return (1);
}
