/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/03 18:34:43 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// static int	find_closing_quote(const char *input, int *i, char quote)
// {
//     int	start;

//     start = *i;
//     while (input[*i] && input[*i] != quote)
//         (*i)++;
//     if (!input[*i])
//         return (1);
//     return (0);
// }

t_token *handle_single_quotes(const char *input, int *i)
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
    token = new_token(TOKEN_WORD, word, ft_strlen(word));
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
    
    token = new_token(TOKEN_WORD, expanded, ft_strlen(expanded));
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
        token = handle_single_quotes(input, i);
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
