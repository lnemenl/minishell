/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 14:03:14 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*create_ast_node(t_token_type type)
{
	t_ast_node	*node;

	node = (t_ast_node *)ft_calloc(1, sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = NULL;
	node->redirs = NULL;
	node->left = NULL;
	node->right = NULL;
	node->is_quote_heredoc = 0;
	return (node);
}

char	**append_arg(char **args, char *new_arg)
{
	int		count;
	char	**new_args;
	int		i;

	count = 0;
	if (args)
	{
		while (args[count])
			count++;
	}
	new_args = (char **)ft_calloc(count + 2, sizeof(char *));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[count] = new_arg;
	free(args);
	return (new_args);
}

t_ast_node	*handle_word_token(t_ast_node *cmd_node, t_token **tokens)
{
	char	*arg;

	if (ft_strncmp((*tokens)->content, "-n", 2) == 0)
		cmd_node->is_quote_heredoc = (*tokens)->is_quote_heredoc;
	arg = ft_strdup((*tokens)->content);
	if (!arg)
		return (free_ast_return_null(&cmd_node));
	cmd_node->args = append_arg(cmd_node->args, arg);
	if (!cmd_node->args)
	{
		free(arg);
		return (free_ast_return_null(&cmd_node));
	}
	*tokens = (*tokens)->next;
	return (cmd_node);
}

t_ast_node	*create_pipe_structure(t_ast_node *root, t_token **tokens)
{
	t_ast_node	*pipe_node;

	pipe_node = create_ast_node(TOKEN_PIPE);
	if (!pipe_node)
	{
		free_ast(root);
		return (NULL);
	}
	*tokens = (*tokens)->next;
	pipe_node->left = root;
	pipe_node->right = parse_pipeline(tokens);
	if (!pipe_node->right)
	{
		free_ast(pipe_node);
		return (NULL);
	}
	return (pipe_node);
}

t_ast_node	*parse_pipeline(t_token **tokens)
{
	t_ast_node	*root;

	root = parse_command(tokens);
	if (!root)
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		if (!validate_pipe(*tokens))
		{
			free_ast(root);
			return (NULL);
		}
		root = create_pipe_structure(root, tokens);
		if (!root)
			return (NULL);
	}
	return (root);
}
