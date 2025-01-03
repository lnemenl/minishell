/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/03 12:23:18 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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
        return;
    tokens = tokenize(obj->cmd_line, obj);
    if (!tokens)
        return;
    obj->ast = parse_pipeline(&tokens);
    if (!obj->ast)
    {
        clean_tokens(tokens);  // Updated to use new function name
        return;
    }
	print_ast(obj->ast, 0);
	print_parse_debug(obj);
	clean_tokens(tokens);
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

t_token *process_trimmed_input(t_token **head, t_token **current, char *trimmed_input, t_mshell *mshell)
{
    int i;
    t_token *first_token;

    i = 0;
    // Creating initial empty token to store mshell
    first_token = new_token(TOKEN_WORD, "", 0);
    if (!first_token)
        return (NULL);
    first_token->mshell = mshell;
    *head = first_token;
    *current = first_token;

    while (trimmed_input[i])
    {
        if (!process_token(head, current, trimmed_input, &i))
        {
            clean_tokens(*head);
            return (NULL);
        }
    }
    // Removing the initial empty token
    *head = (*head)->next;
    free(first_token->content);
    free(first_token);
    return (*head);
}

t_token	*tokenize(const char *input, t_mshell *mshell)
{
    t_token	*head;
    t_token	*current;
    t_token	*result;
    char	*trimmed_input;

    if (!input)
        return (NULL);
    init_tokenize(&head, &current);
    trimmed_input = ft_strtrim(input, " \t\n\r\f\v");
    if (!trimmed_input || !trimmed_input[0])
    {
        free(trimmed_input);
        return (NULL);
    }
    result = process_trimmed_input(&head, &current, trimmed_input, mshell);
    if (result)
        result->mshell = mshell;
    free(trimmed_input);
    return (result);
}
