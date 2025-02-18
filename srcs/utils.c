/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 13:59:06 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/02 14:08:37 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

size_t	get_envp_memory_size(char **envp)
{
	size_t	len;
	size_t	total_memory;

	if (!envp)
		return (0);
	len = 0;
	total_memory = 0;
	while (envp[len])
	{
		total_memory += ft_strlen(envp[len]) + 1;
		len++;
	}
	total_memory += (len + 1) * sizeof(char *);
	return (total_memory);
}

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
/* `var_name` should be provided with `=` character */
char	*get_env_var(char **envp, const char *var_name)
{
	size_t	i;
	size_t	var_name_len;
	char	*trimmed_str;

	if (!envp || !*envp || !var_name)
		return (NULL);
	i = 0;
	var_name_len = ft_strlen(var_name);
	while (envp[i])
    {
        if (ft_strncmp(envp[i], var_name, var_name_len) == 0)
		{
			trimmed_str = ft_strtrim(envp[i] + var_name_len, "=");
			return (trimmed_str);
		}
		i++;
    }
	return (NULL);
}
