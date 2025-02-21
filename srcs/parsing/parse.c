/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 15:18:23 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	**fetch_paths(char **envp)
{
	char	*path;
	char	**paths;

	if (!envp)
		return (NULL);
	path = get_env_var(envp, "PATH");
	if (path)
	{
		paths = ft_split(path, ':');
		free(path);
		return (paths);
	}
	return (NULL);
}

void	tokenize_and_count_pipes(t_mshell *obj)
{
	t_token	*tokens;
	t_token	*temp;

	if (!obj || !obj->cmd_line)
		return ;
	tokens = tokenize(obj->cmd_line, obj);
	if (!tokens)
		return ;
	obj->token = tokens;
	obj->pipes_count = 0;
	temp = tokens;
	while (temp)
	{
		if (temp->type == TOKEN_PIPE)
			obj->pipes_count++;
		temp = temp->next;
	}
}

void	init_tokenize(t_token **head, t_token **current)
{
	*head = NULL;
	*current = NULL;
}

t_token	*tokenize(const char *input, t_mshell *mshell)
{
	t_token	*head;
	t_token	*current;
	t_token	*tokens;
	char	*trimmed_input;

	if (!input)
		return (NULL);
	trimmed_input = ft_strtrim(input, " \t\n\r\f\v");
	if (!trimmed_input || !trimmed_input[0])
	{
		free(trimmed_input);
		return (NULL);
	}
	init_tokenize(&head, &current);
	tokens = process_trimmed_input(&head, &current, trimmed_input, mshell);
	free(trimmed_input);
	return (tokens);
}
