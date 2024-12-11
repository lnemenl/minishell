/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/11 12:52:06 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	**fetch_paths(char **envp)
{
	int		i;
	char	*check_path;
	char	**paths;

	i = 0;
	paths = NULL;
	check_path = NULL;
	while (envp[i])
	{
		if (ft_strnstr(envp[i], "PATH=", 5))
		{
			check_path = ft_strdup(envp[i]);
			if (!check_path)
				error_ret(6, NULL);
		}
		i++;
	}
	if (check_path)
	{
		paths = ft_split(check_path + 5, ':');
		free(check_path);
		if (!paths)
			error_ret(6, NULL);
	}
	return (paths);
}

void	parse(t_mshell *obj)
{
    t_token *tokens;
    t_token *tmp;

    // Call the tokenizer to process the input line
    tokens = tokenize(obj->cmd_line);
    if (!tokens)
    {
        ft_printf("Error: Failed to tokenize input.\n");
        return;
    }
    // Print tokens for debugging
    tmp = tokens;
    while (tmp)
    {
        if (tmp->type == TOKEN_WORD)
            ft_printf("TOKEN_WORD: %s\n", tmp->value);
        else if (tmp->type == TOKEN_PIPE)
            ft_printf("TOKEN_PIPE: %s\n", tmp->value);
        else if (tmp->type == TOKEN_REDIRECT_OUT)
            ft_printf("TOKEN_REDIRECT_OUT: %s\n", tmp->value);
        else if (tmp->type == TOKEN_REDIRECT_IN)
            ft_printf("TOKEN_REDIRECT_IN: %s\n", tmp->value);
        else if (tmp->type == TOKEN_REDIRECT_APPEND)
            ft_printf("TOKEN_REDIRECT_APPEND: %s\n", tmp->value);
        else if (tmp->type == TOKEN_HEREDOC)
            ft_printf("TOKEN_HEREDOC: %s\n", tmp->value);

        tmp = tmp->next;
    }
    // Free tokens after printing
    while (tokens)
    {
        tmp = tokens->next;
        free(tokens->value); // Free the token value
        free(tokens);        // Free the token itself
        tokens = tmp;
    }
}

int	ft_isspace(int c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

int	is_operator(char c)
{
    return (c == '|' || c == '>' || c == '<');
}

int	is_word_char(char c)
{
    return (ft_isalnum(c) || c == '-' || c == '_');
}

char *ft_strndup(const char *s, size_t n)
{
    char *dup;

    if (!s)
        return (NULL);
    dup = ft_calloc(sizeof(char), n + 1); // Allocate memory (n + 1 for null terminator)
    if (!dup)
        return (NULL);
    ft_memcpy(dup, s, n); // Copy 'n' characters from 's' to 'dup'
    return (dup);
}

t_token	*tokenize(const char *input)
{
	t_token	*head;
	t_token	*current;
	char	*trimmed_input;
	int		i;

	head = NULL;
	current = NULL;
	trimmed_input = ft_strtrim(input, " \t\n\r\f\v");
	if (!trimmed_input)
		return (NULL);
	i = 0;
	while (trimmed_input[i])
	{
		if (ft_isspace(trimmed_input[i]))
			i++;
		else if (is_operator(trimmed_input[i]))
			add_operator_token(&head, &current, trimmed_input, &i);
		else if (trimmed_input[i] == '"' || trimmed_input[i] == '\'')
			add_quoted_token(&head, &current, trimmed_input, &i);
		else
			add_word_token(&head, &current, trimmed_input, &i);
	}
	free(trimmed_input);
	return (head);
}

t_token	*new_token(t_token_type type, char *value)
{
	t_token	*token;
	
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}

void	add_operator_token(t_token **head, t_token **current, const char *input, int *i)
{
	t_token	*token;
	char	*value;

	if (input[*i] == '>' && input[*i + 1] == '>')
	{
		value = ft_strdup(">>");
		token = new_token(TOKEN_REDIRECT_APPEND, value);
		*i += 2;
	}
	else if (input[*i] == '<' && input[*i + 1] == '<')
	{
		value = ft_strdup("<<");
		token = new_token(TOKEN_HEREDOC, value);
		*i += 2;
	}
	else if (input[*i] == '>')
	{
		value = ft_strdup(">");
		token = new_token(TOKEN_REDIRECT_OUT, value);
		(*i)++;
	}
	else if (input[*i] == '<')
	{
		value = ft_strdup("<");
		token = new_token(TOKEN_REDIRECT_IN, value);
		(*i)++;
	}
	else if (input[*i] == '|')
	{
		value = ft_strdup(">");
		token = new_token(TOKEN_PIPE, value);
		(*i)++;
	}
	else
		return ;
	if (!*head)
		*head = token;
	else
		(*current)->next = token;
	*current = token;
}

void	add_quoted_token(t_token **head, t_token **current, const char *input, int *i)
{
	char	quote_char;
	int		start;
	int		len;
	char	*value;
	t_token	*token;

	quote_char = input[*i];
	start = ++(*i);

	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (input[*i] != quote_char)
	{
		//handle error: unmatched quote
		ft_printf("Error: Unmatched quote\n");
		return ;
	}
	len = *i - start;
	value = ft_strndup(&input[start], len);
	(*i)++;
	token = new_token(TOKEN_WORD, value);
	if (!*head)
		*head = token;
	else
		(*current)->next = token;
	*current = token;
}

void	add_word_token(t_token **head, t_token **current, const char *input, int *i)
{
	int		start;
	int		len;
	char	*value;
	t_token	*token;
	
	start = *i;
	// Collecting characters until a space or operator
	while (input[*i] && !ft_isspace(input[*i]) && !is_operator(input[*i]))
		(*i)++;
	len = *i - start; // Calculating length of the word
	value = ft_strndup(&input[start], len);
	token = new_token(TOKEN_WORD, value);
	if (!*head)
		*head = token;
	else//Append to the current list
		(*current)->next = token;
	*current = token;// Update current pointer
}