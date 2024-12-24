/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/24 16:53:57 by rkhakimu         ###   ########.fr       */
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
    obj->ast = parse_pipeline(&tokens, obj);
	if (!obj->ast)
	{
		ft_printf("Error: Failed to create AST\n");
		while (tokens)
		{
			if (tokens->start)
				free(tokens->start);
			tmp = tokens->next;
			free(tokens);
			tokens = tmp;
		}
		return ;
	}
	 // Debug print
    ft_printf("\nAST Structure for: %s\n", obj->cmd_line);
    ft_printf("------------------------\n");
    print_ast(obj->ast, 0);
    ft_printf("------------------------\n\n");
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

int is_quote(char c)
{
    return (c == '\'' || c == '\"');
}

t_token	*tokenize(const char *input)
{
	t_token	*head;
	t_token	*current;
	char	*trimmed_input;
	int		i;

	if (!input)
		return (NULL);
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

static t_quote_state	get_quote_state(char quote_char)
{
	if (quote_char == '\'')
		return (QUOTE_SINGLE);
	return (QUOTE_DOUBLE);
}

static int	find_closing_quote(const char *input, int *i, char quote_char)
{
    int	j;
    
    j = *i;
    while (input[j] && input[j] != quote_char)
        j++;
    if (!input[j])
    {
        *i = j;
        return (1);
    }
    *i = j;
    return (0);
}

void	add_quoted_token(t_token **head, t_token **current, const char *input, int *i)
{
	char			quote_char;
	int				start;
	int				len;
	t_token			*token;
	t_quote_state	quote_state;

	quote_char = input[*i];	// Store opening quote character
	quote_state = get_quote_state(quote_char);
	start = *i + 1;
	*i = start;
	if (find_closing_quote(input, i, quote_char))
	{
		ft_printf("Error: Unmatched quote\n");
		return ;
	}
	len = *i - start;
	(*i)++;
	token = new_token(TOKEN_WORD, &input[start], len);
	if (!token)
		return ;
	token->quote_state = quote_state;
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
