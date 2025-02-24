/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_construction.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 14:09:37 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/24 12:33:06 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	construct_parse_tree(t_mshell *obj)
{
	t_token	*tokens;

	tokens = obj->token;
	obj->ast = parse_pipeline(&tokens);
	if (!obj->ast)
	{
		clean_tokens(obj->token);
		obj->token = NULL;
		if (obj->exit_code != 2)
			obj->exit_code = 1;
		return ;
	}
}

void	parse(t_mshell *obj)
{
	tokenize_and_count_pipes(obj);
	construct_parse_tree(obj);
}

t_token	*init_and_process(t_token **head, t_token **current,
	char *trimmed_input, t_mshell *mshell)
{
	t_token	*dummy;
	int		i;

	dummy = new_token(TOKEN_WORD, "", 0, mshell);
	if (!dummy)
		return (NULL);
	*head = dummy;
	*current = dummy;
	i = 0;
	while (trimmed_input[i])
	{
		if (!process_token(head, current, trimmed_input, &i))
		{
			clean_tokens(dummy);
			return (NULL);
		}
	}
	return (dummy);
}

t_token	*process_trimmed_input(t_token **head, t_token **current,
	char *trimmed_input, t_mshell *mshell)
{
	t_token	*dummy;

	dummy = init_and_process(head, current, trimmed_input, mshell);
	if (!dummy)
		return (NULL);
	if (!dummy->next)
	{
		clean_tokens(dummy);
		*head = NULL;
		return (NULL);
	}
	*head = dummy->next;
	free(dummy->content);
	free(dummy);
	return (*head);
}
