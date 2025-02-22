/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_token_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 14:15:04 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/22 10:37:04 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*extract_quoted_content(const char *input, int start, int end)
{
	char	*content;

	content = ft_substr(input, start, end - start);
	if (!content)
	{
		ft_putstr_fd("syntax error: unclosed double quote\n", STDERR_FILENO);
		return (NULL);
	}
	return (content);
}

char	*process_content(char *content, t_token_type type, t_mshell *mshell)
{
	char	*expanded;

	if (type == TOKEN_HEREDOC)
		return (content);
	expanded = expand_env_vars(content, mshell);
	free(content);
	return (expanded);
}

t_token	*create_quoted_token(char *expanded, const char *input,
	int start, t_mshell *mshell)
{
	t_token	*token;

	token = new_token(TOKEN_WORD, expanded, ft_strlen(expanded), mshell);
	if (!token)
		return (NULL);
	if (ft_strncmp(&input[start], "-n", 2) == 0)
		token->is_quote_heredoc = 1;
	token->quote_state = QUOTE_DOUBLE;
	return (token);
}

t_token	*join_word_token(t_token *prev_token, t_token *token)
{
	char	*joined;

	if (ft_strcmp(prev_token->content, "$") == 0)
	{
		free(prev_token->content);
		prev_token->content = ft_strdup(token->content);
		prev_token->quote_state = token->quote_state;
		free(token->content);
		free(token);
		return (prev_token);
	}
	joined = ft_strjoin(prev_token->content, token->content);
	if (!joined)
		return (NULL);
	free(prev_token->content);
	prev_token->content = joined;
	prev_token->quote_state = token->quote_state;
	free(token->content);
	free(token);
	return (prev_token);
}

t_token	*process_word_token(t_token **head, t_token **current,
	t_token *token, int in_word)
{
	t_token	*prev_token;

	prev_token = *current;
	if (in_word && prev_token && prev_token->type == TOKEN_WORD)
		return (join_word_token(prev_token, token));
	link_token(head, current, token);
	return (token);
}
