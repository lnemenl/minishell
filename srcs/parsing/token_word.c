/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:01:47 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/01 15:19:27 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// void    add_word_token(t_token **head, t_token **current, const char *input, int *i)
// t_token *create_word_token(const char *input, int start, int len)
// int     calculate_word_length(const char *input, int *i)

void	add_word_token(t_token **head, t_token **current, 
					  const char *input, int *i)
{
	int     start;
	int     len;
	t_token *token;

	if (!input || !input[*i])
		return ;
	start = *i;
	len = calculate_word_length(input, i);
	token = create_word_token(input, start, len);
	if (!token)
	{
		clean_parse_error(head, current);
		return ;
	}
	if (!*head)
		*head = token;
	else
		(*current)->next = token;
	*current = token;
}

t_token	*create_word_token(const char *input, int start, int len)
{
	if (!input || len <= 0)
		return (NULL);
	return (new_token(TOKEN_WORD, &input[start], len));
}

int	calculate_word_length(const char *input, int *i)
{
	int start;

	if (!input || !input[*i])
		return (0);
	start = *i;
	while (input[*i] && !ft_isspace(input[*i]) && !is_operator(input[*i]))
		(*i)++;
	return (*i - start);
}
