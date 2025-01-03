/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:11:55 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/03 14:26:17 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char *get_var_name(const char *str, int *i)
{
    int start;
    int len;

    start = *i;
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
    value = getenv(var_name);
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
    free(s1);
    free(s2);
    return (result);
}

char *expand_env_vars(const char *str, t_mshell *mshell)
{
    char *result;
    char *var_name;
    char *var_value;
    int i;
    int start;

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
            var_name = get_var_name(str, &i);
            if (var_name)
            {
                var_value = get_env_value(var_name, mshell);
                result = join_and_free(result, var_value);
                free(var_name);
            }
        }
    }
    if (!result)
        return (ft_strdup(""));
    return (result);
}
