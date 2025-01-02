/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_create.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 14:56:07 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/02 17:42:45 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

t_token	*new_token(t_token_type type, const char *start, int length)
{
	t_token    *token;
	char       *content;

	if (!start || length <= 0)
		return (NULL);
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	content = allocate_token_content(start, length);
	if (!content)
	{
		free(token);
		return (NULL);
	}
	token->type = type;
	token->start = content;
	token->length = length;
	token->next = NULL;
	token->quote_state = QUOTE_NONE;
	return (token);
}

char	*allocate_token_content(const char *start, int length)
{
	char    *content;

	if (!start || length <= 0)
		return (NULL);
	content = ft_calloc(length + 1, sizeof(char));
	if (!content)
		return (NULL);
	ft_strlcpy(content, start, length + 1);
	return (content);
}

t_token	*create_token_from_str(const char *str, t_token_type type, int len)
{
	if (!str || len <= 0)
		return (NULL);
	return (new_token(type, str, len));
}

void	link_token(t_token **head, t_token **current, t_token *new)
{
	if (!*head)
		*head = new;
	else
		(*current)->next = new;
	*current = new;
}

t_token	*process_token(t_token **head, t_token **current, const char *input, int *i)
{
    if (ft_isspace(input[*i]))
    {
        (*i)++;
        return (*current);
    }
    if (*head && *current && (*current)->type == TOKEN_WORD && 
        !ft_isspace(input[*i - 1]))
    {
        if (is_quote(input[*i]) || (!is_operator(input[*i]) && !ft_isspace(input[*i])))
        {
            if (!extend_current_token(current, input, i))
                return (NULL);
            return (*current);
        }
    }
    if (is_operator(input[*i]))
        add_operator_token(head, current, input, i);
    else if (is_quote(input[*i]))
        add_quoted_token(head, current, input, i);
    else
        add_word_token(head, current, input, i);

    return (*current);
}

static int	update_token_content(t_token **current, const char *input, int add_len)
{
	char	*new_content;
	char	*temp;

	new_content = ft_calloc((*current)->length + add_len + 1, sizeof(char));
	if (!new_content)
		return (0);
	ft_strlcpy(new_content, (*current)->start, (*current)->length + 1);
	ft_strlcat(new_content, input, (*current)->length + add_len + 1);
	temp = (*current)->start;
	(*current)->start = new_content;
	(*current)->length += add_len;
    if (temp)
	    free(temp);
	return (1);
}

int	extend_current_token(t_token **current, const char *input, int *i)
{
    int		add_len;
    int		start;
    char    quote;

    if (is_quote(input[*i]))
    {
        quote = input[*i];
        start = ++(*i);
        while (input[*i] && input[*i] != quote)
            (*i)++;
        add_len = *i - start;
        if (!update_token_content(current, input + start, add_len))
            return (0);
        (*i)++;
        return (1);
    }
    else
    {
        start = *i;
        add_len = 0;
        while (input[*i] && !ft_isspace(input[*i]) && 
               !is_operator(input[*i]) && !is_quote(input[*i]))
        {
            add_len++;
            (*i)++;
        }
        return (update_token_content(current, input + start, add_len));
    }
}
