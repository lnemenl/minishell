/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 14:06:50 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/27 14:27:05 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	free_path(char *path)
{
	if (!path || !*path)
		return ;
	free(path);
	path = NULL;
}

void	print_exit(char *mes, char *cmd, int exit_code)
{
	char	*full_msg;
	int		mes_len;

	mes_len = ft_strlen(mes) + ft_strlen(cmd) + 3;
	full_msg = ft_calloc(mes_len, sizeof(char));
	if (!full_msg)
	{
		ft_putstr_fd("Malloc failed\n", 2);
		exit (exit_code);
	}
	if (cmd)
		ft_strlcpy(full_msg, cmd, mes_len);
	ft_strlcat(full_msg, ": ", mes_len);
	ft_strlcat(full_msg, mes, mes_len);
	ft_putstr_fd(full_msg, 2);
	free (full_msg);
	exit (exit_code);
}

static void	check_is_dir(char *arg, t_mshell *obj)
{
	int	fd;

	fd = open(arg, O_DIRECTORY);
	if (fd >= 0)
	{
		close(fd);
		clean_mshell(obj);
		obj->exit_code = 126;
		print_exit("Is a directory\n", arg, obj->exit_code);
	}
}

static char	*check_paths(char **paths, char **args)
{
	int		i;
	char	*path;
	size_t	path_len;

	if (!paths || !*paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		path_len = ft_strlen(paths[i]) + ft_strlen(args[0]) + 2;
		path = ft_calloc(path_len, sizeof(char));
		if (!path)
		{
			ft_putstr_fd("Malloc failed\n", 2);
			return (NULL);
		}
		ft_strlcpy(path, paths[i], path_len);
		ft_strlcat(path, "/", path_len);
		ft_strlcat(path, args[0], path_len);
		if (access(path, F_OK) == 0)
			return (path);
		free_path(path);
		i++;
	}
	return (NULL);
}

static char	**check_args(char **args)
{
	static char	*new_args[] = {"", NULL};

	if (!args)
		return (new_args);
	return (args);
}

char	*check_paths_access(char **paths, char **args, t_mshell *obj)
{
	char	*path;

	args = check_args(args);
	check_is_dir(args[0], obj);
	if (args[0][0] == '/' || args[0][0] == '.')
		return (ft_strdup(args[0]));
	else
	{
		path = check_paths(paths, args);
		if (!path || args[0][0] == '\0')
		{
			clean_mshell(obj);
			if (path)
				free(path);
			obj->exit_code = 127;
			print_exit("command not found\n", args[0], obj->exit_code);
		}
		check_is_dir(path, obj);
		return (path);
	}
}

/*int	check_permission(t_mshell *obj, char **argv, int first)
{
	if (first)
	{
		if (access(argv[1], F_OK) != 0)
			exit_child(obj, argv[1], 1);
		obj->fd_in = open(argv[1], O_RDONLY);
		if (obj->fd_in == -1)
			exit_child(obj, argv[1], 1);
	}
	else
	{
		if (!argv[2 + obj->mid_args])
			exit_child(obj, argv[2 + obj->mid_args], 1);
		if (obj->is_heredoc == 1)
			obj->fd_out = open(argv[2 + obj->mid_args],
					O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			obj->fd_out = open(argv[2 + obj->mid_args],
					O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (obj->fd_out == -1)
			exit_child(obj, argv[2 + obj->mid_args], 1);
	}
	return (0);
}*/
