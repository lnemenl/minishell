/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/05 20:30:10 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*handle_single_quotes(const char *input, int *i, t_mshell *mshell)
{
    t_token *token;
    int     start;
    char    *word;

    (*i)++;  // Skip the opening quote
    start = *i;
    while (input[*i] && input[*i] != '\'')
        (*i)++;
    if (!input[*i])
    {
        ft_putstr_fd("minishell: syntax error: unclosed single quote\n", 2);
        mshell->exit_code = 2;  // Set a specific error code for unclosed quotes
        return (NULL);
    }
    // Creating a substring for the content between single quotes
    word = ft_substr(input, start, (*i) - start);
    if (!word)
        return (NULL);
    token = new_token(TOKEN_WORD, word, ft_strlen(word), mshell);
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
    {
        ft_putstr_fd("minishell: syntax error: unclosed double quote\n", 2);
        mshell->exit_code = 2;  // Specific error code for unclosed quotes
        return (NULL);
    }
    
    content = ft_substr(input, start, *i - start);
    if (!content)
    {
        ft_putstr_fd("minishell: syntax error: unclosed double quote\n", 2);
        mshell->exit_code = 2;  // Set a specific error code for unclosed quotes
        return (NULL);
    }
    
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
    int         merging_with_previous;
    char        quote;
    t_token     *token;
    t_token     *prev_token;
    char        *joined;

    if (!input || !input[*i] || !head || !current || !*current)
        return (NULL);
    // If we are not at the start of the line and the preceding character isn't whitespace,
    // set in word to 1 so that we are still in the same word    
    merging_with_previous = ( (*i > 0) && !ft_isspace(input[*i - 1]) );
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
    if (merging_with_previous && prev_token && prev_token->type == TOKEN_WORD)
    {
        joined = ft_strjoin(prev_token->content, token->content);
        if (!joined)
        {
            free(token->content);
            free(token);
            return (NULL);
        }
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
