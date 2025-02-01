/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:28:31 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/31 17:45:58 by rkhakimu         ###   ########.fr       */
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
	t_ast_node	*node;
	int			word_count;
	int			i;

	if (!tokens || !*tokens || (*tokens)->type != TOKEN_WORD)
		return (NULL);

	word_count = count_word_tokens(*tokens);
	if (word_count == 0)
		return (NULL);
	node = create_ast_node(TOKEN_WORD);
	if (!node)
		return (NULL);

	node->args = (char **)ft_calloc(word_count + 1, sizeof(char *));
	if (!node->args)
		return (free_ast_return_null(node));

	i = 0;
	while (*tokens && (*tokens)->type == TOKEN_WORD && i < word_count)
	{
		node->args[i] = ft_strdup((*tokens)->content);
		if (!node->args[i])
			return (free_ast_return_null(node));
		i++;
		*tokens = (*tokens)->next;
	}
	return (node);
}
