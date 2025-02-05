/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/05 21:02:51 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*free_ast_return_null(t_ast_node *node)
{
	free_ast(node);
	return (NULL);
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
	if (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND
		|| type == TOKEN_HEREDOC)
		return (1);
	return (0);
}



static t_ast_node *parse_additional_words(t_ast_node *cmd_node, t_token **tokens)
{
    char **new_args;
    int   old_count;
    int   i;

    if (!cmd_node || !tokens)
        return (cmd_node);

    while (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        /* Count how many arguments we have so far. */
        old_count = 0;
        while (cmd_node->args && cmd_node->args[old_count])
            old_count++;

        /* Allocate space for old_count + 1 new + 1 for NULL terminator. */
        new_args = (char **)ft_calloc(old_count + 2, sizeof(char *));
        if (!new_args)
            return (free_ast_return_null(cmd_node));

        /* Copy over the existing arguments. */
        i = 0;
        while (i < old_count)
        {
            new_args[i] = cmd_node->args[i];
            i++;
        }

        /* Duplicate the new token and append. */
        new_args[old_count] = ft_strdup((*tokens)->content);
        if (!new_args[old_count])
        {
            free(new_args);
            return (free_ast_return_null(cmd_node));
        }

        /* Switch out the old args. */
        free(cmd_node->args);
        cmd_node->args = new_args;

        /* Advance the token stream. */
        *tokens = (*tokens)->next;
    }
    return (cmd_node);
}

static t_ast_node *parse_single_redirection(t_token **tokens)
{
    t_ast_node *redir_node;
    char       *filename;

    if (!tokens || !*tokens)
        return (NULL);

    /* redir_node = '>' or '<' or '>>' or '<<'. */
    redir_node = create_ast_node((*tokens)->type);
    if (!redir_node)
        return (NULL);

    /* Advance past the operator token. */
    *tokens = (*tokens)->next;

    /* The next token must be a TOKEN_WORD for the filename. */
    if (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        filename = ft_strdup((*tokens)->content);
        if (!filename)
            return (free_ast_return_null(redir_node));
        *tokens = (*tokens)->next;
    }
    else
    {
        /* No valid filename => invalid redirection. */
        return (free_ast_return_null(redir_node));
    }

    redir_node->args = (char **)ft_calloc(2, sizeof(char *));
    if (!redir_node->args)
    {
        free(filename);
        return (free_ast_return_null(redir_node));
    }
    redir_node->args[0] = filename;
    return (redir_node);
}

static t_ast_node *parse_redirections(t_token **tokens, t_ast_node *base)
{
    t_ast_node *redir;

    while (*tokens && is_redirect_token((*tokens)->type))
    {
        redir = parse_single_redirection(tokens);
        if (!redir)
            return (free_ast_return_null(base));
        redir->left = base;
        base = redir;
    }
    return (base);
}


static int validate_redirection_chain(t_ast_node *node)
{
	t_ast_node  *current;
	
	current = node;
	while (current && is_redirect_token(current->type))
	{
		if (!current->args || !current->args[0])
			return (0);
		if (!current->left)
			return (0);
		current = current->left;
	}
	if (!current)
		return(0);
	return (1);
}

t_ast_node *parse_command(t_token **tokens)
{
    t_ast_node	*cmd_root;
    t_ast_node	*cmd_node;
	t_ast_node	*real_cmd;

    /* 1) Parse any leading redirections (e.g. < file). */
    cmd_root = parse_redirections(tokens, NULL);

    /* 2) If the next token is a word, that's our command. */
    if (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        cmd_node = build_command_node(tokens);
        if (!cmd_node)
            return (free_ast_return_null(cmd_root));
        /* Attach the command node beneath any leading redirections. */
        if (!cmd_root)
            cmd_root = cmd_node;
        else
            cmd_root->left = cmd_node;

        /* Parse additional arguments if they're present. */
        cmd_node = parse_additional_words(cmd_node, tokens);
        if (!cmd_node)
            return (NULL);
    }

    /* 3) Parse any trailing redirections (e.g. cmd ... > file). */
    cmd_root = parse_redirections(tokens, cmd_root);
    if (!cmd_root)
	{
        return (NULL);
	}

    if (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        real_cmd = cmd_root;
        /* Descend through the chain of redirections to find actual command. */
        while (real_cmd && is_redirect_token(real_cmd->type))
            real_cmd = real_cmd->left;
        /* If that node is indeed a TOKEN_WORD, parse additional arguments. */
        if (real_cmd && real_cmd->type == TOKEN_WORD)
        {
            real_cmd = parse_additional_words(real_cmd, tokens);
            if (!real_cmd)
                return free_ast_return_null(cmd_root);
        }
    }

    /* 5) Validate. If it's purely redirections with no actual command, or missing filenames, this will fail. */
    if (!validate_redirection_chain(cmd_root))
        return free_ast_return_null(cmd_root);

    return (cmd_root);
}

t_ast_node *parse_pipeline(t_token **tokens, int i, t_mshell *obj)
{
    t_ast_node *root;
    t_ast_node *pipe_node;

    (void)i;
    (void)obj;

    root = parse_command(tokens);
    if (!root)
        return (NULL);

    /* Keep going if we see a pipe, building a pipe node each time. */
    while (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        /* Check if next token is also pipe or null => syntax error. */
        if (!(*tokens)->next || (*tokens)->next->type == TOKEN_PIPE)
        {
            ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
            return free_ast_return_null(root);
        }
        /* Create pipe node. */
        pipe_node = create_ast_node(TOKEN_PIPE);
        if (!pipe_node)
            return free_ast_return_null(root);

        /* Advance past the pipe token. */
        *tokens = (*tokens)->next;

        /* Left side is the command(s) we already parsed. */
        pipe_node->left = root;
        /* Right side is the next pipeline segment (recursively parse). */
        pipe_node->right = parse_pipeline(tokens, i + 1, obj);
        if (!pipe_node->right)
            return free_ast_return_null(pipe_node);

        /* The new pipe node becomes the top-level root. */
        root = pipe_node;
    }

    return (root);
}
