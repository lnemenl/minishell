/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:28:31 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/05 20:30:05 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// static int count_word_tokens(t_token *token)
// {
// 	int count;

// 	count = 0;
// 	while (token && token->type == TOKEN_WORD)
// 	{
// 		count++;
// 		token = token->next;
// 	}
// 	return (count);
// }

t_ast_node *build_command_node(t_token **tokens)
{
    t_ast_node *cmd_node;
    char       *word_copy;

    if (!tokens || !*tokens)
        return (NULL);
    if ((*tokens)->type != TOKEN_WORD)
        return (NULL);

    cmd_node = create_ast_node(TOKEN_WORD);
    if (!cmd_node)
        return (NULL);

    word_copy = ft_strdup((*tokens)->content);
    if (!word_copy)
        return (free_ast_return_null(cmd_node));

    cmd_node->args = (char **)ft_calloc(2, sizeof(char *));
    if (!cmd_node->args)
    {
        free(word_copy);
        return (free_ast_return_null(cmd_node));
    }
    cmd_node->args[0] = word_copy;

    *tokens = (*tokens)->next;
    return (cmd_node);
}
