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
