/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_syntax_errors.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:45:04 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/22 17:24:16 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	print_syntax_error(t_token *token, char *message)
{
	token->mshell->exit_code = 2;
	ft_putstr_fd("syntax error near unexpected token '", STDERR_FILENO);
	ft_putstr_fd(message, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
	return (0);
}

int	print_newline_error(t_token *token)
{
	token->mshell->exit_code = 2;
	ft_putstr_fd("syntax error near unexpected token 'newline'\n",
		STDERR_FILENO);
	return (0);
}

char	*get_redir_token_str(t_token_type type)
{
	if (type == TOKEN_REDIRECT_IN)
		return ("<");
	if (type == TOKEN_REDIRECT_OUT)
		return (">");
	if (type == TOKEN_HEREDOC)
		return ("<<");
	if (type == TOKEN_REDIRECT_APPEND)
		return (">>");
	return (NULL);
}
