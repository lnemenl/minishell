/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_env_string_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 14:32:58 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/27 15:15:20 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*remove_quotes(const char *str)
{
	int		len;
	char	*without;

	if (!str)
		return (ft_strdup(""));
	len = ft_strlen(str);
	if ((str[0] == '"' && str[len - 1] == '"')
		|| (str[0] == '\'' && str[len - 1] == '\''))
		without = ft_substr(str, 1, len - 2);
	else
		without = ft_strdup(str);
	return (without);
}

char	*append_until_dollar(char *buffer, const char *input, int *i)
{
	int	start;

	start = *i;
	while (input[*i] && input[*i] != '$')
		(*i)++;
	if (*i > start)
		buffer = join_and_free(buffer, ft_substr(input, start, *i - start));
	return (buffer);
}

char	*handle_pid_expansion(char *buffer, t_mshell *mshell, int *i)
{
	char	*pid_path;

	(*i)++;
	pid_path = get_env_var(mshell->envp, "SESSION_MANAGER=");
	if (pid_path)
	{
		buffer = join_and_free(buffer,
				ft_strdup(ft_strrchr(pid_path, '/') + 1));
		free(pid_path);
	}
	else
		buffer = join_and_free(buffer, ft_strdup(""));
	return (buffer);
}

char	*choose_expand_type(const char *input, t_token *current, char *temp)
{
	char	*expanded;

	if (!ft_strchr(input, '\'') && !ft_strchr(input, '"')
		&& (current && (!current->content
				|| (current->content && !*current->content))))
		expanded = process_word(temp, current->mshell, 1);
	else
		expanded = process_word(temp, current->mshell, 0);
	return (expanded);
}
