/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/20 16:23:05 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*handle_single_quotes(const char *input, int *i, t_mshell *mshell)
{
	int     start;
	t_token *token;
	char    *word;

	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != '\'')
		(*i)++;
	if (!input[*i])
	{
		ft_putstr_fd("syntax error: unclosed single quote\n", 2);
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

static char	*extract_quoted_content(const char *input, int start, int end)
{
    char	*content;

    content = ft_substr(input, start, end - start);
    if (!content)
    {
        ft_putstr_fd("syntax error: unclosed double quote\n", 2);
        return (NULL);
    }
    return (content);
}

static char	*process_content(char *content, t_token_type type, t_mshell *mshell)
{
    char	*expanded;

    if (type == TOKEN_HEREDOC)
        return (content);
    expanded = expand_env_vars(content, mshell);
    free(content);
    return (expanded);
}

static t_token	*create_quoted_token(char *expanded, const char *input, 
                                  int start, t_mshell *mshell)
{
    t_token	*token;

    token = new_token(TOKEN_WORD, expanded, ft_strlen(expanded), mshell);
    if (!token)
        return (NULL);
    if (ft_strncmp(&input[start], "-n", 2) == 0)
        token->is_quote_heredoc = 1;
    token->quote_state = QUOTE_DOUBLE;
    return (token);
}

static t_token	*process_quoted_content(t_quote_data *data)
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
    token = create_quoted_token(expanded, data->input, data->start, data->mshell);
    free(expanded);
    (*data->i)++;
    return (token);
}

t_token	*handle_double_quotes(const char *input, int *i, t_mshell *mshell, t_token_type current_type)
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
        ft_putstr_fd("syntax error: unclosed double quote\n", 2);
        mshell->exit_code = 2;
        return (NULL);
    }
    return (process_quoted_content(&data));
}

static t_token	*join_word_token(t_token *prev_token, t_token *token)
{
    char	*joined;

    if (ft_strcmp(prev_token->content, "$") == 0)
    {
        free(prev_token->content);
        prev_token->content = ft_strdup(token->content);
        prev_token->quote_state = token->quote_state;
        free(token->content);
        free(token);
        return (prev_token);
    }
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

static t_token	*handle_word_token(t_token **head, t_token **current,
	t_token *token, int in_word)
{
    t_token	*prev_token;

    prev_token = *current;
    if (in_word && prev_token && prev_token->type == TOKEN_WORD)
        return (join_word_token(prev_token, token));
    link_token(head, current, token);
    return (token);
}

static t_token	*process_quote_token(t_token **current, const char *input,
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

t_token	*handle_quotes(t_token **head, t_token **current, const char *input,
	int *i)
{
    int		in_word;
    char	quote;
    t_token	*token;

    if (!input[*i])
        return (NULL);
    in_word = 0;
    if (*i > 0 && !ft_isspace(input[*i - 1]))
        in_word = 1;
    quote = input[*i];
    token = process_quote_token(current, input, i, quote);
    if (!token)
        return (NULL);
    return (handle_word_token(head, current, token, in_word));
}
