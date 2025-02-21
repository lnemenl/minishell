/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:37:32 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 11:33:08 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	check_redirections(t_mshell *obj)
{
	t_token	*temp;

	temp = obj->token;
	obj->redir_check = 0;
	while (temp)
	{
		if (temp->type == TOKEN_HEREDOC
			|| temp->type == TOKEN_REDIRECT_IN
			|| temp->type == TOKEN_REDIRECT_OUT
			|| temp->type == TOKEN_REDIRECT_APPEND)
		{
			obj->redir_check = 1;
			return ;
		}
		temp = temp->next;
	}
}

void	apply_redirections(t_mshell *obj, t_ast_node *cmd)
{
	int	i;

	if (!cmd || !cmd->redirs)
		return ;
	i = 0;
	while (cmd->redirs[i])
	{
		if (cmd->redirs[i]->type == TOKEN_HEREDOC
			|| cmd->redirs[i]->type == TOKEN_REDIRECT_IN)
			redirection_input(obj, cmd->redirs[i]);
		else if (cmd->redirs[i]->type == TOKEN_REDIRECT_OUT
			|| cmd->redirs[i]->type == TOKEN_REDIRECT_APPEND)
			redirection_output(obj, cmd->redirs[i]);
		i++;
	}
}
