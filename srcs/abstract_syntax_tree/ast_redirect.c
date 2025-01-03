/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redirect.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:30:06 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/03 19:03:07 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
			type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}

static t_ast_node	*create_redirect_node(t_token **tokens)
{
	t_ast_node	*node;

	node = create_ast_node((*tokens)->type);
	if (!node)
		return (NULL);
	node->args = malloc(sizeof(char *) * 2);
	if (!node->args)
	{
		free(node);
		return (NULL);
	}
	return (node);
}

static int	setup_redirect_node(t_ast_node *node, t_token **tokens,
								t_ast_node *cmd)
{
	if (!*tokens || (*tokens)->type != TOKEN_WORD)
	{
		free_ast(node);
		return (0);
	}
	node->args[0] = ft_strdup((*tokens)->content);
	node->args[1] = NULL;
	if (!node->args[0])
	{
		free_ast(node);
		return (0);
	}
	node->left = cmd;
	return (1);
}

t_ast_node    *handle_redirection(t_token **tokens, t_ast_node *cmd_node)
{
    t_ast_node    *redir_node;

    if (!*tokens)
        return (cmd_node);
    if (!is_redirect_token((*tokens)->type))
    {
        // If there is a command word after redirection
        if ((*tokens)->type == TOKEN_WORD && !cmd_node->args)
        {
            t_ast_node *new_cmd = build_command_node(tokens);
            if (!new_cmd)
                return (NULL);
            cmd_node->args = new_cmd->args;
            new_cmd->args = NULL;
            free_ast(new_cmd);
        }
        return (cmd_node);
    }
    redir_node = create_redirect_node(tokens);
    if (!redir_node)
        return (NULL);
    *tokens = (*tokens)->next;
    if (!setup_redirect_node(redir_node, tokens, cmd_node))
        return (NULL);
    *tokens = (*tokens)->next;
    return (handle_redirection(tokens, redir_node));
}
