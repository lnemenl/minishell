/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_token_quote.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/27 16:12:59 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*handle_single_quotes(const char *input, int *i, t_mshell *mshell)
{
	int		start;
	t_token	*token;
	char	*word;

	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != '\'')
		(*i)++;
	if (!input[*i])
	{
		ft_putstr_fd("syntax error: unclosed single quote\n", STDERR_FILENO);
		mshell->exit_code = 2;
		return (NULL);
	}
	word = ft_substr(input, start, (*i) - start);
	if (!word)
		return (NULL);
	token = new_token(TOKEN_WORD, word, ft_strlen(word), mshell);
	free(word);
	if (!token)
		return (NULL);
	token->quote_state = QUOTE_SINGLE;
	(*i)++;
	return (token);
}

t_token	*handle_double_quotes(const char *input, int *i,
			t_mshell *mshell, t_token_type current_type)
{
	t_quote_data	data;

	if (!input || !i || !mshell)
		return (NULL);
	(*i)++;
	data.input = input;
	data.start = *i;
	data.i = i;
	data.mshell = mshell;
	data.current_type = current_type;
	while (input[*i] && input[*i] != '"')
		(*i)++;
	if (!input[*i])
	{
		ft_putstr_fd("syntax error: unclosed double quote\n", STDERR_FILENO);
		mshell->exit_code = 2;
		return (NULL);
	}
	return (process_quoted_content(&data));
}

t_token	*handle_quotes(t_token **head, t_token **current, const char *input,
	int *i)
{
	int		in_word;
	char	quote;
	t_token	*token;

	if (!input[*i])
		return (NULL);
	in_word = 0;
	if ((*i > 0 && !ft_isspace(input[*i - 1]) && input[*i - 1] != '$')
		|| (*i > 0 && input[*i - 1] == '$'
			&& (*current)->quote_state != QUOTE_NONE))
		in_word = 1;
	quote = input[*i];
	token = process_quote_token(current, input, i, quote);
	if (!token)
		return (NULL);
	return (process_word_token(head, current, token, in_word));
}

t_token	*process_quoted_content(t_quote_data *data)
{
	char	*content;
	char	*expanded;
	t_token	*token;

	content = extract_quoted_content(data->input, data->start, *data->i);
	if (!content)
	{
		data->mshell->exit_code = 2;
		return (NULL);
	}
	expanded = process_content(content, data->current_type, data->mshell);
	if (!expanded)
		return (NULL);
	token = create_quoted_token(expanded, data->input,
			data->start, data->mshell);
	free(expanded);
	(*data->i)++;
	return (token);
}

t_token	*process_quote_token(t_token **current, const char *input,
	int *i, char quote)
{
	t_token	*token;

	if (quote == '"')
		token = handle_double_quotes(input, i, (*current)->mshell,
				(*current)->type);
	else
		token = handle_single_quotes(input, i, (*current)->mshell);
	if (!token)
	{
		if ((*current)->mshell->exit_code != 2)
			(*current)->mshell->exit_code = 1;
		return (NULL);
	}
	if ((*current)->type == TOKEN_HEREDOC)
	{
		(*current)->is_quote_heredoc = 1;
		token->is_quote_heredoc = 1;
	}
	return (token);
}
