/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/05 13:54:04 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static t_ast_node *parse_additional_words(t_ast_node *cmd_node, t_token **tokens);
static t_ast_node *build_and_expand_command(t_token **tokens);
static int validate_redirection_chain(t_ast_node *node);
static t_ast_node *handle_initial_redirection(t_token **tokens);

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

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
			type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}

static t_ast_node *get_command_leaf(t_ast_node *node)
{
    t_ast_node *current = node;

    while (current && is_redirect_token(current->type) && current->left)
        current = current->left;
    return current;
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

static t_ast_node *parse_single_redirection(t_token **tokens)
{
	t_ast_node	*redir;
	char		*filename;

	if (!tokens || !*tokens)
		return NULL;

	redir = create_ast_node((*tokens)->type);
	if (!redir)
		return NULL;

	*tokens = (*tokens)->next;
	
	if (*tokens && (*tokens)->type == TOKEN_WORD)
	{
		filename = ft_strdup((*tokens)->content);
		if (!filename)
			return (free_ast_return_null(redir));
		*tokens = (*tokens)->next;
	}
	else
	{
		return (free_ast_return_null(redir));
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

static t_ast_node *parse_redirections(t_token **tokens, t_ast_node *base)
{
    t_ast_node  *redir;
    while (*tokens && is_redirect_token((*tokens)->type))
    {
        redir = parse_single_redirection(tokens);
        if (!redir)
            return (free_ast_return_null(base));
    }
    redir->left = base;
    base = redir;
}
// static t_ast_node *handle_initial_redirection(t_token **tokens)
// {
//     t_ast_node *redir = handle_redirection_node(tokens);
//     if (!redir)
//         return NULL;

//     // Parse the command that follows as the left child
//     redir->left = parse_command(tokens);
//     if (!redir->left)
//         return free_ast_return_null(redir);

//     return redir;
// }

t_ast_node *handle_command_redirections(t_token **tokens, t_ast_node *cmd_node)
{
    t_ast_node *redir;
    t_ast_node *top;

    top = cmd_node;

    while (*tokens && is_redirect_token((*tokens)->type))
    {
        redir = handle_redirection_node(tokens);
        if (!redir)
        {
            free_ast(top);
            return NULL;
        }
        redir->left = top;
        top = redir;
    }
    return top;
}


static t_ast_node *parse_additional_words(t_ast_node *cmd_node, t_token **tokens)
{
    int     len;
    char    **new_args;
    int     i;
    
    while (*tokens && (*tokens)->type == TOKEN_WORD)
    {

        /* Count how many args we have so far */
        len = 0;
        while (cmd_node->args && cmd_node->args[len])
            len++;

        /* Allocate space for the old args + 1 new arg + NULL terminator */
        new_args = (char **)ft_calloc(len + 2, sizeof(char *));
        if (!new_args)
            return free_ast_return_null(cmd_node);

        /* Copy existing pointers into new_args */
        i = 0;
        while (i < len)
        {
            new_args[i] = cmd_node->args[i];
            i++;
        }

        /* Duplicate the token content as the new argument */
        new_args[len] = ft_strdup((*tokens)->content);
        if (!new_args[len])
        {
            free(new_args);
            return free_ast_return_null(cmd_node);
        }

        /* Replace the old args array with the new one */
        free(cmd_node->args);
        cmd_node->args = new_args;

        /* Advance the token stream */
        *tokens = (*tokens)->next;
    }

    return cmd_node;
}


static t_ast_node *build_and_expand_command(t_token **tokens)
{
    t_ast_node	*cmd_node;
	t_ast_node	*leaf;

    cmd_node = build_command_node(tokens);
    if (!cmd_node || !cmd_node->args)
        return (NULL);

    cmd_node = handle_command_redirections(tokens, cmd_node);
    if (!cmd_node)
	{
        return (NULL);
	}
	leaf = get_command_leaf(cmd_node);
    if (leaf && leaf->type == TOKEN_WORD && *tokens && (*tokens)->type == TOKEN_WORD)
	{
		leaf = parse_additional_words(leaf, tokens);
		if (!leaf)
			return (free_ast_return_null(cmd_node));
	}
    return cmd_node;
}

t_ast_node *parse_command(t_token **tokens)
{
    t_ast_node  *cmd_node;
    t_ast_node  *command_part;

    if (!tokens || !*tokens)
        return NULL;

    if ((*tokens)->type == TOKEN_PIPE)
    {
        ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
        return NULL;
    }
    cmd_node = parse_redirections(tokens, NULL);
    if (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        command_part = build_command_node(tokens);
        if (!command_part)
            return (free_ast_return_null(cmd_node));
        if (cmd_node == NULL)
            cmd_node = command_part;
        else
        {
            cmd_node->left = command_part;
        }
        command_part = parse_additional_words(command_part, tokens);
        if (!command_part)
            return (NULL);
    }
    cmd_node = parse_redirections(tokens, cmd_node);
    if (!cmd_node || !validate_redirection_chain(cmd_node))
        return (free_ast_return_null(cmd_node));
    return(cmd_node);
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
