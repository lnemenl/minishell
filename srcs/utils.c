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

char	**read_alloc(int fd, size_t *i)
{
	size_t	buffer;
	char	**strs;

	buffer = 50;
	strs = ft_calloc(buffer, sizeof(char *));
	if (!strs)
		error_ret(5, NULL);
	strs[*i] = get_next_line(fd);
	while (strs[*i])
	{
		if (*i == buffer - 1)
		{
			strs = ft_realloc(strs, buffer * sizeof(char *), (buffer + 50) * sizeof(char *));
			buffer += 50;
		}
		(*i)++;
		strs[*i] = get_next_line(fd);
	}
	close(fd);
	if (*i > buffer - 3)
	{
		strs = ft_realloc(strs, buffer * sizeof(char *), (buffer + 3) * sizeof(char *));
		buffer += 3;
	}
	return (strs);
}

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
