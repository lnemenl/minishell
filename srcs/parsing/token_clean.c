/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_clean.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:14:21 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/02 16:24:12 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	clean_token(t_token *token)
{
	if (!token)
		return;
	if (token->start)
		free(token->start);
	free(token);
}

void	clean_token_list(t_token *head)
{
	t_token *current;
	t_token *next;

	if (!head)
		return ;
	current = head;
	while (current)
	{
		next = current->next;
		clean_token(current);
		current = next;
	}
}

void	clean_parse_error(t_token **head, t_token **current)
{
	if (!head || !*head)
		return;
	clean_token_list(*head);
	*head = NULL;
	*current = NULL;
}
