/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_operator_and_word.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 14:26:16 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 15:18:01 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token_type	get_operator_type(const char *input, int *i)
{
	if (input[*i] == '|')
		return (TOKEN_PIPE);
	if (input[*i] == '<')
	{
		if (input[*i + 1] && input[*i + 1] == '<')
		{
			(*i)++;
			return (TOKEN_HEREDOC);
		}
		return (TOKEN_REDIRECT_IN);
	}
	if (input[*i] == '>')
	{
		if (input[*i + 1] && input[*i + 1] == '>')
		{
			(*i)++;
			return (TOKEN_REDIRECT_APPEND);
		}
		return (TOKEN_REDIRECT_OUT);
	}
	return (TOKEN_WORD);
}

t_token	*handle_operator(t_token **head,
			t_token **current, const char *input, int *i)
{
	t_token			*token;
	t_token_type	type;
	int				start;

	if (!head || !*head || !current || !*current || !input || !i)
		return (NULL);
	start = *i;
	type = get_operator_type(input, i);
	token = new_token(type, input + start, (*i - start + 1), (*head)->mshell);
	if (!token)
		return (NULL);
	(*i)++;
	token->mshell = (*current)->mshell;
	link_token(head, current, token);
	while (input[*i] && ft_isspace(input[*i]))
		(*i)++;
	return (token);
}

char	*extract_word(const char *input, int *i)
{
	int		start;
	char	*temp;

	if (!input || !i)
		return (NULL);
	start = *i;
	while (input[*i] && !ft_isspace(input[*i])
		&& !is_operator(input[*i]) && !is_quote(input[*i]))
		(*i)++;
	temp = ft_substr(input, start, (*i) - start);
	return (temp);
}

char	*process_word(char *temp, t_mshell *mshell)
{
	char	*expanded;

	if (!temp || !mshell)
		return (NULL);
	expanded = expand_env_vars(temp, mshell);
	free(temp);
	if (!expanded)
		print_exit("Malloc error\n", NULL, mshell);
	return (expanded);
}

t_token	*handle_word(t_token **head,
	t_token **current, const char *input, int *i)
{
	t_token	*token;
	char	*temp;
	char	*expanded;

	if (!head || !current || !*current || !input || !i)
		return (NULL);
	temp = extract_word(input, i);
	if (!temp)
		return (NULL);
	expanded = process_word(temp, (*current)->mshell);
	if (!expanded)
		return (NULL);
	if (*current && (*current)->quote_state != QUOTE_NONE)
		return (handle_quoted_word(*current, expanded));
	token = new_token(TOKEN_WORD,
			expanded, ft_strlen(expanded), (*head)->mshell);
	if (!token)
	{
		free(expanded);
		return (NULL);
	}
	free(expanded);
	token->mshell = (*current)->mshell;
	link_token(head, current, token);
	return (token);
}
