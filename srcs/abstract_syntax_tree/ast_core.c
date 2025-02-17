/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/17 12:37:36 by msavelie         ###   ########.fr       */
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
	node->is_quote_heredoc = 0;
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
	redir->is_quote_heredoc = (*tokens)->is_quote_heredoc;

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
			if (ft_strncmp((*tokens)->content, "-n", 2) == 0)
				cmd_node->is_quote_heredoc = (*tokens)->is_quote_heredoc;
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
		if (cmd_node->redirs)
		{
			t_ast_node *redir = *cmd_node->redirs;
			int fd;
			fd = -1;
			if (redir)
			{
				if (redir->type == TOKEN_REDIRECT_APPEND)
					fd = open(redir->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
				else if (redir->type == TOKEN_REDIRECT_OUT)
					fd = open(redir->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (fd == -1)
				{
					perror(redir->args[0]);
					
				}
			}
			if (fd != -1)
				close(fd);
		}
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
