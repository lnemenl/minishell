/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 15:34:32 by msavelie          #+#    #+#             */
/*   Updated: 2025/03/03 12:39:03 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	handle_fail_arg(t_mshell *obj, char **new_arg, char *arg)
{
	if (ft_strncmp(*new_arg, "fail", 4) == 0)
	{
		if (ft_strcmp(*new_arg, "fail_option") == 0)
			obj->exit_code = 2;
		else
			obj->exit_code = 1;
		if (*new_arg)
			free(*new_arg);
		*new_arg = NULL;
		concat_print("export: `", arg, "': not a valid identifier", obj);
		return (1);
	}
	return (0);
}

static void	sort_envp(char **envp)
{
	size_t	len;
	size_t	i;
	size_t	j;
	char	*temp;

	len = get_envp_length(envp);
	i = 0;
	while (i < len - 1)
	{
		j = 0;
		while (j < len - i - 1)
		{
			if (ft_strcmp(envp[j], envp[j + 1]) > 0)
			{
				temp = envp[j];
				envp[j] = envp[j + 1];
				envp[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

static void	copy_print(char *arg, t_mshell *obj)
{
	char	*equal;
	char	*name;
	size_t	name_len;

	equal = ft_strchr(arg, '=');
	if (!equal)
		printf("declare -x %s\n", arg);
	else
	{
		name_len = equal - arg;
		name = ft_calloc(name_len + 1, sizeof(char));
		if (!name)
			print_exit("Malloc error\n", NULL, obj);
		name = ft_memmove(name, arg, name_len);
		printf("declare -x %s=\"%s\"\n", name, equal + 1);
		free(name);
	}
}

static void	export_no_args(t_mshell *obj)
{
	char	**temp;
	size_t	i;

	if (!obj->envp)
		return ;
	temp = copy_envp(obj->exp_args);
	if (!temp)
		return ;
	sort_envp(temp);
	i = 0;
	while (temp[i])
	{
		copy_print(temp[i], obj);
		i++;
	}
	ft_free_strs(temp, get_envp_length(temp));
}

int	export(char **args, t_mshell *obj)
{
	char	*new_arg;
	int		i;

	obj->exit_code = 0;
	if (!args[1])
	{
		export_no_args(obj);
		return (1);
	}
	i = 0;
	while (args[++i])
	{
		new_arg = check_env_arg(args[i]);
		if (!new_arg)
			continue ;
		if (handle_fail_arg(obj, &new_arg, args[i]) == 1)
			continue ;
		if (ft_strcmp(new_arg, "export") == 0)
			put_env_var(obj, args[i], "exp");
		else
			put_env_var(obj, new_arg, "envp");
		free(new_arg);
		new_arg = NULL;
	}
	return (1);
}
