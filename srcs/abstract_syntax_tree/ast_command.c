/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:28:31 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/22 14:57:24 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*create_command_node(t_token **tokens)
{
	t_ast_node	*cmd_node;

	(void)tokens;
	cmd_node = create_ast_node(TOKEN_WORD);
	if (!cmd_node)
		return (NULL);
	cmd_node->args = NULL;
	cmd_node->redirs = NULL;
	return (cmd_node);
}

t_ast_node	*assemble_simple_command(t_ast_node *cmd_node, t_token **tokens)
{
	t_mshell	*mshell;

	mshell = (*tokens)->mshell;
	while (*tokens && (*tokens)->type != TOKEN_PIPE)
	{
		if (is_redirect_token((*tokens)->type))
		{
			cmd_node = handle_redir_token(cmd_node, tokens);
			if (!cmd_node)
				return (NULL);
		}
		else if ((*tokens)->type == TOKEN_WORD)
		{
			cmd_node = handle_word_token(cmd_node, tokens);
			if (!cmd_node)
				return (NULL);
		}
		else
			break ;
	}
	return (cmd_node);
}

t_ast_node	*parse_simple_command(t_token **tokens)
{
	t_ast_node	*cmd_node;

	cmd_node = create_command_node(tokens);
	if (!cmd_node)
		return (NULL);
	return (assemble_simple_command(cmd_node, tokens));
}

t_ast_node	*parse_command(t_token **tokens)
{
	t_ast_node	*cmd_node;

	if (!tokens || !*tokens)
		return (NULL);
	if ((*tokens)->type == TOKEN_PIPE)
	{
		if (!validate_pipe(*tokens))
			return (NULL);
	}
	cmd_node = parse_simple_command(tokens);
	return (cmd_node);
}
