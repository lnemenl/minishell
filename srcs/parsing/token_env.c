/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:11:55 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/20 09:39:11 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*get_var_name(const char *str, int *i)
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

static char *join_and_free(char *s1, char *s2)
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

static char	*append_until_dollar(char *buffer, const char *input, int *i)
{
    int	start;

    start = *i;
    while (input[*i] && input[*i] != '$')
        (*i)++;
    if (*i > start)
        buffer = join_and_free(buffer, ft_substr(input, start, *i - start));
    return (buffer);
}

static char	*handle_pid_expansion(char *buffer, t_mshell *mshell, int *i)
{
    char	*pid_path;

    (*i)++;
    pid_path = get_env_var(mshell->envp, "SESSION_MANAGER=");
    if (pid_path)
        buffer = join_and_free(buffer, ft_strdup(ft_strrchr(pid_path, '/') + 1));
    else
        buffer = join_and_free(buffer, ft_strdup(""));
    return (buffer);
}

static char *handle_quoted_var(char *buffer, const char *input, int *i)
{
    char	*var_name;

    var_name = extract_quoted_var_name(input, i);
    if (var_name)
        buffer = join_and_free(buffer, var_name);
    else
        buffer = join_and_free(buffer, ft_strdup("$"));
    return (buffer);
}

static char	*handle_regular_var(char *buffer, const char *input, int *i, t_mshell *mshell)
{
    char	*var_name;
    char	*var_value;

    var_name = get_var_name(input, i);
    if (var_name)
    {
        var_value = get_env_value(var_name, mshell);
        buffer = join_and_free(buffer, var_value);
        free(var_name);
    }
    else
        buffer = join_and_free(buffer, ft_strdup("$"));
    return (buffer);
}

static char	*handle_dollar_expansion(char *buffer, const char *input, int *i, t_mshell *mshell)
{
    (*i)++;
    if (input[*i] == '$')
        buffer = handle_pid_expansion(buffer, mshell, i);
    else if (is_quote(input[*i]))
        buffer = handle_quoted_var(buffer, input, i);
    else
        buffer = handle_regular_var(buffer, input, i, mshell);
    return (buffer);
}

char	*expand_env_vars(const char *input, t_mshell *mshell)
{
    char	*buffer;
    int		i;

    if (!input || !mshell)
        return (ft_strdup(""));
    buffer = NULL;
    i = 0;
    while (input[i])
    {
        if (input[i] != '$')
            buffer = append_until_dollar(buffer, input, &i);
        else
            buffer = handle_dollar_expansion(buffer, input, &i, mshell);
    }
    if (!buffer)
        return (ft_strdup(""));
    return (buffer);
}
