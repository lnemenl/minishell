/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_cleaning.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 13:54:21 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 13:56:15 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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
