/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 13:59:06 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/20 17:00:02 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	**read_alloc(int fd, size_t *i)
{
	size_t		buffer;
	char		**strs;
	t_mshell	*obj;

	obj = NULL;
	buffer = 50;
	strs = ft_calloc(buffer, sizeof(char *));
	if (!strs)
		error_ret(5, NULL, obj);
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
