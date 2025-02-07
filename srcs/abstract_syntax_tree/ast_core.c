/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/07 15:10:34 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node *create_ast_node(t_token_type type)
{
    t_ast_node *node = (t_ast_node *)ft_calloc(1, sizeof(t_ast_node));
    if (!node)
        return (NULL);
    node->type = type;
    node->args = NULL;
    node->redirs = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}

t_ast_node	*free_ast_return_null(t_ast_node *node)
{
	free_ast(node);
	return (NULL);
}

void free_ast(t_ast_node *node)
{
    int i;
    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->right);
    if (node->redirs)
    {
        i = 0;
        while (node->redirs[i])
        {
            free_ast(node->redirs[i]);
            i++;
        }
        free(node->redirs);
    }
    if (node->args)
    {
        i = 0;
        while (node->args[i])
            free(node->args[i++]);
        free(node->args);
    }
    free(node);
}


int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
			type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}

/*
• This function skips over the redirection operator token.
• It then expects a following TOKEN_WORD (representing the filename).
• It creates a new redirection node whose args array holds this filename.
• Finally, that redirection node is appended to the current command node’s redirs array
    using the append_redir helper.
*/
static t_ast_node *handle_redirection_node(t_token **tokens)
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
    redir->args[1] = NULL;
	return redir;
}


/*
    This function appends a new string (new_arg) to the array of arguments.
    • First, it counts how many items are already in the args array using a while loop.
    • Then it allocates a new array with one extra slot (plus one for the NULL terminator).
    • It copies the old pointers into the new array using a while loop, appends the new_arg,
      and frees the old array (only the array, not the strings themselves).
*/
static char **append_arg(char **args, char *new_arg)
{
    int count;
    char **new_args;
    int i;

    count = 0;
    if (args)
    {
        while (args[count] != NULL)
            count++;
    }
    new_args = (char **)ft_calloc(count + 2, sizeof(char *));
    if (new_args == NULL)
        return (NULL);
    i = 0;
    while (i < count)
    {
        new_args[i] = args[i];
        i++;
    }
    new_args[count] = new_arg;
    free(args);  /* free old pointer array (not the strings) */
    return (new_args);
}


/*
    This one is almost the same in functionality as append_arg but for an array of AST node pointers representing redirection nodes.
    • It uses a while loop to count how many redirection nodes (redirs) are already there.
    • Then it allocates a new array (with one extra slot for the new node, plus one for the terminating NULL).
    • It copies the old node pointers and appends the new redirection node, then frees the previously allocated array
*/
static t_ast_node **append_redir(t_ast_node **redirs, t_ast_node *redir)
{
    int count;
    t_ast_node **new_redirs;
    int i;

    count = 0;
    if (redirs)
    {
        while (redirs[count] != NULL)
            count++;
    }
    new_redirs = (t_ast_node **)ft_calloc(count + 2, sizeof(t_ast_node *));
    if (new_redirs == NULL)
        return (NULL);
    i = 0;
    while (i < count)
    {
        new_redirs[i] = redirs[i];
        i++;
    }
    new_redirs[count] = redir;
    free(redirs);
    return (new_redirs);
}

/*
    The parse_simple_command function loops over the tokens until it reaches a PIPE token
    (or the end of tokens for the current command).
    • If the token is a word, it gets appended to the command’s args array (using append_arg).
    • If it’s a redirection token, it calls handle_redirection_node,
    and the result is added to the redirs array.
    • At the end of the loop, it checks to make sure
        that there is at least one argument (the command name) before returning the command node.
*/
t_ast_node *parse_simple_command(t_token **tokens)
{
    t_ast_node *cmd_node = create_ast_node(TOKEN_WORD);
    if (!cmd_node)
        return (NULL);
    cmd_node->args = NULL;
    cmd_node->redirs = NULL;

    while (*tokens && (*tokens)->type != TOKEN_PIPE)
    {
        if (is_redirect_token((*tokens)->type))
        {
            t_ast_node *redir = handle_redirection_node(tokens);
            if (!redir)
            {
                free_ast(cmd_node);
                return (NULL);
            }
            cmd_node->redirs = append_redir(cmd_node->redirs, redir);
            if (!cmd_node->redirs)
            {
                free_ast(cmd_node);
                return (NULL);
            }
        }
        else if ((*tokens)->type == TOKEN_WORD)
        {
            char *arg = ft_strdup((*tokens)->content);
            if (!arg)
            {
                free_ast(cmd_node);
                return (NULL);
            }
            cmd_node->args = append_arg(cmd_node->args, arg);
            if (!cmd_node->args)
            {
                free(arg);
                free_ast(cmd_node);
                return (NULL);
            }
            *tokens = (*tokens)->next;
        }
        else
        {
            // If unexpected token type encountered, break out.
            break;
        }
    }

    // At least one argument (the command name) must be present.
    if (!cmd_node->args || !cmd_node->args[0])
    {
        free_ast(cmd_node);
        return (NULL);
    }
    return cmd_node;
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
    cmd_node = parse_simple_command(tokens);
    if (!cmd_node)
        return (NULL);
    return cmd_node;
}


