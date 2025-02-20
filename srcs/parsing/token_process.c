/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 11:31:38 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/20 11:38:58 by msavelie         ###   ########.fr       */
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

	if (!head || !*head || !current || !*current || !input || !i)
		return (NULL);
	start = *i;
	type = get_operator_type(input, i);
	token = new_token(type, input + start, (*i - start + 1), (*head)->mshell);
	if (!token)
		return (NULL);
	(*i)++;
	token->mshell = (*current)->mshell;
	link_token(head, current, token);
	while (input[*i] && ft_isspace(input[*i]))
		(*i)++;
	
	return (token);
}

char	*handle_backslash(char *str)
{
	char	*new_str;
	size_t	i;
	size_t	j;
	size_t	len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	new_str = ft_calloc(len + 1, sizeof(char));
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (i < len)
    {
        if (str[i] == '\\' && i + 1 < len)
            i++;
        new_str[j] = str[i];
        i++;
        j++;
    }
    new_str[j] = '\0';
    return (new_str);
}

static char	*extract_word(const char *input, int *i)
{
	int		start;
	char	*temp;
	
	if (!input || !i)
		return (NULL);
	start = *i;
	while (input[*i] && !ft_isspace(input[*i]) && 
		!is_operator(input[*i]) && !is_quote(input[*i]))
		(*i)++;
	temp = ft_substr(input, start, (*i) - start);
	return (temp);
}

static char	*process_word(char *temp, t_mshell *mshell)
{
	char	*expanded;
	
	if (!temp || !mshell)
		return (NULL);
	expanded = expand_env_vars(temp, mshell);
	free(temp);
	if (!expanded)
		print_exit("Malloc error\n", NULL, mshell);
	return (expanded);
}

static t_token	*handle_quoted_word(t_token *current, char *expanded)
{
	char	*joined;
	
	if (!current || !expanded)
		return (NULL);
	joined = ft_strjoin(current->content, expanded);
	free(expanded);
	if (!joined)
		return (NULL);
	free(current->content);
	current->content = joined;
	return (current);
}

t_token	*handle_word(t_token **head, t_token **current, const char *input, int *i)
{
	t_token	*token;
	char	*temp;
	char	*expanded;
	
	if (!head || !current || !*current || !input || !i)
		return (NULL);
	temp = extract_word(input, i);
	if (!temp)
		return (NULL);
	expanded = process_word(temp, (*current)->mshell);
	if (!expanded)
		return (NULL);
	if (*current && (*current)->quote_state != QUOTE_NONE)
		return (handle_quoted_word(*current, expanded));
	token = new_token(TOKEN_WORD, expanded, ft_strlen(expanded), (*head)->mshell);
	if (!token)
	{
		free(expanded);
		return (NULL);
	}
	free(expanded);
	token->mshell = (*current)->mshell;
	link_token(head, current, token);
	return (token);
}

t_token *process_token(t_token **head, t_token **current, const char *input, int *i)
{
	if (!head || !current || !*current || !input || !i)
		return (NULL);
	if (ft_isspace(input[*i]))
	{
		(*current)->quote_state = QUOTE_NONE;
		(*head)->quote_state = QUOTE_NONE;
		(*i)++;
		return (*current);
	}
	if (is_quote(input[*i]))
		return (handle_quotes(head, current, input, i));
	if (is_operator(input[*i]))
		return (handle_operator(head, current, input, i));
	return (handle_word(head, current, input, i));
}
