/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_token_env.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:11:55 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 15:18:08 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*get_var_name(const char *str, int *i)
{
	int	start;
	int	len;

	start = *i;
	if (!str[*i])
		return (NULL);
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_strdup("?"));
	}
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	len = *i - start;
	if (len == 0)
		return (NULL);
	return (ft_substr(str, start, len));
}

char	*get_env_value(const char *var_name, t_mshell *mshell)
{
	char	*value;
	char	*temp_var_name;

	if (!var_name)
		return (NULL);
	if (ft_strcmp(var_name, "?") == 0)
		return (ft_itoa(mshell->exit_code));
	temp_var_name = ft_strjoin(var_name, "=");
	value = get_env_var(mshell->envp, temp_var_name);
	free(temp_var_name);
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

char	*join_and_free(char *s1, char *s2)
{
	char	*result;

	if (!s1)
		return (s2);
	if (!s2)
		return (s1);
	result = ft_strjoin(s1, s2);
	if (!result)
	{
		free(s1);
		free(s2);
		return (NULL);
	}
	free(s1);
	free(s2);
	return (result);
}

char	*extract_quoted_var_name(const char *str, int *i)
{
	char	quote;
	int		start;
	char	*quoted;

	if (str[*i] != '"' && str[*i] != '\'')
		return (NULL);
	quote = str[*i];
	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != quote)
		(*i)++;
	quoted = ft_substr(str, start, *i - start);
	if (str[*i] == quote)
		(*i)++;
	return (quoted);
}
