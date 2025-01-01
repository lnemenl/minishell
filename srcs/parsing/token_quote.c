/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/01 19:37:50 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// void    add_quoted_token(t_token **head, t_token **current, const char *input, int *i)
// t_token *handle_quote_content(const char *input, int *i, char quote)
// t_token *handle_double_quote_content(const char *input, int *i, int start)
// int     find_closing_quote(const char *input, int *i, char quote_char)
// t_token *handle_double_quote_env(const char *input, int *i, int start)

void	add_quoted_token(t_token **head, t_token **current, 
						const char *input, int *i)
{
	char    quote;
	t_token *token;

	if (!input || !input[*i])
		return ;
	quote = input[*i];
	token = handle_quote_content(input, i, quote);
	if (!token)
	{
		clean_parse_error(head, current);
		ft_printf("Error: Unmatched quote\n");
		return ;
	}
	link_token(head, current, token);
}

t_token	*handle_quote_content(const char *input, int *i, char quote)
{
	t_token *token;
	int     start;
	int     len;
	char    *content;

	start = *i + 1;
	*i = start;
	if (quote == '"')
		return (handle_double_quote_content(input, i, start));
	if (find_closing_quote(input, i, quote))
		return (NULL);
	len = *i - start;
	(*i)++;
	if (len == 0)
	{
		token = new_token(TOKEN_WORD, "", 0);
		if (token)
			token->quote_state = get_quote_state(quote);
		return (token);
	}
	content = ft_calloc(len + 1, sizeof(char));
	if (!content)
		return (NULL);
	ft_strlcpy(content, &input[start], len + 1);
	token = new_token(TOKEN_WORD, content, len);
	free(content);
	if (!token)
		return (NULL);
	token->quote_state = get_quote_state(quote);
	return (token);
}

t_token	*handle_double_quote_content(const char *input, int *i, int start)
{
    t_token	*token;
    int     len;
    char    *content;
    
    if (!input || !i)
        return (NULL);
    while (input[*i] && input[*i] != '"')
    {
        if (input[*i] == '$')
            return(handle_double_quote_env(input, i, start));
        (*i)++;
    }
    if (!input[*i])
        return (NULL);
    len = *i - start;
    (*i)++;
    if (len == 0)
    {
        token = new_token(TOKEN_WORD, "", 0);
        if (token)
            token->quote_state = QUOTE_DOUBLE;
        return (token);
    }
    content = ft_calloc(len + 1, sizeof(char));
    if (!content)
        return (NULL);
    ft_strlcpy(content, &input[start], len + 1);
    token = new_token(TOKEN_WORD, content, len);
    free(content);
    if (!token)
        return (NULL);
    token->quote_state = QUOTE_DOUBLE;
    return (token);
}

int	find_closing_quote(const char *input, int *i, char quote_char)
{
	if (!input || !input[*i])
		return (1);
	(*i)++;
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (!input[*i])
		return (1);
	return (0);
}

t_token	*handle_double_quote_env(const char *input, int *i, int start)
{
    t_token *token;
    int     len;

    len = *i - start;
    if (len > 0)
    {
        token = new_token(TOKEN_WORD, &input[start], len);
        if (!token)
            return (NULL);
        token->quote_state = QUOTE_DOUBLE;
        return (token);
    }
    return (handle_env_var_token(input, i));
}
