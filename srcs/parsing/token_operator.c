/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:06:21 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/01 15:19:17 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// void    add_operator_token(t_token **head, t_token **current, const char *input, int *i)
// t_token *handle_double_redirect(const char *input, int *i)
// t_token *handle_single_operator(const char *input, int *i)

void	add_operator_token(t_token **head, t_token **current, 
						 const char *input, int *i)
{
	t_token *token;

	if (!input || !input[*i])
		return ;
	if (ft_strlen(&input[*i]) >= 2)
		token = handle_double_redirect(input, i);
	else
		token = handle_single_operator(input, i);
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

t_token	*handle_double_redirect(const char *input, int *i)
{
	t_token *token;

	token = NULL;
	if (!ft_strncmp(&input[*i], ">>", 2))
		token = create_token_from_str(">>", TOKEN_REDIRECT_APPEND, 2);
	else if (!ft_strncmp(&input[*i], "<<", 2))
		token = create_token_from_str("<<", TOKEN_HEREDOC, 2);
	if (token)
		(*i) += 2;
	return (token);
}

t_token	*handle_single_operator(const char *input, int *i)
{
	t_token *token;

	token = NULL;
	if (input[*i] == '>')
		token = create_token_from_str(">", TOKEN_REDIRECT_OUT, 1);
	else if (input[*i] == '<')
		token = create_token_from_str("<", TOKEN_REDIRECT_IN, 1);
	else if (input[*i] == '|')
		token = create_token_from_str("|", TOKEN_PIPE, 1);
	if (token)
		(*i)++;
	return (token);
}