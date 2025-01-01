/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/01 18:32:47 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

static t_ast_node	*free_ast_return_null(t_ast_node *node);
t_ast_node			*parse_command(t_token **tokens);

t_ast_node	*create_ast_node(t_token_type type)
{
	t_ast_node	*node;
	
	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

static t_ast_node	*free_ast_return_null(t_ast_node *node)
{
	free_ast(node);
	return (NULL);
}

t_ast_node	*parse_pipeline(t_token **tokens)
{
	t_ast_node	*left;
	t_ast_node	*pipe_node;

	if (!tokens || !*tokens)
		return (NULL);
	left = parse_command(tokens);
	if (!left)
		return (NULL);
	if (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		pipe_node = create_ast_node(TOKEN_PIPE);
		if (!pipe_node)
			return (free_ast_return_null(left));
		*tokens = (*tokens)->next;
		pipe_node->left = left;
		pipe_node->right = parse_pipeline(tokens);
		return (pipe_node);
	}
	return (left);
}

t_ast_node	*parse_command(t_token **tokens)
{
	t_ast_node    *cmd_node;

	if (!tokens || !*tokens)
		return (NULL);
	if ((*tokens)->type != TOKEN_WORD)
		return (NULL);
	cmd_node = build_command_node(tokens);
	if (!cmd_node)
		return (NULL);
	return (handle_redirection(tokens, cmd_node));
}

void	free_ast(t_ast_node *node)
{
	int	i;
	
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->args)
	{
		i = 0;
		while (node->args[i])
			free(node->args[i++]);
		free(node->args);
	}
	free(node);
}
