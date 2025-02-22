/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_validating.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:47:07 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/22 09:59:30 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	validate_consecutive_redirects(t_token *token)
{
	t_token	*temp;
	int		count;

	temp = token;
	count = 0;
	while (temp && is_redirect_token(temp->type))
	{
		count++;
		if (count > 1)
			return (print_syntax_error(token,
					get_redir_token_str(token->type)));
		temp = temp->next;
	}
	return (1);
}

int	validate_redirection(t_token *token)
{
	if (!validate_consecutive_redirects(token))
		return (0);
	if (!token->next || token->next->type != TOKEN_WORD)
		return (print_newline_error(token));
	return (1);
}

int	validate_pipe(t_token *token)
{
	t_token	*temp;

	temp = token->mshell->token;
	if (temp && temp->type == TOKEN_PIPE)
		return (print_syntax_error(token, "|"));
	if (!token->next || token->next->type == TOKEN_PIPE)
		return (print_syntax_error(token, "|"));
	return (1);
}

t_ast_node	*validate_command(t_ast_node *cmd_node, t_mshell *mshell)
{
	int	fd;

	if (!cmd_node->args || !cmd_node->args[0])
	{
		if (cmd_node->redirs)
		{
			fd = handle_empty_command_redirs(cmd_node->redirs, mshell);
			// CLOSE IT IN THE FUNCTION ABOVE
			if (fd != -1)
				close(fd);
		}
		return (free_ast_return_null(&cmd_node));
	}
	return (cmd_node);
}
