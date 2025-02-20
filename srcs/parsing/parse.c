/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/20 09:38:27 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	**fetch_paths(char **envp)
{
	char    *path;
	char    **paths;

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

void parse(t_mshell *obj)
{
	t_token *tokens;
	t_token *temp;

	if (!obj || !obj->cmd_line)
		return;
	tokens = tokenize(obj->cmd_line, obj);
	if (!tokens)
		return;
	obj->token = tokens;
	obj->pipes_count = 0;
	temp = tokens;
	while (temp)
	{
		if (temp->type == TOKEN_PIPE)
			obj->pipes_count++;
		temp = temp->next;
	}
	obj->ast = parse_pipeline(&tokens);
	if (!obj->ast)
	{
		clean_tokens(tokens);
		obj->token = NULL;
		obj->exit_code = 1;
		return;
	}
}

void    init_tokenize(t_token **head, t_token **current)
{
	*head = NULL;
	*current = NULL;
}

static t_token	*init_and_process(t_token **head, t_token **current,
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

t_token *tokenize(const char *input, t_mshell *mshell)
{
	t_token *head;
	t_token *current;
	t_token *tokens;
	char    *trimmed_input;

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