/*
    The parse_pipeline function builds the tree structure for pipelines.
    • It first calls parse_command (which internally calls parse_simple_command)
    to build the AST for the first command.
    • Then, while there are pipe tokens, it:
        – Creates a new PIPE node.
        – Assigns the left pointer of this PIPE node to the command tree built so far.
        – Advances token consumption and recursively builds the tree for the command(s)
        after the pipe, which is assigned to the right pointer.
    • This recursive definition builds a binary tree
    where every PIPE node splits the surrounding commands into left
    (before "|") and right (after "|") subtrees.
*/
t_ast_node *parse_pipeline(t_token **tokens)
{
    t_ast_node *root;
    t_ast_node *pipe_node;

    root = parse_command(tokens);
    if (!root)
        return (NULL);
    while (*tokens && (*tokens)->type == TOKEN_PIPE)
    {
        if (!(*tokens)->next || (*tokens)->next->type == TOKEN_PIPE)
        {
            ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
            free_ast(root);
            return (NULL);
        }
        pipe_node = create_ast_node(TOKEN_PIPE);
        if (!pipe_node)
        {
            free_ast(root);
            return (NULL);
        }
        *tokens = (*tokens)->next; // consume pipe token
        pipe_node->left = root;
        pipe_node->right = parse_pipeline(tokens);
        if (!pipe_node->right)
        {
            free_ast(pipe_node);
            return (NULL);
        }
        root = pipe_node;
    }
    return root;
}


/*
• type: Indicates what kind of node it is.
For example, a command node (often marked as TOKEN_WORD),
a pipe operator (TOKEN_PIPE), or a redirection operator (like TOKEN_REDIRECT_IN, TOKEN_REDIRECT_OUT, etc.)

• args: For command nodes, this is a NULL-terminated array of strings.
The first element is usually the command name and the rest are its arguments.
For redirection nodes, this array holds its associated filename (typically stored in args[0]).

• redirs: This is also a NULL-terminated array.
In command nodes, any redirection nodes parsed (using redirection tokens like <, >, etc.) are stored here.
This separation makes execution easier because you later iterate over this array to set up the file redirections.

• left and right: These are used to build a binary tree structure for operations that connect commands.
In the context of pipelines, a pipe operator node will have:
    – left child: The command (or pipeline) on the left of the pipe.
    – right child: The command (or pipeline) on the right of the pipe.
In other words, when you have a command like "ls | grep foo",
a PIPE node is created where the left pointer refers to the AST node for "ls" and the right pointer is the AST node for "grep foo".
*/

static void normalize_redirs(t_ast_node *cmd_node)
{
    int i;
    int count;
    int last_input = -1;
    int last_output = -1;
    t_ast_node **new_redirs;

    if (!cmd_node || !cmd_node->redirs)
        return;
    count = 0;
    while (cmd_node->redirs[count])
    {
        if (cmd_node->redirs[count]->type == TOKEN_REDIRECT_IN ||
            cmd_node->redirs[count]->type == TOKEN_HEREDOC)
            last_input = count;
        if (cmd_node->redirs[count]->type == TOKEN_REDIRECT_OUT ||
            cmd_node->redirs[count]->type == TOKEN_REDIRECT_APPEND)
            last_output = count;
        count++;
    }
    new_redirs = (t_ast_node **)ft_calloc(3, sizeof(t_ast_node *));
    if (!new_redirs)
        return; /* Allocation issue: leave unchanged */
    /* Keep only the winning redirections (if they exist) */
    if (last_input != -1)
        new_redirs[0] = cmd_node->redirs[last_input];
    if (last_output != -1)
        new_redirs[1] = cmd_node->redirs[last_output];
    new_redirs[(last_input != -1) + (last_output != -1)] = NULL;
    /* Free all redir nodes that were not kept */
    i = 0;
    while (i < count)
    {
        if ((i != last_input) && (i != last_output))
            free_ast(cmd_node->redirs[i]);
        i++;
    }
    free(cmd_node->redirs);
    cmd_node->redirs = new_redirs;
}

void normalize_ast(t_ast_node *node)
{
    if (!node)
        return;
    if (node->type == TOKEN_WORD)
    {
        if (node->redirs)
            normalize_redirs(node);
    }
    normalize_ast(node->left);
    normalize_ast(node->right);
}

