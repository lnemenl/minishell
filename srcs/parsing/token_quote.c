/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/07 21:52:15 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*handle_single_quotes(const char *input, int *i, t_mshell *mshell)
{
    int     start;
    t_token *token;
    char    *word;

    (*i)++;  // Skip the opening quote
    start = *i;
    while (input[*i] && input[*i] != '\'')
        (*i)++;
    if (!input[*i])
        return (NULL);
    word = ft_substr(input, start, *i - start);
    if (!word)
        return (NULL);
    token = new_token(TOKEN_WORD, input + start, *i - start, mshell);
    free(word);
    if (!token)
        return (NULL);
    token->quote_state = QUOTE_SINGLE;
    (*i)++;  // Skip the closing quote
    return (token);
}

t_token *handle_double_quotes(const char *input, int *i, t_mshell *mshell)
{
    int     start;
    char    *content;
    char    *expanded;
    t_token *token;

    (*i)++;  // Skip opening quote
    start = *i;
    while (input[*i] && input[*i] != '"')
        (*i)++;
    if (!input[*i])
        return (NULL);
    
    content = ft_substr(input, start, *i - start);
    if (!content)
        return (NULL);
    
    expanded = expand_env_vars(content, mshell);
    free(content);
    if (!expanded)
        return (NULL);
    token = new_token(TOKEN_WORD, expanded, ft_strlen(expanded), mshell);
    free(expanded);
    if (!token)
        return (NULL);
    
    token->quote_state = QUOTE_DOUBLE;
    (*i)++;  // Skip closing quote
    return (token);
}

t_token *handle_quotes(t_token **head, t_token **current, const char *input, int *i)
{
    static int  in_word;
    char        quote;
    t_token     *token;
    t_token     *prev_token;

    if (!input[*i])
        return (NULL);
    in_word = (!ft_isspace(input[*i - 1]));
    prev_token = *current;
    quote = input[*i];
    if (quote == '"')
        token = handle_double_quotes(input, i, (*current)->mshell);
    else
        token = handle_single_quotes(input, i, (*current)->mshell);
    if (!token)
    {
        (*current)->mshell->exit_code = 1;
        return (NULL);
    }
    if (in_word && prev_token && prev_token->content)
    {
        char *joined = ft_strjoin(prev_token->content, token->content);
        if (!joined)
            return (NULL);
        free(prev_token->content);
        prev_token->content = joined;
        prev_token->quote_state = token->quote_state;
        free(token->content);
        free(token);
        return prev_token;
    }
    link_token(head, current, token);
    return (token);
}
