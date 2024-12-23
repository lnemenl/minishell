/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/23 12:18:08 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	**fetch_paths(char **envp)
{
	int		i;
	char	**paths;

	i = 0;
	paths = NULL;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			paths = ft_split(envp[i] + 5, ':'); // Split the value after "PATH="
			if (!paths)
				error_ret(6, NULL); // Handle error on split failure
			break; // Exit loop once PATH is found
		}
		i++;
	}
	return (paths); // Returns NULL if PATH is not found
}

void	parse(t_mshell *obj)
{
    t_token *tokens;
    t_token *tmp;

    if (!obj->cmd_line)
    {
        ft_printf("Error: No input provided.\n");
        return;
    }
    tokens = tokenize(obj->cmd_line);
    if (!tokens)
    {
        ft_printf("Error: Failed to tokenize input.\n");
        return;
    }
    tmp = tokens;
    while (tmp)
    {
        if (tmp->type == TOKEN_WORD)
            ft_printf("TOKEN_WORD: %s\n", tmp->start);
        else if (tmp->type == TOKEN_PIPE)
            ft_printf("TOKEN_PIPE: |\n");
        else if (tmp->type == TOKEN_REDIRECT_OUT)
            ft_printf("TOKEN_REDIRECT_OUT: >\n");
        else if (tmp->type == TOKEN_REDIRECT_IN)
            ft_printf("TOKEN_REDIRECT_IN: <\n");
        else if (tmp->type == TOKEN_REDIRECT_APPEND)
            ft_printf("TOKEN_REDIRECT_APPEND: >>\n");
        else if (tmp->type == TOKEN_HEREDOC)
            ft_printf("TOKEN_HEREDOC: <<\n");

        tmp = tmp->next;
    }
    while (tokens)
    {
		if (tokens->start)
			free(tokens->start);
        tmp = tokens->next;
        free(tokens);
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

t_token	*new_token(t_token_type type, const char *start, int length)
{
	t_token	*token;
	
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->start = ft_calloc(length + 1, sizeof(char));
	if (!token->start)
	{
		free(token);
		return (NULL);
	}
	ft_strlcpy(token->start, start, length + 1);
	token->length = length;
	token->next = NULL;
	return (token);
}

void	add_operator_token(t_token **head, t_token **current, const char *input, int *i)
{
	t_token	*token;

	if (!input || !input[*i])
		return ;
	token = NULL;
	if (input[*i + 1] && input[*i] == '>' && input[*i + 1] == '>')
	{
		token = new_token(TOKEN_REDIRECT_APPEND, &input[*i], 2);
		(*i) += 2;
	}
	else if (input[*i + 1] && input[*i] == '<' && input[*i + 1] == '<')
	{
		token = new_token(TOKEN_HEREDOC, &input[*i], 2);
		(*i) += 2;
	}
	else if (input[*i] == '>')
	{
		token = new_token(TOKEN_REDIRECT_OUT, &input[*i], 1);
		(*i)++;
	}
	else if (input[*i] == '<')
	{
		token = new_token(TOKEN_REDIRECT_IN, &input[*i], 1);
		(*i)++;
	}
	else if (input[*i] == '|')
	{
		token = new_token(TOKEN_PIPE, &input[*i], 1);
		(*i)++;
	}
	else
		return ;
	if (!token)
	{
		return ;
		// TODO: free all head and current pointers
	}
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
	t_token	*token;

	quote_char = input[*i];	// Store opening quote character
	start = ++(*i);			// Move past the opening quote
	while (input[*i] && input[*i] != quote_char)
		(*i)++;
	if (input[*i] != quote_char)
	{
		ft_printf("Error: Unmatched quote\n");
		return ;
	}
	len = *i - start;		// Length of the quoted string
	(*i)++;					// Move past the closing quote
	token = new_token(TOKEN_WORD, &input[start], len);
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
	t_token	*token;
	
	start = *i;
	// Collecting characters until a space or operator
	while (input[*i] && !ft_isspace(input[*i]) && !is_operator(input[*i]))
		(*i)++;
	len = *i - start; // Calculating length of the word
	token = new_token(TOKEN_WORD, &input[start], len);
	if (!*head)
		*head = token;
	else//Append to the current list
		(*current)->next = token;
	*current = token;// Update current pointer
}
