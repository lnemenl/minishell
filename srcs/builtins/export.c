/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 15:34:32 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/15 16:46:18 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_env_created(char *arg, char **strs)
{
	int		i;
	size_t	len;

	len = 0;
	while (arg[len] && arg[len] != '=')
		len++;
	if (arg[len] == '=')
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
	int		name_len;

	if (!arg)
		return (NULL);
	if (!ft_isalpha(arg[0]) && arg[0] != '_')
		return (ft_strdup("fail"));
	equal = ft_strchr(arg, '=');
	name_len = equal - arg - 1;
	while (name_len >= 0)
	{
		if (arg[name_len] == '-')
			return (ft_strdup("fail"));
		name_len--;
	}
	if (!equal && ft_strchr(arg, '-'))
		return (ft_strdup("fail"));
	else if (!equal || (equal && (equal[1] == '=' || equal[1] == '\0')))
		return (NULL);
	return (ft_strdup(arg));
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
		obj->envp = ft_realloc(obj->envp, envp_mem_size, envp_mem_size + 2 * sizeof(char *));
		obj->envp[envp_len] = ft_strdup(new_arg);
		if (!obj->envp[envp_len])
		{
			//ft_free_strs(strs, i);
			exit(error_ret(6, NULL));
		}
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
	obj->envp[pos][arg_len] = '\0';
	obj->envp[++pos] = NULL;
}

int	export(char **args, t_mshell *obj)
{
	char	*new_arg;

	if (!args || !args[1] || !*args[1])
		return (1);
	new_arg = check_env_arg(args[1]);
	if (ft_strcmp(new_arg, "fail") == 0)
	{
		free(new_arg);
		obj->exit_code = 1;
		ft_fprintf(2, "export: `%s`: not a valid identifier\n", args[1]);
		return (1);
	}
	else if (!new_arg)
		return (1);
	put_env_var(obj, new_arg);
	free(new_arg);
	obj->exit_code = 0;
	return (1);
}
