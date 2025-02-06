/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:28:31 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/06 13:09:31 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

  static int count_word_tokens_rec(t_token *token)
  {
      int count = 0;
      
      while (token != NULL && token->type == TOKEN_WORD)
      {
          count++;
          token = token->next;
      }
      return count;
  }

static int build_command_arguments(t_token **tokens, char **args, int index)
{
    if (*tokens != NULL && (*tokens)->type == TOKEN_WORD)
    {
        args[index] = ft_strdup((*tokens)->content);
        if (args[index] == NULL)
            return (0);
        *tokens = (*tokens)->next;
        return (build_command_arguments(tokens, args, index + 1));
    }
    return (1);
}

t_ast_node *build_command_node(t_token **tokens)
{
    t_ast_node *node;
    int word_count;

    if (!tokens || !*tokens || (*tokens)->type != TOKEN_WORD)
        return (NULL);
    word_count = count_word_tokens_rec(*tokens);
    if (word_count == 0)
        return (NULL);
    node = create_ast_node(TOKEN_WORD);
    if (!node)
        return (NULL);
    node->args = (char **)ft_calloc(word_count + 1, sizeof(char *));
    if (!node->args)
        return (free_ast_return_null(node));
    if (!build_command_arguments(tokens, node->args, 0))
        return (free_ast_return_null(node));
    return (node);
}
