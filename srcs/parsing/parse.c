/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/30 19:20:50 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	**fetch_paths(char **envp)
{
    char    *path;
    char    **paths;

    if (!envp)
        return (NULL);
    path = get_env_var(envp, "PATH");
    if (path)
    {
        paths = ft_split(path, ':');
        free(path);
        return (paths);
    }
	return (NULL);
}

void print_tokens(t_token *tokens)
{
    while (tokens)
    {
        printf("Token: type=%d, content='%s'\n", 
               tokens->type, tokens->content);
        tokens = tokens->next;
    }
}

void    parse(t_mshell *obj)
{
	t_token *tokens;
    t_token *temp;
    int     i;

    if (!obj || !obj->cmd_line)
        return;
    tokens = tokenize(obj->cmd_line, obj);
    if (!tokens)
        return;
    obj->token = tokens;
    temp = tokens;
    while (temp)
	{
		if (temp->type == TOKEN_PIPE)
			obj->pipes_count++;
		temp = temp->next;
	}
    i = 0;
    obj->ast = parse_pipeline(&tokens, i, obj);
    if (!obj->ast)
    {
        clean_tokens(tokens);
        return ;
    }
}

void    print_parse_debug(t_mshell *obj)
{
	if (!obj || !obj->cmd_line)
		return ;
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

// t_token *process_trimmed_input(t_token **head, t_token **current, char *trimmed_input, t_mshell *mshell)
// {
//     int i;
//     t_token *first_token;

//     i = 0;
//     // Creating initial empty token to store mshell
//     first_token = new_token(TOKEN_WORD, "", 0, mshell);
//     if (!first_token)
//         return (NULL);
//     //first_token->mshell = mshell;
//     *head = first_token;
//     *current = first_token;

//     while (trimmed_input[i])
//     {
//         if (!process_token(head, current, trimmed_input, &i))
//         {
//             clean_tokens(*head);
//             return (NULL);
//         }
//     }
//     *head = (*head)->next;
//     free(first_token->content);
//     free(first_token);
//     return (*head);
// }

t_token *process_trimmed_input(t_token **head, t_token **current, char *trimmed_input, t_mshell *mshell)
{
    int i;
    t_token *first_token;

    i = 0;
    first_token = new_token(TOKEN_WORD, "", 0, mshell);
    if (!first_token)
        return (NULL);
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
    *head = (*head)->next;
    free(first_token->content);
    free(first_token);
    return (*head);
}
t_token *tokenize(const char *input, t_mshell *mshell)
{
    t_token *head;
    t_token *current;
    t_token *tokens;
    char    *trimmed_input;

    if (!input)
        return (NULL);
    trimmed_input = ft_strtrim(input, " \t\n\r\f\v");
    if (!trimmed_input || !trimmed_input[0])
    {
        free(trimmed_input);
        return (NULL);
    }
    init_tokenize(&head, &current);
    tokens = process_trimmed_input(&head, &current, trimmed_input, mshell);
    free(trimmed_input);
    return (tokens);
}
