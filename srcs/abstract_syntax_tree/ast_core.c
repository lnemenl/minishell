/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/06 11:37:25 by rkhakimu         ###   ########.fr       */
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
	
	node = (t_ast_node *)ft_calloc(1, sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	return (node);
}

static int validate_redirection_chain(t_ast_node *node)
{
	t_ast_node  *current;
	
	current = node;
	while (current && is_redirect_token(current->type))
	{
		if (!current->args || !current->args[0])
			return (0);
		current = current->left;
	}
	return (1);
}

static t_ast_node *handle_redirection_node(t_token **tokens)
{
	t_ast_node	*redir;
	char		*filename;
	char		*temp;

	if (!tokens || !*tokens)
		return NULL;

	redir = create_ast_node((*tokens)->type);
	if (!redir)
		return NULL;

	*tokens = (*tokens)->next;
	
	// Handle concatenated quoted strings
	filename = ft_strdup("");
	if (!filename)
		return(free_ast_return_null(redir));
	while (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		temp = filename;
		filename = ft_strjoin(filename, (*tokens)->content);
		free(temp);
		if (!filename)
			return (free_ast_return_null(redir));
		*tokens = (*tokens)->next;
	}

	redir->args = (char **)ft_calloc(2, sizeof(char *));
	if (!redir->args)
	{
		free(filename);
		return (free_ast_return_null(redir));
	}

	redir->args[0] = filename;
	return redir;
}

static t_ast_node *handle_initial_redirection(t_token **tokens) //Handles redirections that appear before commands
{
	t_ast_node	*redir;

	redir = handle_redirection_node(tokens);
	if (!redir)
		return (NULL);
	redir->right = NULL;
	if (!redir->left)
		redir->left = parse_command(tokens);
	if (!redir->left)
		return (free_ast_return_null(redir));
	return (redir);
}

t_ast_node	*handle_command_redirections(t_token **tokens, t_ast_node *cmd_node)
{
	t_ast_node	*redir;
	t_ast_node	*current;
	t_ast_node	*last_redir;
	
	last_redir = NULL;
	current = cmd_node;
	
	while (*tokens && is_redirect_token((*tokens)->type))
	{
		// Create new redirection node
		redir = handle_redirection_node(tokens);
		if (!redir)
		{
			free_ast(current);
			return (NULL);
		}

		// Chain the redirections
		if (!last_redir)
		{
			// First redirection
			redir->left = current;
			current = redir;
		}
		else
		{
			// Subsequent redirections
			redir->left = current->left;
			current->left = redir;
		}
		last_redir = redir;
	}
	return (current);
}

t_ast_node	*parse_command(t_token **tokens)
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
	{
		cmd_node = handle_initial_redirection(tokens);
		if (!cmd_node || !validate_redirection_chain(cmd_node))
		{
			free_ast(cmd_node);
			return (NULL);
		}
		return (cmd_node);
	}

	cmd_node = build_command_node(tokens);
	if (!cmd_node || !cmd_node->args)
		return (NULL);

	cmd_node = handle_command_redirections(tokens, cmd_node);
	if (!cmd_node || !validate_redirection_chain(cmd_node))
	{
		free_ast(cmd_node);
		return (NULL);
	}

	return (cmd_node);
}

t_ast_node *parse_pipeline(t_token **tokens, int i, t_mshell *obj)
{
	t_ast_node	*root;
	t_ast_node	*pipe_node;

	root = parse_command(tokens);
	if (!root)
		return (NULL);
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
		pipe_node->left = root; // Current command goes left
		i++;
		pipe_node->right = parse_pipeline(&(*tokens), i, obj); //Recursive call for the remaining pipeline
		if (!pipe_node->right)
			return (free_ast_return_null(pipe_node));
		root = pipe_node; // Update root
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
