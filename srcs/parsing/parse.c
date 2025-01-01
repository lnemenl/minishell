/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/01 18:08:37 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// void    parse(t_mshell *obj)
// void    print_parse_debug(t_mshell *obj) will be removed
// t_token *tokenize(const char *input)
// void    init_tokenize(t_token **head, t_token **current)
// t_token *process_trimmed_input(t_token **head, t_token **current, char *trimmed)
// char	**fetch_paths(chat **envp)

char	**fetch_paths(char **envp)
{
	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			return ft_split(*envp + 5, ':');
		envp++;
	}
	return (NULL);
}

void    parse(t_mshell *obj)
{
	t_token *tokens;

	if (!obj || !obj->cmd_line)
	{
		ft_printf("Error: Invalid input provided.\n");
		return;
	}
	tokens = tokenize(obj->cmd_line);
	if (!tokens)
	{
		ft_printf("Error: Failed to tokenize input.\n");
		return;
	}
	obj->ast = parse_pipeline(&tokens);
	if (!obj->ast)
	{
		ft_printf("Error: Failed to create AST.\n");
		clean_token_list(tokens);
		return;
	}
	print_ast(obj->ast, 0);
	print_parse_debug(obj);
	clean_token_list(tokens);
}

void    print_parse_debug(t_mshell *obj)
{
	if (!obj || !obj->cmd_line)
		return;
	ft_printf("\nAST Structure for: %s\n", obj->cmd_line);
	ft_printf("------------------------\n");
	print_ast(obj->ast, 0);
	ft_printf("------------------------\n\n");
}

void    init_tokenize(t_token **head, t_token **current)
{
	*head = NULL;
	*current = NULL;
}

t_token	*process_trimmed_input(t_token **head, t_token **current,
                                       char *trimmed_input)
{
    int    i;

    i = 0;
    while (trimmed_input[i])
    {
        if (!process_token(head, current, trimmed_input, &i))
        {
            clean_parse_error(head, current);
            free(trimmed_input);
            return (NULL);
        }
    }
    return (*head);
}

t_token	*tokenize(const char *input)
{
    t_token *head;
    t_token *current;
    t_token *result;
    char    *trimmed_input;

    if (!input)
        return (NULL);
    init_tokenize(&head, &current);
    trimmed_input = ft_strtrim(input, " \t\n\r\f\v");
    if (!trimmed_input || !trimmed_input[0])
    {
        free(trimmed_input);
        return (NULL);
    }
    result = process_trimmed_input(&head, &current, trimmed_input);
    free(trimmed_input);
    return (result);
}
