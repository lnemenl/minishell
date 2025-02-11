/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/11 16:04:23 by msavelie         ###   ########.fr       */
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

int	cd(char **cd_args, t_mshell *obj)
{
	char	*buf;
	char	*path;
	char	*full_path;
	size_t	buffer_size;

	buffer_size = 50;
	if (!cd_args[1] || !*cd_args[1])
	{
		obj->exit_code = 0;
		chdir(get_env_var(obj->envp, "HOME"));
		return (1);
	}
	else if (cd_args[2] && *cd_args[2])
	{
		obj->exit_code = 1;
		ft_fprintf(2, "minishell: cd: too many arguments\n");
		return (1);
	}
	buf = ft_calloc(buffer_size, sizeof(char));
	if (!buf)
	{
		//cleanup struct
		obj->exit_code = 1;
		ft_fprintf(2, "Malloc error!\n");
		exit(1);
	}
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	path = ft_strjoin(buf, "/");
	full_path = ft_strjoin(path, cd_args[1]);
	free(path);
	if (ft_strcmp(buf, cd_args[1]) == 0)
		obj->exit_code = 0;
	else if (chdir(full_path) == -1)
	{
		obj->exit_code = 1;
		ft_fprintf(2, "minishell: cd: %s: No such file or directory\n", cd_args[1]);
	}
	else
		obj->exit_code = 0;
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	free(buf);
	free(full_path);
	return (1);
}

int	pwd(void)
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
	return (1);
}

void	set_env_args(t_mshell *obj, t_ast_node *node)
{
	int	i;

	i = 0;
	while (node->args[i])
		i++;
	ft_free_strs(node->args, i);
	node->args = ft_calloc(3, sizeof(char *));
	if (!node->args)
	{
		clean_mshell(obj);
		return ;
	}
	node->args[0] = ft_strdup("cat");
	node->args[1] = ft_strdup(".env_temp.txt");
	node->args[2] = NULL;
}

int	env(t_mshell *obj)
{
	size_t	i;

	if (!obj || !obj->envp)
		return (0);
	i = 0;
	while (obj->envp[i])
	{
		printf("%s\n", obj->envp[i]);
		i++;
	}
	return (1);
}

int	echo(char **args)
{
	int	i;

	if (!args || !*args)
	{
		printf("\n");
		return (0);
	}
	i = 1;
	if (ft_strcmp(args[1], "-n") == 0)
		i++;
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (ft_strcmp(args[1], "-n") != 0)
		printf("\n");
	return (1);
}
