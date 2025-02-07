/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/06 19:55:00 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int is_redirect_token(t_token_type type)
{
    return (type == TOKEN_REDIRECT_IN ||
            type == TOKEN_REDIRECT_OUT ||
            type == TOKEN_REDIRECT_APPEND ||
            type == TOKEN_HEREDOC);
}

t_ast_node *free_ast_return_null(t_ast_node *node)
{
    free_ast(node);
    return NULL;
}

t_ast_node *create_ast_node(t_token_type type)
{
    t_ast_node *node;

    node = (t_ast_node *)ft_calloc(1, sizeof(t_ast_node));
    if (!node)
        return NULL;
    node->type = type;
    return node;
}

static int validate_redirection_chain(t_ast_node *node)
{
    while (node && is_redirect_token(node->type))
    {
        if (!node->args || !node->args[0])
            return 0;
        node = node->left;
    }
    return 1;
}

static char *concatenate_filename(t_token **tokens)
{
    char *filename;
    char *temp;
    int first = 1;

    filename = ft_strdup("");
    if (!filename)
        return NULL;
    while (*tokens && (*tokens)->type == TOKEN_WORD)
    {
        /* If not the first token, insert a space */
        if (!first)
        {
            temp = ft_strjoin(filename, " ");
            free(filename);
            if (!temp)
                return NULL;
            filename = temp;
        }
        else
            first = 0;
        temp = ft_strjoin(filename, (*tokens)->content);
        free(filename);
        if (!temp)
            return NULL;
        filename = temp;
        *tokens = (*tokens)->next;
    }
    return filename;
}

static t_ast_node *handle_redirection_node(t_token **tokens)
{
    t_ast_node *redir;
    char *filename;

    if (!tokens || !*tokens)
        return NULL;
    redir = create_ast_node((*tokens)->type);
    if (!redir)
        return NULL;
    /* Advance past the redirection token */
    *tokens = (*tokens)->next;
    filename = concatenate_filename(tokens);
    if (!filename)
        return free_ast_return_null(redir);
    redir->args = (char **)ft_calloc(2, sizeof(char *));
    if (!redir->args)
    {
        free(filename);
        return free_ast_return_null(redir);
    }
    redir->args[0] = filename;
    redir->args[1] = NULL;
    return redir;
}

t_ast_node *handle_command_redirections(t_token **tokens, t_ast_node *cmd_node)
{
    t_ast_node *redir;
    t_ast_node *current;

    while (*tokens && is_redirect_token((*tokens)->type))
    {
        redir = handle_redirection_node(tokens);
        if (!redir)
        {
            free_ast(cmd_node);
            return NULL;
        }
        if (cmd_node->left == NULL)
            cmd_node->left = redir;
        else
        {
            current = cmd_node->left;
            while (current->left)
                current = current->left;
            current->left = redir;
        }
    }
    return cmd_node;
}

static t_ast_node *handle_initial_redirection(t_token **tokens)
{
    t_ast_node *redir;
    t_ast_node *cmd;

    redir = handle_redirection_node(tokens);
    if (!redir)
        return NULL;
    cmd = parse_command(tokens);
    if (!cmd)
        return free_ast_return_null(redir);
    redir->left = cmd;
    return redir;
}

t_ast_node *parse_command(t_token **tokens)
{
    t_ast_node *cmd_node;

    if (!tokens || !*tokens)
        return NULL;
    if ((*tokens)->type == TOKEN_PIPE)
    {
        ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
        return NULL;
    }
    if (is_redirect_token((*tokens)->type))
    {
        cmd_node = handle_initial_redirection(tokens);
        if (!cmd_node || !validate_redirection_chain(cmd_node))
        {
            free_ast(cmd_node);
            return NULL;
        }
        return cmd_node;
    }
    cmd_node = build_command_node(tokens);
    if (!cmd_node || !(cmd_node->args))
        return NULL;
    cmd_node = handle_command_redirections(tokens, cmd_node);
    if (!cmd_node || !validate_redirection_chain(cmd_node))
    {
        free_ast(cmd_node);
        return NULL;
    }
    return cmd_node;
}
static t_ast_node *parse_pipeline_rec(t_token **tokens, t_mshell *obj, t_ast_node *root)
{
    t_ast_node *pipe_node;

    if (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        /* Check for syntax errors: trailing or consecutive pipes */
        if (!((*tokens)->next) || ((*tokens)->next)->type == TOKEN_PIPE)
        {
            ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
            return free_ast_return_null(root);
        }
        pipe_node = create_ast_node(TOKEN_PIPE);
        if (!pipe_node)
            return free_ast_return_null(root);
        /* Consume the pipe token */
        *tokens = (*tokens)->next;
        pipe_node->left = root;
        pipe_node->right = parse_pipeline(tokens, 0, obj);
        if (!pipe_node->right)
            return free_ast_return_null(pipe_node);
        return parse_pipeline_rec(tokens, obj, pipe_node);
    }
    return root;
}

t_ast_node *parse_pipeline(t_token **tokens, int i, t_mshell *obj)
{
    t_ast_node *root;

    (void)i;
    (void)obj;
    root = parse_command(tokens);
    if (!root)
        return NULL;
    return parse_pipeline_rec(tokens, obj, root);
}

void free_ast(t_ast_node *node)
{
    int i;

    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->right);
    if (node->args)
    {
        i = 0;
        while (node->args[i])
        {
            free(node->args[i]);
            i++;
        }
        free(node->args);
    }
    free(node);
}

