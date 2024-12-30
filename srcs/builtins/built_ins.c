/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/30 16:33:37 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	realloc_buffer(char **buf, size_t *buffer_size)
{
	*buffer_size *= 2;
	free(*buf);
	*buf = ft_calloc(*buffer_size, sizeof(char));
	if (!*buf)
	{
		ft_fprintf(2, "Malloc error!\n");
		exit(1);
	}
}

int	open_dir(const char *dir)
{
	char	*buf;
	char	*path;
	char	*full_path;
	size_t	buffer_size;

	buffer_size = 50;
	if (!dir || !*dir)
		chdir(getenv("HOME"));
	buf = ft_calloc(buffer_size, sizeof(char));
	if (!buf)
	{
		//cleanup struct
		ft_fprintf(2, "Malloc error!\n");
		exit(1);
	}
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	ft_printf("buf: %s\n", buf);
	path = ft_strjoin(buf, "/");
	full_path = ft_strjoin(path, dir);
	free(path);
	ft_printf("path: %s\n", full_path);
	chdir(full_path);
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	ft_printf("buf: %s\n", buf);
	free(buf);
	free(full_path);
	return (1);
}

void	pwd(void)
{
	size_t	buffer_size;
	char	*buf;

	buffer_size = 50;
	buf = ft_calloc(buffer_size, sizeof(char));
	if (!buf)
	{
		//cleanup struct
		ft_fprintf(2, "Malloc error!\n");
		exit(1);
	}
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	printf("%s\n", buf);
	free(buf);
}

void	env(char **env_args, char **envp)
{
	execve("/bin/sh", env_args, envp);
}

void	echo(char **args)
{
	execve("/usr/bin/echo", args, NULL);
}

int	unset(char **args)
{
	// read the env file
	(void) args;
	return (1);
	// append the variable to the proper place
	// append the rest of data
}
