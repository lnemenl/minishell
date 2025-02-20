/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/17 17:20:38 by msavelie         ###   ########.fr       */
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

static char	*handle_cd_path(char *provided_path, char *buf)
{
	char	*ret_path;
	char	*path;

	if (*provided_path == '/')
		return (provided_path);
	path = ft_strjoin(buf, "/");
	if (!path)
	{
		// clean
	}
	ret_path = ft_strjoin(path, provided_path);
	free(path);
	if (!ret_path)
	{
		// clean
	}
	return (ret_path);
}

int	cd(char **cd_args, t_mshell *obj)
{
	char	buf[PATH_BUFFER_SIZE];
	char	*full_path;

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
	if (!getcwd(buf, PATH_BUFFER_SIZE))
	{
		perror("getcwd");
		obj->exit_code = 1;
	}
	full_path = handle_cd_path(cd_args[1], buf);
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

int	pwd(t_mshell *obj)
{
	char	buf[PATH_BUFFER_SIZE];
	char	*error_args[] = {"cd", NULL};

	if (!getcwd(buf, PATH_BUFFER_SIZE))
	{
		perror("getcwd");
		cd(error_args, obj);
	}
	else
		printf("%s\n", buf);
	obj->exit_code = 0;
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

int	env(t_mshell *obj, char **args)
{
	size_t	i;

	if (!obj || !obj->envp)
		return (0);
	if (*(args + 1) != NULL)
	{
		ft_fprintf(STDERR_FILENO, "env: %s: too many arguments\n", *(args + 1));
		obj->exit_code = 1;
		return (1);
	}
	i = 0;
	while (obj->envp[i])
	{
		printf("%s\n", obj->envp[i]);
		i++;
	}
	obj->exit_code = 0;
	return (1);
}

static int	check_no_nl(char *arg)
{
	int	i;

	if (!arg || !*arg)
		return (0);
	if (ft_strncmp(arg, "-n", 2) != 0)
		return (0);
	i = 2;
	while (arg[i])
	{
		if (arg[i] == ' ')
			return (i + 1);
		else if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (i);
}

int	echo(char **args, t_mshell *obj, int is_quote)
{
	int	i;
	size_t	no_nl_len;

	if (!args || !*args)
	{
		printf("\n");
		return (0);
	}
	i = 1;
	no_nl_len = 0;
	if (ft_strcmp(args[1], "-n") == 0)
		i++;
	else if (is_quote == 0)
	{
		no_nl_len = check_no_nl(args[1]);
		if (no_nl_len != 0 && no_nl_len == ft_strlen(args[1]))
		{
			i++;
			no_nl_len = 0;
		}
	}
	while (args[i])
	{
		if (*(args[i] + no_nl_len) != '\0')
			printf("%s", args[i] + no_nl_len);
		if (args[i + 1])
			printf(" ");
		i++;
		no_nl_len = 0;
	}
	if (is_quote == 1 || (ft_strcmp(args[1], "-n") != 0 && check_no_nl(args[1]) == 0))
		printf("\n");
	obj->exit_code = 0;
	return (1);
}
