/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 13:22:30 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/27 15:56:15 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	**fill_new_envp(char **dest, char *arg,
	char **new_envp, size_t arg_len)
{
	size_t	i;
	size_t	skip;

	i = -1;
	skip = 0;
	while (dest[++i])
	{
		if (ft_strncmp(dest[i], arg, arg_len) == 0
			&& (!dest[i][arg_len] || dest[i][arg_len] == '='))
			skip = 1;
		if (dest[i + skip])
		{
			new_envp[i] = ft_strdup(dest[i + skip]);
			if (!new_envp)
			{
				ft_free_strs(new_envp, i);
				ft_putendl_fd("Malloc error\n", 2);
				return (NULL);
			}
		}
	}
	new_envp[i] = NULL;
	return (new_envp);
}

static char	**delete_env(char *arg, char ***dest, char *hint)
{
	size_t	envp_len;
	size_t	arg_len;
	char	**new_envp;

	if (!arg)
		return (*dest);
	if (is_env_created(arg, *dest, hint) == -1)
		return (*dest);
	envp_len = get_envp_length(*dest);
	new_envp = ft_calloc(envp_len + 1, sizeof(char *));
	if (!new_envp)
		return (*dest);
	arg_len = ft_strlen(arg);
	new_envp = fill_new_envp(*dest, arg, new_envp, arg_len);
	ft_clean_strs(*dest);
	*dest = NULL;
	return (new_envp);
}

int	unset(char **args, t_mshell *obj)
{
	int	i;

	obj->exit_code = 0;
	if (!args || !args[1] || !*args[1])
		return (1);
	i = 1;
	while (args[i])
	{
		if (args[i][0] == '-')
		{
			obj->exit_code = 2;
			ft_fprintf(STDERR_FILENO, "minishell: unset: %s: invalid option",
				args[i]);
			return (1);
		}
		else if (ft_strcmp(args[i], "PATH") == 0)
		{
			ft_clean_strs(obj->paths);
			obj->paths = NULL;
		}
		obj->envp = delete_env(args[i], &obj->envp, "envp");
		obj->exp_args = delete_env(args[i], &obj->exp_args, "exp");
		i++;
	}
	return (1);
}
