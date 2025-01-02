/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:08:24 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/02 17:43:43 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*append_substring(char *dest, const char *src, int length)
{
    char    *new_buf;
    int     old_len;
    int     new_len;

    if (!dest || !src || length <= 0)
        return (dest);
    old_len = ft_strlen(dest); // Find the length of the current string in dest
    new_len = old_len + length; // Calculate the new length after appending
    new_buf = ft_calloc(new_len + 1, sizeof(char)); // Allocate space for the new string
    if (!new_buf)
    {
        free(dest);
        return (NULL);
    }
    ft_strlcpy(new_buf, dest, new_len + 1); // Correct size for copying
    ft_strlcat(new_buf, src, new_len + 1);  // Correct size for appending
    free(dest);                             // Free the old buffer
    return (new_buf);
}

static char	*expand_variable(char *buffer, const char *input, int *i, t_mshell *mshell)
{
	char	*expanded;
	char	*var_name;
	int		start;
	int		len;
	
	(*i)++;
	if (input[*i] == '?')
	{
		expanded = ft_itoa(mshell->last_exit_status);
		if (!expanded)
		{
			free(buffer);
			return (NULL);
		}
		buffer = append_substring(buffer, expanded, ft_strlen(expanded));
		free(expanded);
		return (buffer);
	}
	start = *i;
	while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
		(*i)++;
	len = *i - start;
	if (len == 0)
		return append_substring(buffer, "$", 1);
	var_name = ft_calloc(len + 1, sizeof(char));
	if (!var_name)
	{
		free(buffer);
		return (NULL);
	}
	ft_strlcpy(var_name, &input[start], len + 1);
	expanded = getenv(var_name);
	free(var_name);
	if (!expanded)
		expanded = "";
	buffer = append_substring(buffer, expanded, ft_strlen(expanded));
	return (buffer);
}

static t_token *finalize_double_quoted_token(char *buffer)
{
	t_token	*token;
	int		length;
	
	length = ft_strlen(buffer);
	if (length == 0)
		token = new_token(TOKEN_WORD, "", 0);
	else
		token = new_token(TOKEN_WORD, buffer, length);
	free(buffer);
	if (token)
		token->quote_state = QUOTE_DOUBLE;
	return (token);
}

t_token *handle_double_quote_content(const char *input, int *i, int start, t_mshell *mshell)
{
	t_token *token;
	char *buffer;
	int buf_start;

	if (!input || !i)
		return (NULL);
	//printf ("second arg: %zu\n", ft_strlen(&input[*i]));
	buffer = ft_calloc(1, sizeof(char));
	if (!buffer)
		return (NULL);
	*i = start + 1;
	buf_start = *i;
	while (input[*i] && input[*i] != '"')
	{
		if (input[*i] == '$')
		{
			if (*i > buf_start)
			{
				buffer = append_substring(buffer, &input[buf_start], *i - buf_start);
				if (!buffer)
					return (NULL);
			}
			buffer = expand_variable(buffer, input, i, mshell);
			if (!buffer)
				return (NULL);
			buf_start = *i;
		}
		else
			(*i)++;
	}
	if (!input[*i])
	{
		free(buffer);
		return (NULL);
	}
	if (*i > buf_start)
	{
		buffer = append_substring(buffer, &input[buf_start], *i - buf_start);
		if (!buffer)
			return (NULL);
	}
	(*i)++;
	token = finalize_double_quoted_token(buffer);
	token->mshell = mshell;
	return (token);
}

t_token *handle_quote_content(const char *input, int *i, char quote, t_mshell *mshell)
{
    t_token *token;
    int     start;
    int     len;
    char    *content;

    start = ++(*i);
    // *i += 1;
    if (quote == '"')
        return (handle_double_quote_content(input, i, --start, mshell));
    if (find_closing_quote(input, i, quote))
        return (NULL);

    len = *i - start;
    (*i)++;
    if (len == 0)
    {
        token = new_token(TOKEN_WORD, "", 1);
        if (token)
            token->quote_state = QUOTE_SINGLE;
        return (token);
    }
    content = ft_calloc(len + 1, sizeof(char));
    if (!content)
        return (NULL);
    ft_strlcpy(content, &input[start], len + 1);
    token = new_token(TOKEN_WORD, content, len);
    free(content);
    if (!token)
        return (NULL);
    token->quote_state = QUOTE_SINGLE;
	token->mshell = mshell;
    return (token);
}

void add_quoted_token(t_token **head, t_token **current, 
                      const char *input, int *i)
{
    char    quote;
    t_token *token;

    if (!input || !input[*i])
        return ;
    quote = input[*i];
    token = handle_quote_content(input, i, quote, (*current)->mshell);
    if (!token)
    {
        clean_parse_error(head, current);
        ft_printf("Error: Unmatched quote\n");
        return ;
    }
    link_token(head, current, token);
}

int find_closing_quote(const char *input, int *i, char quote_char)
{
    if (!input || !input[*i])
        return (1);
    //(*i)++;
    while (input[*i] && input[*i] != quote_char)
        (*i)++;
    if (!input[*i])
        return (1);
    return (0);
}
