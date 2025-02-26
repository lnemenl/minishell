/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 14:06:50 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/26 15:26:44 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	check_is_dir(char *arg, t_mshell *obj)
{
	int	fd;

	if (!ft_strchr(arg, '/'))
		return ;
	fd = open(arg, O_DIRECTORY);
	if (fd >= 0)
	{
		close(fd);
		obj->exit_code = 126;
		print_exit("Is a directory\n", arg, obj);
	}
}

static char	*create_path(char *arg, size_t args_move, char *envp_path)
{
	size_t	path_len;
	char	*path;

	path_len = ft_strlen(envp_path) + ft_strlen(arg + args_move) + 2;
	path = ft_calloc(path_len, sizeof(char));
	if (!path)
	{
		ft_putstr_fd("Malloc failed\n", STDERR_FILENO);
		return (NULL);
	}
	ft_strlcpy(path, envp_path, path_len);
	ft_strlcat(path, "/", path_len);
	ft_strlcat(path, arg + args_move, path_len);
	return (path);
}

static char	*search_paths(char **paths, char **args, size_t *args_move)
{
	int		i;
	char	*path;

	if (!args || !*args || !**args)
		return (ft_strdup(""));
	else if (!**args && *(*args + 2))
		*args_move = 1;
	if (!paths || !*paths)
		return (ft_strdup(args[0]));
	i = 0;
	while (paths[i])
	{
		path = create_path(*args, *args_move, paths[i]);
		if (access(path, F_OK) == 0)
			return (path);
		check_free_str(&path);
		i++;
	}
	return (ft_strdup(*args + *args_move));
}

static char	*check_path(char *path, char *node_arg,
	t_mshell *obj, t_ast_node *node)
{
	if (!path || (path && !ft_strchr(node->args[0], '/') && access(path, X_OK) != 0) || !node_arg || !*node_arg)
	{
		check_free_str(&path);
		if (!node_arg)
		{
			obj->exit_code = 0;
			clean_mshell(obj);
			return (NULL);
		}
		obj->exit_code = 127;
		print_exit("command not found\n", node->args[0 + obj->args_move], obj);
	}
	return (path);
}

char	*check_paths_access(char **paths, t_ast_node *node, t_mshell *obj)
{
	char		*path;
	static char	*new_args[] = {"", NULL};

	if (!node)
		return (NULL);
	if (!node->args)
		node->args = new_args;
	check_is_dir(node->args[0], obj);
	if (node->args[0][0] == '/' || node->args[0][0] == '.')
		return (ft_strdup(node->args[0]));
	path = search_paths(paths, node->args, &obj->args_move);
	if (!check_path(path, *(node->args) + obj->args_move, obj, node))
		return (NULL);
	if (ft_strchr(node->args[0], '/'))
		check_is_dir(path, obj);
	return (path);
}
