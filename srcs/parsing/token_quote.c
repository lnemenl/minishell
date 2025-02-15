/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/15 16:02:11 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*handle_single_quotes(const char *input, int *i, t_mshell *mshell)
{
	int     start;
	t_token *token;
	char    *without_backslashes;
	char    *word;

	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != '\'')
		(*i)++;
	if (!input[*i])
	{
		ft_putstr_fd("minishell: syntax error: unclosed single quote\n", 2);
		mshell->exit_code = 2;
		return (NULL);
	}
	word = ft_substr(input, start, (*i) - start);
	if (!word)
		return (NULL);
	without_backslashes = handle_backslash(word);
	if (!without_backslashes)
	{
		free(word);
		return (NULL);
	}
	free(word);
	token = new_token(TOKEN_WORD, without_backslashes, ft_strlen(without_backslashes), mshell);
	free(without_backslashes);
	if (!token)
		return (NULL);
	token->quote_state = QUOTE_SINGLE;
	(*i)++;
	return (token);
}

t_token *handle_double_quotes(const char *input, int *i, t_mshell *mshell, t_token_type current_type)
{
	int     start;
	char    *content;
	char    *without_backslashes;
	char    *expanded;
	t_token *token;

	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != '"')
		(*i)++;
	if (!input[*i])
	{
		ft_putstr_fd("minishell: syntax error: unclosed double quote\n", 2);
		mshell->exit_code = 2;
		return (NULL);
	}
	content = ft_substr(input, start, *i - start);
	if (!content)
	{
		ft_putstr_fd("minishell: syntax error: unclosed double quote\n", 2);
		mshell->exit_code = 2;
		return (NULL);
	}
	without_backslashes = handle_backslash(content);
	if (!without_backslashes)
	{
		free(content);
		return (NULL);
	}
	free(content);
	if (current_type == TOKEN_HEREDOC)
		expanded = without_backslashes;
	else
	{
		expanded = expand_env_vars(without_backslashes, mshell);
		free(without_backslashes);
		if (!expanded)
			return (NULL);
	}
	token = new_token(TOKEN_WORD, expanded, ft_strlen(expanded), mshell);
	free(expanded);
	if (!token)
		return (NULL);
	token->quote_state = QUOTE_DOUBLE;
	(*i)++;
	return (token);
}

t_token	*handle_quotes(t_token **head, t_token **current, const char *input, int *i)
{
	int in_word;
	char quote;
	char *joined;
	t_token *token;
	t_token *prev_token;

	if (!input[*i])
		return (NULL);
	in_word = 0;
	if (*i > 0 && !ft_isspace(input[*i - 1]))
		in_word = 1;

	prev_token = *current;
	quote = input[*i];
	if (quote == '"')
		token = handle_double_quotes(input, i, (*current)->mshell, (*current)->type);
	else
		token = handle_single_quotes(input, i, (*current)->mshell);
	if (!token)
	{
		(*current)->mshell->exit_code = 1;
		return (NULL);
	}
	if ((*current)->type == TOKEN_HEREDOC)
	{
		(*current)->is_quote_heredoc = 1;
		token->is_quote_heredoc = 1;
	}
	if (in_word && prev_token && prev_token->type == TOKEN_WORD)
	{
		if (ft_strcmp(prev_token->content, "$") == 0)
		{
			free(prev_token->content);
			prev_token->content = ft_strdup(token->content);
			prev_token->quote_state = token->quote_state;
			free(token->content);
			free(token);
			return (prev_token);
		}
		else
		{
			joined = ft_strjoin(prev_token->content, token->content);
			if (!joined)
				return (NULL);
			free(prev_token->content);
			prev_token->content = joined;
			prev_token->quote_state = token->quote_state;
			free(token->content);
			free(token);
			return (prev_token);
		}
	}
	link_token(head, current, token);
	return (token);
}
