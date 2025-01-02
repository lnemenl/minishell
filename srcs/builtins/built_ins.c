/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/02 15:01:34 by msavelie         ###   ########.fr       */
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

int	env(void)
{
	int		fd;
	char	*str;

	fd = open(".env_temp.txt", O_RDONLY);
	if (fd == -1)
	{
		//cleanup
		error_ret(6, NULL);
	}
	str = get_next_line(fd);
	while (str)
	{
		printf("%s", str);
		free(str);
		str = get_next_line(fd);
	}
	close(fd);
	return (1);
}

void	echo(char **args)
{
	execve("/usr/bin/echo", args, NULL);
}
