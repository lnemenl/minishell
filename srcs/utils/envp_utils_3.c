/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_utils_3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 14:53:10 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/25 16:00:15 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	join_put_env(t_mshell *obj, char *name, char *value)
{
	char	*joined;

	joined = ft_strjoin(name, value);
	if (!joined)
		return ;
	put_env_var(obj, joined, "envp");
	put_env_var(obj, joined, "export");
	free(joined);
}

void	set_pwds(t_mshell *obj)
{
	char	*temp;
	char	buf[PATH_BUFFER_SIZE];

	getcwd_and_check(obj, buf);
	join_put_env(obj, "PWD=", buf);
	temp = get_env_var(obj->envp, "OLDPWD=");
	if (!temp)
	{
		temp = get_env_var(obj->envp, "HOME=");
		if (!temp)
			return ;
		join_put_env(obj, "OLDPWD=", temp);
		free(temp);
	}
	else
		free(temp);
}

/* `var_name` should be provided with `=` character */
char	*get_env_var(char **envp, const char *var_name)
{
	size_t	i;
	size_t	var_name_len;
	char	*trimmed_str;

	if (!envp || !*envp || !var_name)
		return (NULL);
	i = 0;
	var_name_len = ft_strlen(var_name);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var_name, var_name_len) == 0)
		{
			trimmed_str = ft_strtrim(envp[i] + var_name_len, "=");
			return (trimmed_str);
		}
		i++;
	}
	return (NULL);
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
