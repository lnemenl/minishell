/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 15:34:32 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/24 13:01:21 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	handle_fail_arg(t_mshell *obj, char *new_arg, char *arg)
{
	if (ft_strncmp(new_arg, "fail", 4) == 0)
	{
		if (ft_strcmp(new_arg, "fail_option") == 0)
			obj->exit_code = 2;
		else
			obj->exit_code = 1;
		free(new_arg);
		ft_fprintf(2, "export: `%s`: not a valid identifier\n", arg);
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

static void	export_no_args(t_mshell *obj)
{
	char	**temp;
	size_t	i;

	if (!obj->envp)
		return ;
	temp = copy_envp(obj->envp);
	if (!temp)
		return ;
	sort_envp(temp);
	i = 0;
	while (temp[i])
	{
		printf("declare -x %s\n", temp[i]);
		i++;
	}
	ft_free_strs(temp, get_envp_length(temp));
}

int	export(char **args, t_mshell *obj)
{
	char	*new_arg;
	int		i;

	if (!args[1] || !*args[1])
	{
		export_no_args(obj);
		return (1);
	}
	i = 1;
	while (args[i])
	{
		new_arg = check_env_arg(args[i]);
		if (!new_arg)
			return (1);
		if (handle_fail_arg(obj, new_arg, args[i]) == 1)
			return (1);
		put_env_var(obj, new_arg);
		free(new_arg);
		i++;
	}
	obj->exit_code = 0;
	return (1);
}
