/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:25:26 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/20 10:13:36 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*create_ast_node(t_token_type type)
{
	t_ast_node	*node;
	
	node = (t_ast_node *)ft_calloc(1, sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = NULL;
	node->redirs = NULL;
	node->left = NULL;
	node->right = NULL;
	node->is_quote_heredoc = 0;
	return (node);
}

t_ast_node	*free_ast_return_null(t_ast_node **node)
{
	free_ast(*node);
	*node = NULL;
	return (NULL);
}

void	free_ast(t_ast_node *node)
{
	int	i;
	
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->redirs)
	{
		i = 0;
		while (node->redirs[i])
			free_ast(node->redirs[i++]);
		free(node->redirs);
		node->redirs = NULL;
	}
	if (node->args)
	{
		i = 0;
		while (node->args[i])
			free(node->args[i++]);
		free(node->args);
		node->args = NULL;
	}
	free(node);
}

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
			type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}

static t_ast_node	*create_redir_args(t_ast_node *redir, char *filename)
{
	redir->args = (char **)ft_calloc(2, sizeof(char *));
	if (!redir->args)
	{
		free(filename);
		return (free_ast_return_null(&redir));
	}
	redir->args[0] = filename;
	redir->args[1] = NULL;
	return (redir);
}

static t_ast_node	*handle_redirection_node(t_token **tokens)
{
	t_ast_node	*redir;
	char		*filename;

	if (!tokens || !*tokens)
		return (NULL);
	redir = create_ast_node((*tokens)->type);
	if (!redir)
		return (NULL);
	redir->is_quote_heredoc = (*tokens)->is_quote_heredoc;
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type != TOKEN_WORD)
		return (free_ast_return_null(&redir));
	filename = ft_strdup((*tokens)->content);
	if (!filename)
		return (free_ast_return_null(&redir));
	*tokens = (*tokens)->next;
	return (create_redir_args(redir, filename));
}

static char **append_arg(char **args, char *new_arg)
{
	int		count;
	char	**new_args;
	int		i;

	count = 0;
	if (args)
	{
		while (args[count])
			count++;
	}
	new_args = (char **)ft_calloc(count + 2, sizeof(char *));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[count] = new_arg;
	free(args);
	return (new_args);
}

static t_ast_node	**append_redir(t_ast_node **redirs, t_ast_node *redir)
{
	int			count;
	int			i;
	t_ast_node	**new_redirs;

	count = 0;
	if (redirs)
	{
		while (redirs[count])
			count++;
	}
	new_redirs = (t_ast_node **)ft_calloc(count + 2, sizeof(t_ast_node *));
	if (!new_redirs)
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

static int	handle_empty_command_redirs(t_ast_node *redir)
{
	int	fd;

	fd = -1;
	if (!redir)
		return (fd);
	if (redir->type == TOKEN_REDIRECT_APPEND)
		fd = open(redir->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redir->type == TOKEN_REDIRECT_OUT)
		fd = open(redir->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		perror(redir->args[0]);
	return (fd);
}

static t_ast_node	*handle_word_token(t_ast_node *cmd_node, t_token **tokens)
{
	char	*arg;

	if (ft_strncmp((*tokens)->content, "-n", 2) == 0)
		cmd_node->is_quote_heredoc = (*tokens)->is_quote_heredoc;
	arg = ft_strdup((*tokens)->content);
	if (!arg)
		return (free_ast_return_null(&cmd_node));
	cmd_node->args = append_arg(cmd_node->args, arg);
	if (!cmd_node->args)
	{
		free(arg);
		return (free_ast_return_null(&cmd_node));
	}
	*tokens = (*tokens)->next;
	return (cmd_node);
}

static t_ast_node	*handle_redir_token(t_ast_node *cmd_node, t_token **tokens)
{
	t_ast_node	*redir;

	redir = handle_redirection_node(tokens);
	if (!redir)
		return (free_ast_return_null(&cmd_node));
	cmd_node->redirs = append_redir(cmd_node->redirs, redir);
	if (!cmd_node->redirs)
		return (free_ast_return_null(&cmd_node));
	return (cmd_node);
}

static t_ast_node	*validate_command(t_ast_node *cmd_node)
{
	int	fd;

	if (!cmd_node->args || !cmd_node->args[0])
	{
		if (cmd_node->redirs)
		{
			fd = handle_empty_command_redirs(*cmd_node->redirs);
			if (fd != -1)
				close(fd);
		}
		return (free_ast_return_null(&cmd_node));
	}
	return (cmd_node);
}

t_ast_node	*parse_simple_command(t_token **tokens)
{
	t_ast_node	*cmd_node;

	cmd_node = create_ast_node(TOKEN_WORD);
	if (!cmd_node)
		return (NULL);
	cmd_node->args = NULL;
	cmd_node->redirs = NULL;
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
	return (validate_command(cmd_node));
}

t_ast_node *parse_command(t_token **tokens)
{
	t_ast_node	*cmd_node;

	if (!tokens || !*tokens)
		return (NULL);
	if ((*tokens)->type == TOKEN_PIPE)
	{
		(*tokens)->mshell->exit_code = 2;
		ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
		return (NULL);
	}
	cmd_node = parse_simple_command(tokens);
	return cmd_node;
}

static int	validate_pipe_syntax(t_token *token)
{
	if (!token->next || token->next->type == TOKEN_PIPE)
	{
		ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

static t_ast_node	*create_pipe_structure(t_ast_node *root, t_token **tokens)
{
	t_ast_node	*pipe_node;

	pipe_node = create_ast_node(TOKEN_PIPE);
	if (!pipe_node)
	{
		free_ast(root);
		return (NULL);
	}
	*tokens = (*tokens)->next;
	pipe_node->left = root;
	pipe_node->right = parse_pipeline(tokens);
	if (!pipe_node->right)
	{
		free_ast(pipe_node);
		return (NULL);
	}
	return (pipe_node);
}

t_ast_node	*parse_pipeline(t_token **tokens)
{
	t_ast_node	*root;

	root = parse_command(tokens);
	if (!root)
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE)
	{
		if (!validate_pipe_syntax(*tokens))
		{
			free_ast(root);
			return (NULL);
		}
		root = create_pipe_structure(root, tokens);
		if (!root)
			return (NULL);
	}
	return (root);
}