/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:28:31 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/03 13:34:39 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int count_word_tokens(t_token *token)
{
    int count;

    count = 0;
    while (token && token->type == TOKEN_WORD)
    {
        count++;
        token = token->next;
    }
    return (count);
}

t_ast_node *build_command_node(t_token **tokens)
{
    t_ast_node  *node;
    int         arg_count;
    int         i;
    t_token     *current;

    if (!tokens || !*tokens)
        return (NULL);
    node = ft_calloc(1, sizeof(t_ast_node));
    if (!node)
        return (NULL);
    node->type = TOKEN_WORD;
    arg_count = count_word_tokens(*tokens);
    node->args = ft_calloc(arg_count + 1, sizeof(char *));
    if (!node->args)
    {
        free(node);
        return (NULL);
    }
    i = 0;
    current = *tokens;
    while (i < arg_count)
    {
        node->args[i] = ft_strdup(current->content);
        if (!node->args[i])
        {
            while (--i >= 0)
                free(node->args[i]);
            free(node->args);
            free(node);
            return (NULL);
        }
        current = current->next;
        i++;
    }
    node->args[arg_count] = NULL;
    *tokens = current;
    return (node);
}
