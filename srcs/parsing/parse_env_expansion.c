/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_env_expansion.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 14:34:17 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 15:17:50 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*handle_quoted_var(char *buffer, const char *input, int *i)
{
	char	*var_name;

	var_name = extract_quoted_var_name(input, i);
	if (var_name)
		buffer = join_and_free(buffer, var_name);
	else
		buffer = join_and_free(buffer, ft_strdup("$"));
	return (buffer);
}

char	*handle_regular_var(char *buffer,
	const char *input, int *i, t_mshell *mshell)
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

char	*handle_dollar_expansion(char *buffer,
	const char *input, int *i, t_mshell *mshell)
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
