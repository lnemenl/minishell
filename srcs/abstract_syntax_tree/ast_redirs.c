/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redirs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:50:46 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 13:57:24 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	is_redirect_token(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND || type == TOKEN_HEREDOC);
}

t_ast_node	*create_redir_args(t_ast_node *redir, char *filename)
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

t_ast_node	*handle_redirection_node(t_token **tokens)
{
	t_ast_node	*redir;
	char		*filename;

	if (!tokens || !*tokens)
		return (NULL);
	if (!validate_redirection(*tokens))
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

t_ast_node	**append_redir(t_ast_node **redirs, t_ast_node *redir)
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

t_ast_node	*handle_redir_token(t_ast_node *cmd_node, t_token **tokens)
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
