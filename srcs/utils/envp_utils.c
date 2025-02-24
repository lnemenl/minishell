/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 13:59:06 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/24 14:59:23 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

size_t	get_envp_length(char **envp)
{
	size_t	len;

	if (!envp)
		return (0);
	len = 0;
	while (envp[len])
		len++;
	return (len);
}

static int	set_env_to_copy(char **envp, char **new_envp)
{
	size_t	envp_len;

	envp_len = 0;
	while (envp[envp_len])
	{
		new_envp[envp_len] = ft_strdup(envp[envp_len]);
		if (!new_envp[envp_len])
		{
			ft_free_strs(new_envp, envp_len);
			return (0);
		}
		envp_len++;
	}
	new_envp[envp_len] = NULL;
	return (1);
}

char	**copy_envp(char **envp)
{
	char	**new_envp;
	size_t	envp_len;

	if (!envp)
	{
		new_envp = ft_calloc(3, sizeof(char *));
		if (!new_envp)
			return (NULL);
		return (new_envp);
	}
	envp_len = get_envp_length(envp);
	if (envp_len == 0)
		return (NULL);
	new_envp = ft_calloc(envp_len + 1, sizeof(char *));
	if (!new_envp)
		return (NULL);
	if (set_env_to_copy(envp, new_envp) == 0)
		return (NULL);
	return (new_envp);
}
