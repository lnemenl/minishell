/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_utils_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 14:39:05 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/24 13:13:57 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_env_created(char *arg, char **strs)
{
	int		i;
	size_t	len;

	if (!strs || !*strs)
		return (-1);
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

static int	arg_name_invalid(char *equal, char *arg)
{
	int	name_len;

	name_len = equal - arg - 1;
	while (name_len >= 0)
	{
		if (arg[name_len] == '+' && arg[name_len + 1] == '=')
		{
			equal--;
			equal = ft_memmove(&arg[name_len], &arg[name_len + 1],
					ft_strlen(equal));
			name_len--;
			arg[ft_strlen(arg)] = '\0';
		}
		if (!ft_isalpha(arg[name_len]) && !ft_isdigit(arg[name_len])
			&& arg[name_len] != '_')
			return (1);
		name_len--;
	}
	return (0);
}

char	*check_env_arg(char *arg)
{
	char	*equal;

	if (!arg)
		return ("fail");
	if (arg[0] == '-')
		return (ft_strdup("fail_option"));
	else if (!ft_isalpha(arg[0]) && arg[0] != '_')
		return (ft_strdup("fail"));
	equal = ft_strchr(arg, '=');
	if (arg_name_invalid(equal, arg) == 1)
		return (ft_strdup("fail"));
	if (!equal && ft_strchr(arg, '-'))
		return (ft_strdup("fail"));
	else if (!equal || (equal && (equal[1] == '=' || equal[1] == '\0')))
		return (NULL);
	return (ft_strdup(arg));
}

static void	create_new_var(t_mshell *obj, char *new_arg)
{
	size_t	envp_len;
	size_t	envp_mem_size;

	envp_len = get_envp_length(obj->envp);
	envp_mem_size = (envp_len + 1) * sizeof(char *);
	obj->envp = ft_realloc(obj->envp, envp_mem_size,
			envp_mem_size + sizeof(char *));
	if (!obj->envp)
		print_exit("Realloc error\n", NULL, obj);
	obj->envp[envp_len] = ft_strdup(new_arg);
	if (!obj->envp[envp_len])
		print_exit("Malloc_error\n", NULL, obj);
	obj->envp[++envp_len] = NULL;
}

void	put_env_var(t_mshell *obj, char *new_arg)
{
	int		pos;
	size_t	arg_len;

	arg_len = ft_strlen(new_arg);
	pos = is_env_created(new_arg, obj->envp);
	if (pos == -1)
	{
		create_new_var(obj, new_arg);
		return ;
	}
	obj->envp[pos] = ft_realloc(obj->envp[pos], ft_strlen(obj->envp[pos]),
			arg_len + 1);
	if (!obj->envp[pos])
		print_exit("Malloc_error\n", NULL, obj);
	obj->envp[pos] = ft_memmove(obj->envp[pos], new_arg, arg_len);
	obj->envp[pos][arg_len] = '\0';
}
