/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_token_process.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 11:31:38 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/22 10:00:06 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*handle_quoted_word(t_token *current, char *expanded)
{
	char	*joined;

	if (!current || !expanded)
		return (NULL);
	joined = ft_strjoin(current->content, expanded);
	free(expanded);
	if (!joined)
		return (NULL);
	free(current->content);
	current->content = joined;
	return (current);
}

t_token	*process_token(t_token **head,
	t_token **current, const char *input, int *i)
{
	if (!head || !current || !*current || !input || !i)
		return (NULL);
	if (ft_isspace(input[*i]))
	{
		(*current)->quote_state = QUOTE_NONE;
		(*head)->quote_state = QUOTE_NONE;
		(*i)++;
		return (*current);
	}
	if (is_quote(input[*i]))
		return (handle_quotes(head, current, input, i));
	if (is_operator(input[*i]))
		return (handle_operator(head, current, input, i));
	return (handle_word(head, current, input, i));
}
