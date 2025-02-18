/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 14:06:50 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/17 14:24:16 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	print_exit(char *mes, char *cmd, t_mshell *obj)
{
	char	*full_msg;
	int		mes_len;

	mes_len = ft_strlen(mes) + ft_strlen(cmd) + 3;
	full_msg = ft_calloc(mes_len, sizeof(char));
	if (!full_msg)
		ft_putstr_fd("Malloc failed\n", 2);
	else
	{
		if (cmd)
			ft_strlcpy(full_msg, cmd, mes_len);
		ft_strlcat(full_msg, ": ", mes_len);
		ft_strlcat(full_msg, mes, mes_len);
		ft_putstr_fd(full_msg, 2);
		check_free_str(&full_msg);
	}
	clean_exit(obj);
}

static void	check_is_dir(char *arg, t_mshell *obj)
{
	int	fd;

	fd = open(arg, O_DIRECTORY);
	if (fd >= 0)
	{
		close(fd);
		obj->exit_code = 126;
		print_exit("Is a directory\n", arg, obj);
	}
}

static char	*check_paths(char **paths, char **args, size_t *args_move)
{
	int		i;
	char	*path;
	size_t	path_len;

	if (!args || !*args || !**args)
		return (ft_strdup(""));
	else if (!**args && *(*args + 2))
		*args_move = 1;
	if (!paths || !*paths)
		return (ft_strdup(args[0]));
	i = 0;
	while (paths[i])
	{
		path_len = ft_strlen(paths[i]) + ft_strlen((*args) + *args_move) + 2;
		path = ft_calloc(path_len, sizeof(char));
		if (!path)
		{
			ft_putstr_fd("Malloc failed\n", STDERR_FILENO);
			return (NULL);
		}
		ft_strlcpy(path, paths[i], path_len);
		ft_strlcat(path, "/", path_len);
		ft_strlcat(path, (*args) + *args_move, path_len);
		if (access(path, F_OK) == 0)
			return (path);
		check_free_str(&path);
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

char	*check_paths_access(char **paths, t_ast_node *node, t_mshell *obj)
{
	char	*path;

	node->args = check_args(node->args);
	check_is_dir(node->args[0], obj);
	if (node->args[0][0] == '/' || node->args[0][0] == '.')
		return (ft_strdup(node->args[0]));
	path = check_paths(paths, node->args, &obj->args_move);
	if (path && !*path)
	{
		check_free_str(&path);
		exit_child(obj, NULL, 0, 0);
	}
	else if (!path || !*(node->args) + obj->args_move || !*(*(node->args) + obj->args_move))
	{
		check_free_str(&path);
		if (!*(node->args) + obj->args_move || !*(*(node->args) + obj->args_move))
		{
			obj->exit_code = 0;
			clean_mshell(obj);
			return (NULL);
		}
		obj->exit_code = 127;
		print_exit("command not found\n", node->args[0 + obj->args_move], obj);
	}
	check_is_dir(path, obj);
	return (path);
}
