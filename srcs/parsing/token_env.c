/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:11:55 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/13 19:13:13 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char *get_var_name(const char *str, int *i)
{
	int start;
	int len;

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

char *get_env_value(const char *var_name, t_mshell *mshell)
{
	char *value;

	if (!var_name)
		return (NULL);
	if (ft_strcmp(var_name, "?") == 0)
		return (ft_itoa(mshell->exit_code));
	value = get_env_var(mshell->envp, var_name);
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

static char *join_and_free(char *s1, char *s2)
{
	char *result;

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

char	*remove_quotes(const char *str)
{
	int		len;
	char	*without;
	
	if (!str)
		return (ft_strdup(""));
	len = ft_strlen(str);
	if ((str[0] == '"' && str[len - 1] == '"') ||
		(str[0] == '\'' && str[len - 1] == '\''))
		without = ft_substr(str, 1, len - 2);
	else
		without = ft_strdup(str);
	return (without);
}



char	*expand_env_vars(const char *str, t_mshell *mshell)
{
	char	*result;
	char	*var_name;
	char	*var_value;
	char	*bash_pid;
	int		i;
	int		start;

	if (!str || !mshell)
		return (ft_strdup(""));
	result = NULL;
	i = 0;
	while (str[i])
	{
		start = i;
		while (str[i] && str[i] != '$')
			i++;
		if (i > start)
			result = join_and_free(result, ft_substr(str, start, i - start));
		if (str[i] == '$')
		{
			i++;
			/* Handle literal "$$" case */
			if (str[i] == '$')
			{
				i++;
				bash_pid = get_env_var(mshell->envp, "SESSION_MANAGER=");
				if (bash_pid)
					result = join_and_free(result, ft_strdup(ft_strrchr(bash_pid, '/') + 1));
				else
					result = join_and_free(result, ft_strdup(""));
			}
			else if (is_quote(str[i]))
			{
				var_name = extract_quoted_var_name(str, &i);
				if (var_name)
				{
					result = join_and_free(result, var_name);
					free(var_name);
				}
				else
					result = join_and_free(result, ft_strdup("$"));
			}
			else
			{
				var_name = get_var_name(str, &i);
				if (var_name)
				{
					var_value = get_env_value(var_name, mshell);
					result = join_and_free(result, var_value);
					free(var_name);
				}
				else
					result = join_and_free(result, ft_strdup("$"));
			}
		}
	}
	if (!result)
		return (ft_strdup(""));
	return (result);
}
