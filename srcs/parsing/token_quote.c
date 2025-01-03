/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/03 14:25:56 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	find_closing_quote(const char *input, int *i, char quote)
{
    int	start;

    start = *i;
    while (input[*i] && input[*i] != quote)
        (*i)++;
    if (!input[*i])
        return (1);
    return (0);
}

t_token	*handle_single_quotes(const char *input, int *i)
{
    int		start;
    t_token	*token;

    start = *i;
    if (find_closing_quote(input, i, '\''))
        return (NULL);
    token = new_token(TOKEN_WORD, input + start, *i - start);
    if (!token)
        return (NULL);
    token->quote_state = QUOTE_SINGLE;
    (*i)++;
    return (token);
}

t_token *handle_double_quotes(const char *input, int *i, t_mshell *mshell)
{
    int     start;
    char    *content;
    char    *expanded;
    t_token *token;

    start = *i;
    if (find_closing_quote(input, i, '"'))
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
    (*i)++;
    return (token);
}

t_token *handle_quotes(t_token **head, t_token **current, const char *input, int *i)
{
    char    quote;
    t_token *token;

    if (!input[*i])
        return (NULL);
    quote = input[*i];
    (*i)++;
    if (!input[*i])
    {
        (*current)->mshell->exit_code = 1;
        ft_putendl_fd("minishell: syntax error: unclosed quote", 2);
        return (NULL);
    }
    if (quote == '"')
        token = handle_double_quotes(input, i, (*current)->mshell);
    else
        token = handle_single_quotes(input, i);
    if (!token)
    {
        (*current)->mshell->exit_code = 1;
        return (NULL);
    }
    link_token(head, current, token);
    return (token);
}
