/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/30 14:44:40 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	size_t	copy_size;
	void	*new_ptr;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	if (ptr == NULL)
		return (ft_calloc(new_size, 1));
	new_ptr = ft_calloc(new_size, 1);
	if (!new_ptr)
	{
		free(ptr);
		return (NULL);
	}
	if (old_size < new_size)
		copy_size = old_size;
	else
		copy_size = new_size;
	ft_memcpy(new_ptr, ptr, copy_size);
	free(ptr);
	return (new_ptr);
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

static void	append_env(char *arg, char **strs, size_t i)
{
	int		fd;
	int		j;
	char	*last_str;
	size_t	str_len;

	unlink(".env_temp.txt");
	fd = open(".env_temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		//cleanup struct
		ft_free_strs(strs, i);
		unlink(".env_temp.txt");
		exit(error_ret(6, NULL));
	}
	str_len = ft_strlen(arg);
	last_str = strs[i];
	printf("last str: %s", last_str);
	strs[i] = ft_calloc(str_len + 2, 1);
	strs[i] = ft_memcpy(strs[i], arg, str_len);
	strs[i][str_len] = '\n';
	if (!strs[i])
	{
		ft_free_strs(strs, i);
		exit(error_ret(6, NULL));
	}
	strs[++i] = last_str;
	strs[++i] = NULL;
	j = 0;
	while (strs[j])
	{
		if (write(fd, strs[j], ft_strlen(strs[j])) == -1)
		{
			close(fd);
			unlink(".env_temp.txt");
			exit(error_ret(6, NULL));
		}
		j++;
	}
	close(fd);
}

int	export(char **args, char **envp)
{
	// read the env file
	// append the variable to the proper place
	// append the rest of data
	char	**strs;
	size_t	buffer;
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
	buffer = 50;
	strs = ft_calloc(buffer, sizeof(char *));
	if (!strs)
		error_ret(5, NULL);
	i = 0;
	strs[i] = get_next_line(fd);
	while (strs[i])
	{
		printf("strs[%zu]: %s", i, strs[i]);
		if (i == buffer - 1)
		{
			strs = ft_realloc(strs, buffer * sizeof(char *), (buffer + 50) * sizeof(char *));
			buffer += 50;
		}
		i++;
		strs[i] = get_next_line(fd);
	}
	close(fd);
	if (i > buffer - 3)
	{
		strs = ft_realloc(strs, buffer * sizeof(char *), (buffer + 3) * sizeof(char *));
		buffer += 3;
	}
	append_env(args[1], strs, --i);
	ft_free_strs(strs, ++i);
	(void) args;
	(void) envp;
	return (1);
}

int	unset(char **args)
{
	// read the env file
	(void) args;
	return (1);
	// append the variable to the proper place
	// append the rest of data
}
