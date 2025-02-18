/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 15:36:40 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/18 16:07:15 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*handle_cd_path(char *provided_path, char *buf, t_mshell *obj)
{
	char	*ret_path;
	char	*path;

	if (*provided_path == '/')
		return (provided_path);
	path = ft_strjoin(buf, "/");
	if (!path)
		print_exit("Malloc error\n", "cd", obj);
	ret_path = ft_strjoin(path, provided_path);
	free(path);
	if (!ret_path)
		print_exit("Malloc error\n", "cd", obj);
	return (ret_path);
}

static int	check_cd_args(char **cd_args, t_mshell *obj, char *buf)
{
	if (!cd_args[1] || !*cd_args[1])
	{
		obj->exit_code = 0;
		if (obj->prev_path)
			free(obj->prev_path);
		obj->prev_path = ft_strdup(buf);
		chdir(get_env_var(obj->envp, "HOME"));
		return (1);
	}
	else if (cd_args[2] && *cd_args[2])
	{
		obj->exit_code = 1;
		ft_fprintf(2, "minishell: cd: too many arguments\n");
		return (1);
	}
	return (0);
}

static void	change_path(t_mshell *obj, char **cd_args, char *buf, char *full_path)
{
	if (ft_strcmp(buf, cd_args[1]) == 0)
		obj->exit_code = 0;
	else if (ft_strcmp(cd_args[1], "-") == 0)
	{
		if (chdir(obj->prev_path) == -1)
		{
			obj->exit_code = 1;
			ft_fprintf(2, "minishell: cd: %s: No such file or directory\n", obj->prev_path);
		}
		printf("%s\n", obj->prev_path);
	}
	else if (chdir(full_path) == -1)
	{
		obj->exit_code = 1;
		ft_fprintf(2, "minishell: cd: %s: No such file or directory\n", cd_args[1]);
	}
	else
		obj->exit_code = 0;
}

int	cd(char **cd_args, t_mshell *obj)
{
	char	buf[PATH_BUFFER_SIZE];
	char	*full_path;

	if (!getcwd(buf, PATH_BUFFER_SIZE))
	{
		perror("getcwd");
		obj->exit_code = 1;
	}
	if (check_cd_args(cd_args, obj, buf) == 1)
		return (1);
	full_path = handle_cd_path(cd_args[1], buf, obj);
	if (!full_path)
		return (1);
	change_path(obj, cd_args, buf, full_path);
	free(obj->prev_path);
	obj->prev_path = ft_strdup(buf);
	if (!getcwd(buf, PATH_BUFFER_SIZE))
	{
		perror("getcwd");
		obj->exit_code = 1;
	}
	if (ft_strcmp(full_path, cd_args[1]) != 0)
		free(full_path);
	return (1);
}
