/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/07 20:44:00 by rkhakimu         ###   ########.fr       */
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

t_ast_node *parse_pipeline(t_token **tokens)
{
    t_ast_node *left;
    t_ast_node *pipe_node;

    left = parse_command(tokens);
    if (!left)
        return (NULL);
    while (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        if (!(*tokens)->next)
        {
            ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
            return (free_ast_return_null(left));
        }
        pipe_node = create_ast_node(TOKEN_PIPE);
        if (!pipe_node)
            return (free_ast_return_null(left));
        *tokens = (*tokens)->next;   
        pipe_node->left = left;
        pipe_node->right = parse_command(tokens);
        if (!pipe_node->right)
            return (free_ast_return_null(pipe_node));
        left = pipe_node;
    }
    return (left);
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
    return (redir);
}

static t_ast_node *handle_initial_redirection(t_token **tokens) //Handles redirections that appear before commands
{
    t_ast_node *redir;

    redir = handle_redirection_node(tokens);
    if (!redir)
        return (NULL);
    redir->right = parse_command(tokens);
    if (!redir->right)
        return (free_ast_return_null(redir));
    return (redir);
}

static t_ast_node *handle_command_redirections(t_token **tokens, t_ast_node *cmd_node) //Handles redirections after a command
{
    t_ast_node *redir;

    while (*tokens && is_redirect_token((*tokens)->type))
    {
        if ((*tokens)->next && is_redirect_token((*tokens)->next->type))
        {
            ft_putstr_fd("syntax error near unexpected token `", 2);
            ft_putstr_fd((*tokens)->next->content, 2);
            ft_putstr_fd("'\n", 2);
            return (free_ast_return_null(cmd_node));
        }
        redir = handle_redirection_node(tokens);
        if (!redir)
            return (free_ast_return_null(cmd_node));
        redir->left = cmd_node;
        cmd_node = redir;
    }
    return (cmd_node);
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
