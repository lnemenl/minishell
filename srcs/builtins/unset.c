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

static void	delete_env(char *arg, char **strs, int fd, size_t i)
{
	size_t	j;

	fd = open(".env_temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		//cleanup struct
		ft_free_strs(strs, i);
		unlink(".env_temp.txt");
		exit(error_ret(6, NULL));
	}
	j = 0;
	if (!strs || !*strs || !arg)
		return ;
	while (strs[j])
	{
		if (ft_strncmp(strs[j], arg, ft_strlen(arg)) != 0)
		{
			if (write(fd, strs[j], ft_strlen(strs[j])) == -1)
			{
				// cleanup
				close(fd);
				unlink(".env_temp.txt");
				exit(error_ret(6, NULL));
			}
		}
		j++;
	}
	close(fd);
}

int	unset(char **args, t_mshell *obj)
{
	char	**strs;
	size_t	i;
	int		fd;

	if (!args || !args[1] || !*args[1])
		return (1);
	fd = open(".env_temp.txt", O_RDONLY);
	if (fd == -1)
	{
		//cleanup
		error_ret(6, NULL);
	}
	i = 0;
	strs = read_alloc(fd, &i);
	if (ft_strcmp(args[1], "PATH") == 0)
	{
		clean_strs(obj->paths);
		obj->paths = NULL;
	}
	delete_env(args[1], strs, fd, i);
	ft_free_strs(strs, i);
	return (1);
}
