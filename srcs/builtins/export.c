/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 15:34:32 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/27 16:37:54 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	is_env_created(char *arg, char **strs)
{
	int		i;
	size_t	len;

	len = 0;
	while (arg[len] != '=')
		len++;
	i = 0;
	while (strs[i])
	{
		if (ft_strncmp(strs[i], arg, len) == 0)
			return (i);
		i++;
	}
	return (-1);
}

static char	*check_env_arg(char *arg)
{
	char 	*equal;

	if (!arg)
		return (NULL);
	else if (ft_isdigit(arg[0]))
		return (NULL);
	equal = ft_strchr(arg, '=');
	if (!equal)
		return (NULL);
	else if (equal[1] == '=' || equal[1] == '\0'
		|| (equal - 1 && *(equal - 1) == '-'))
		return (NULL);
	return (ft_strdup(arg));
}

size_t	get_envp_memory_size(char **envp)
{
	size_t	len;
	size_t	total_memory;

	if (!envp)
		return (0);
	len = 0;
	total_memory = 0;
	while (envp[len])
	{
		total_memory += ft_strlen(envp[len]) + 1;
		len++;
	}
	total_memory += (len + 1) * sizeof(char *);
	return (total_memory);
}

size_t	get_envp_length(char **envp)
{
	size_t	len;

	if (!envp)
		return (0);
	len = 0;
	while (envp[len])
		len++;
	return (len);
}

static void	put_env_var(t_mshell *obj, char *new_arg)
{
	int		pos;
	size_t	arg_len;
	size_t	envp_len;
	size_t	envp_mem_size;

	arg_len = ft_strlen(new_arg);
	envp_len = get_envp_length(obj->envp);
	envp_mem_size = get_envp_memory_size(obj->envp);
	pos = is_env_created(new_arg, obj->envp);
	if (pos == -1)
	{
		obj->envp = ft_realloc(obj->envp, envp_mem_size, envp_mem_size + sizeof(char *));
		obj->envp[envp_len] = ft_calloc(arg_len + 1, 1);
		if (!obj->envp[envp_len])
		{
			//ft_free_strs(strs, i);
			exit(error_ret(6, NULL));
		}
		obj->envp[envp_len] = ft_memcpy(obj->envp[envp_len], new_arg, arg_len);
		obj->envp[++envp_len] = NULL;
		return ;
	}
	obj->envp[pos] = ft_realloc(obj->envp[pos], ft_strlen(obj->envp[pos]), arg_len + 1);
	if (!obj->envp[pos])
	{
		//ft_free_strs(strs, strs_len);
		exit(error_ret(6, NULL));
	}
	obj->envp[pos] = ft_memmove(obj->envp[pos], new_arg, arg_len);
	obj->envp[++pos] = NULL;
}

int	export(char **args, t_mshell *obj)
{
	char	*new_arg;

	if (!args || !args[1] || !*args[1])
		return (1);
	new_arg = check_env_arg(args[1]);
	if (!new_arg)
	{
		printf("export: `%s`: not a valid identifier\n", args[1]);
		return (1);
	}
	put_env_var(obj, new_arg);
	free(new_arg);
	return (1);
}
