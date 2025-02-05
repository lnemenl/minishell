/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 11:31:38 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/05 20:30:09 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static t_token_type get_operator_type(const char *input, int *i)
{
	if (input[*i] == '|')
		return (TOKEN_PIPE);
	if (input[*i] == '<')
	{
		if ( input[*i + 1] && input[*i + 1] == '<')
		{
			(*i)++;
			return (TOKEN_HEREDOC);
		}
		return (TOKEN_REDIRECT_IN);
	}
	if (input[*i] == '>')
	{
		if (input[*i + 1] && input[*i + 1] == '>')
		{
			(*i)++;
			return (TOKEN_REDIRECT_APPEND);
		}
		return (TOKEN_REDIRECT_OUT);
	}
	return (TOKEN_WORD);
}

t_token *handle_operator(t_token **head, t_token **current, const char *input, int *i)
{
	t_token			*token;
	t_token_type	type;
	int				start;

	if (!head || !*head || !current || !*current)
		return (NULL);
	start = *i;
	type = get_operator_type(input, i);
	token = new_token(type, input + start, (*i - start + 1), (*head)->mshell);
	if (!token)
		return (NULL);
	(*i)++;
	token->mshell = (*current)->mshell;
	link_token(head, current, token);
	
	// Skip spaces but continue processing if there's more content
	while (input[*i] && ft_isspace(input[*i]))
		(*i)++;
	
	// Handle immediate next token if it exists
	// if (input[*i] && !is_operator(input[*i]))
	// {
	// 	t_token *next_token;
	// 	if (is_quote(input[*i]))
	// 		next_token = handle_quotes(head, current, input, i);
	// 	else
	// 		next_token = handle_word(head, current, input, i);
	// 	if (!next_token)
	// 		return (NULL);
	// }
	
	return (token);
}

t_token *handle_word(t_token **head, t_token **current, const char *input, int *i)
{
	t_token	*token;
	char	*expanded;
	char	*temp;
	int		start;
	char	*joined;
	
	if (!head || !current || !*current)
		return (NULL);
	start = *i;
	while (input[*i] && !ft_isspace(input[*i]) && 
		   !is_operator(input[*i]) && !is_quote(input[*i]))
		(*i)++;
	temp = ft_substr(input, start, (*i) - start);
	if (!temp)
		return (NULL);
	
	// Check if previous token exists and has quote state
	if (*current && (*current)->quote_state != QUOTE_NONE)
	{
		joined = ft_strjoin((*current)->content, temp);
		free(temp);
		if (!joined)
			return (NULL);
		free((*current)->content);
		(*current)->content = joined;
		return (*current);
	}
	
	expanded = expand_env_vars(temp, (*current)->mshell);
	free(temp);
	if (!expanded)
		return (NULL);
	token = new_token(TOKEN_WORD, expanded, ft_strlen(expanded), (*head)->mshell);
	free(expanded);
	if (!token)
		return (NULL);
	token->mshell = (*current)->mshell;
	link_token(head, current, token);
	return (token);
}

t_token *process_token(t_token **head, t_token **current, const char *input, int *i)
{

	if (ft_isspace(input[*i]))
	{
		(*i)++;
		return (*current);
	}
	if (is_quote(input[*i]))
		return (handle_quotes(head, current, input, i));
	if (is_operator(input[*i]))
		return (handle_operator(head, current, input, i));
	return (handle_word(head, current, input, i));
}
