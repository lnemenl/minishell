/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/27 14:34:40 by msavelie         ###   ########.fr       */
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
	chdir(full_path);
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	free(buf);
	free(full_path);
	obj->exit_code = 0;
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

void	echo(char **args)
{
	int	i;

	if (!args || !*args)
	{
		printf("\n");
		return ;
	}
	i = 1;
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	printf("\n");
}
