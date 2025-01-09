/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/09 17:16:04 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
			type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}

t_ast_node	*free_ast_return_null(t_ast_node *node)
{
	free_ast(node);
	return (NULL);
}

t_ast_node	*create_ast_node(t_token_type type)
{
	t_ast_node	*node;
	
	node = calloc(1, sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	return (node);
}

static t_ast_node *handle_redirection_node(t_token **tokens) //Creates and sets up a single redirection node
{
    t_ast_node *redir;

    redir = create_ast_node((*tokens)->type);
    if (!redir)
        return (NULL);
    *tokens = (*tokens)->next;
    if (!*tokens || (*tokens)->type != TOKEN_WORD)
    {
        ft_putstr_fd("syntax error near unexpected token `newline'\n", 2);
        return (free_ast_return_null(redir));
    }
    redir->args = ft_calloc(2, sizeof(char *));
    if (!redir->args)
        return (free_ast_return_null(redir));
    redir->args[0] = ft_strdup((*tokens)->content);
    if (!redir->args[0])
        return (free_ast_return_null(redir));
    *tokens = (*tokens)->next;
    if (redir->type == TOKEN_HEREDOC)
        redir->left = NULL;
    return (redir);
}

static t_ast_node *handle_initial_redirection(t_token **tokens) //Handles redirections that appear before commands
{
    t_ast_node *redir;

    redir = handle_redirection_node(tokens);
    if (!redir)
        return (NULL);
    redir->right = NULL;
    redir->left = parse_command(tokens);
    if (!redir->left)
        return (free_ast_return_null(redir));
    return (redir);
}

static t_ast_node *handle_command_redirections(t_token **tokens, t_ast_node *cmd_node) //Handles redirections after a command
{
    t_ast_node *redir;
    t_ast_node *current;
    t_token_type last_redir_type;

    current = cmd_node;
    last_redir_type = 0;
    while (*tokens && is_redirect_token((*tokens)->type))
    {
        // Check for consecutive redirections of the same type
        if (last_redir_type == (*tokens)->type)
        {
            ft_putstr_fd("syntax error: multiple redirections of same type\n", 2);
            free_ast(current);
            return (NULL);
        }
        last_redir_type = (*tokens)->type;
        redir = handle_redirection_node(tokens);
        if (!redir)
        {
            free_ast(current);
            return (NULL);
        }
        redir->left = current;
        current = redir;
    }
    return (current);
}

t_ast_node *parse_command(t_token **tokens)
{
    t_ast_node *cmd_node;

    if (!tokens || !*tokens)
        return (NULL);
    if ((*tokens)->type == TOKEN_PIPE)
    {
        ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
        return (NULL);
    }
    if (is_redirect_token((*tokens)->type))
        return (handle_initial_redirection(tokens));

    cmd_node = build_command_node(tokens);
    if (!cmd_node || !cmd_node->args)
        return (NULL);

    return (handle_command_redirections(tokens, cmd_node));
}

t_ast_node *parse_pipeline(t_token **tokens)
{
    t_ast_node *root;
    t_ast_node *current;
    t_ast_node *pipe_node;

    root = parse_command(tokens);
    if (!root)
        return (NULL);
    current = root;
    while (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        if (!(*tokens)->next || (*tokens)->next->type == TOKEN_PIPE)
        {
            ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
            return (free_ast_return_null(root));
        }
        pipe_node = create_ast_node(TOKEN_PIPE);
        if (!pipe_node)
            return (free_ast_return_null(root));
        *tokens = (*tokens)->next;
        pipe_node->left = current;

        if (*tokens && is_redirect_token((*tokens)->type))
        {
            t_ast_node *redir = handle_redirection_node(tokens);
            if (!redir)
                return (free_ast_return_null(pipe_node));
            pipe_node->right = redir;
        }
        else
            pipe_node->right = parse_command(tokens);

        if (!pipe_node->right)
            return (free_ast_return_null(pipe_node));

        current = pipe_node;
        root = pipe_node;
    }
    return (root);
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
