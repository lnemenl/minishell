/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_core.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 14:56:07 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/03 11:33:37 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token *new_token(t_token_type type, const char *content, size_t len)
{
    t_token *token;

    token = ft_calloc(1, sizeof(t_token));
    if (!token)
        return (NULL);
    token->type = type;
    token->content = ft_substr(content, 0, len);
    if (!token->content)
    {
        free(token);
        return (NULL);
    }
    token->quote_state = QUOTE_NONE;
    token->next = NULL;
    return (token);
}

void    link_token(t_token **head, t_token **current, t_token *new)
{
    if (!new)
        return;
    if (!*head)
        *head = new;
    else
        (*current)->next = new;
    *current = new;
}

void    clean_tokens(t_token *head)
{
    t_token *temp;

    while (head)
    {
        temp = head;
        head = head->next;
        free(temp->content);
        free(temp);
    }
}
