/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:28:31 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 18:09:55 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_ast_node	*create_command_node(t_token **tokens)
{
	t_ast_node	*cmd_node;
	t_mshell	*mshell;

	mshell = (*tokens)->mshell;
	cmd_node = create_ast_node(TOKEN_WORD);
	if (!cmd_node)
		return (NULL);
	cmd_node->args = NULL;
	cmd_node->redirs = NULL;
	return (cmd_node);
}

t_ast_node	*assemble_simple_command(t_ast_node *cmd_node, t_token **tokens)
{
	t_mshell	*mshell;

	mshell = (*tokens)->mshell;
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
	return (validate_command(cmd_node, mshell));
}

t_ast_node	*parse_simple_command(t_token **tokens)
{
	t_ast_node	*cmd_node;

	cmd_node = create_command_node(tokens);
	if (!cmd_node)
		return (NULL);
	return (assemble_simple_command(cmd_node, tokens));
}

t_ast_node	*parse_command(t_token **tokens)
{
	t_ast_node	*cmd_node;

	if (!tokens || !*tokens)
		return (NULL);
	if ((*tokens)->type == TOKEN_PIPE)
	{
		if (!validate_pipe(*tokens))
			return (NULL);
	}
	cmd_node = parse_simple_command(tokens);
	return (cmd_node);
}

int	handle_empty_command_redirs(t_ast_node *redir, t_mshell *mshell)
{
	int	fd;

	fd = -1;
	if (!redir)
		return (fd);
	// TODO: ADD WHILE LOOP
	if (redir->type == TOKEN_REDIRECT_APPEND)
		fd = open(redir->args[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (redir->type == TOKEN_REDIRECT_OUT)
		fd = open(redir->args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == TOKEN_REDIRECT_IN)
		fd = open(redir->args[0], O_RDONLY);
	else if (redir->type == TOKEN_HEREDOC)
	{
		fd = handle_here_doc(mshell, redir, fd);
		if (fd != -1)
			close(fd);
		fd = -1;
	}
	if (fd == -1)
		perror(redir->args[0]);
	return (fd);
}
