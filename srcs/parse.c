/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/27 19:49:26 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


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


char **fetch_paths(char **envp)
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
    obj->ast = parse_pipeline(&tokens, obj);
    if (!obj->ast)
    {
        ft_printf("Error: Failed to create AST.\n");
        clean_token_list(tokens);
        return;
    }
    print_parse_debug(obj);
    clean_token_list(tokens);
}

static void    print_parse_debug(t_mshell *obj)
{
    if (!obj || !obj->cmd_line)
        return;
    ft_printf("\nAST Structure for: %s\n", obj->cmd_line);
    ft_printf("------------------------\n");
    print_ast(obj->ast, 0);
    ft_printf("------------------------\n\n");
}

static t_token    *process_token(t_token **head, t_token **current, 
                               const char *input, int *i)
{
    if (ft_isspace(input[*i]))
    {
        (*i)++;
        return (*current);
    }
    if (is_operator(input[*i]))
        add_operator_token(head, current, input, i);
    else if (is_quote(input[*i]))
        add_quoted_token(head, current, input, i);
    else
        add_word_token(head, current, input, i);
    if (!*current)
        return (NULL);
    return (*current);
}

static void    init_tokenize(t_token **head, t_token **current)
{
    *head = NULL;
    *current = NULL;
}

t_token    *tokenize(const char *input)
{
    t_token *head;
    t_token *current;
    char    *trimmed_input;
    int     i;

    if (!input)
        return (NULL);
    init_tokenize(&head, &current);
    trimmed_input = ft_strtrim(input, " \t\n\r\f\v");
    if (!trimmed_input)
        return (NULL);
    i = 0;
    while (trimmed_input[i])
    {
        if (!process_token(&head, &current, trimmed_input, &i))
        {
            clean_parse_error(&head, &current);
            free(trimmed_input);
            return (NULL);
        }
    }
    free(trimmed_input);
    return (head);
}

static char    *allocate_token_content(const char *start, int length)
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

t_token    *new_token(t_token_type type, const char *start, int length)
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

static t_token    *create_token_from_str(const char *str, t_token_type type, int len)
{
    if (!str || len <= 0)
        return (NULL);
    return (new_token(type, str, len));
}

static t_token    *handle_double_redirect(const char *input, int *i)
{
    t_token *token;

    token = NULL;
    if (!ft_strncmp(&input[*i], ">>", 2))
        token = create_token_from_str(">>", TOKEN_REDIRECT_APPEND, 2);
    else if (!ft_strncmp(&input[*i], "<<", 2))
        token = create_token_from_str("<<", TOKEN_HEREDOC, 2);
    if (token)
        (*i) += 2;
    return (token);
}

static t_token    *handle_single_operator(const char *input, int *i)
{
    t_token *token;

    token = NULL;
    if (input[*i] == '>')
        token = create_token_from_str(">", TOKEN_REDIRECT_OUT, 1);
    else if (input[*i] == '<')
        token = create_token_from_str("<", TOKEN_REDIRECT_IN, 1);
    else if (input[*i] == '|')
        token = create_token_from_str("|", TOKEN_PIPE, 1);
    if (token)
        (*i)++;
    return (token);
}

void    add_operator_token(t_token **head, t_token **current, 
                         const char *input, int *i)
{
    t_token *token;

    if (!input || !input[*i])
        return ;
    if (ft_strlen(&input[*i]) >= 2)
        token = handle_double_redirect(input, i);
    else
        token = handle_single_operator(input, i);
    if (!token)
    {
        clean_parse_error(head, current);
        return ;
    }
    if (!*head)
        *head = token;
    else
        (*current)->next = token;
    *current = token;
}

bool    is_quote(char c)
{
    return (c == '\'' || c == '\"');
}

static int    find_closing_quote(const char *input, int *i, char quote_char)
{
    if (!input || !input[*i])
        return (1);
    (*i)++;
    while (input[*i] && input[*i] != quote_char)
        (*i)++;
    if (!input[*i])
        return (1);
    return (0);
}

static void    link_token(t_token **head, t_token **current, t_token *new)
{
    if (!*head)
        *head = new;
    else
        (*current)->next = new;
    *current = new;
}

static t_token    *handle_quote_content(const char *input, int *i, char quote)
{
    t_token *token;
    int     start;
    int     len;

    start = *i + 1;
    *i = start;
    if (find_closing_quote(input, i, quote))
        return (NULL);
    len = *i - start;
    (*i)++;
    token = new_token(TOKEN_WORD, &input[start], len);
    if (token)
        token->quote_state = get_quote_state(quote);
    return (token);
}

void    add_quoted_token(t_token **head, t_token **current, 
                        const char *input, int *i)
{
    char    quote;
    t_token *token;

    if (!input || !input[*i])
        return ;
    quote = input[*i];
    token = handle_quote_content(input, i, quote);
    if (!token)
    {
        clean_parse_error(head, current);
        ft_printf("Error: Unmatched quote\n");
        return ;
    }
    link_token(head, current, token);
}

static t_token    *create_word_token(const char *input, int start, int len)
{
    if (!input || len <= 0)
        return (NULL);
    return (new_token(TOKEN_WORD, &input[start], len));
}

static int    calculate_word_length(const char *input, int *i)
{
    int start;

    if (!input || !input[*i])
        return (0);
    start = *i;
    while (input[*i] && !ft_isspace(input[*i]) && !is_operator(input[*i]))
        (*i)++;
    return (*i - start);
}

void    add_word_token(t_token **head, t_token **current, 
                      const char *input, int *i)
{
    int     start;
    int     len;
    t_token *token;

    if (!input || !input[*i])
        return ;
    start = *i;
    len = calculate_word_length(input, i);
    token = create_word_token(input, start, len);
    if (!token)
    {
        clean_parse_error(head, current);
        return ;
    }
    if (!*head)
        *head = token;
    else
        (*current)->next = token;
    *current = token;
}


// Cleaning functions

void    clean_token(t_token *token)
{
    if (!token)
        return;
    if (token->start)
        free(token->start);
    free(token);
}

void    clean_token_list(t_token *head)
{
    t_token *current;
    t_token *next;

    if (!head)
        return ;
    current = head;
    while (current)
    {
        next = current->next;
        clean_token(current);
        current = next;
    }
}

void    clean_parse_error(t_token **head, t_token **current)
{
    if (!head || !*head)
        return;
    clean_token_list(*head);
    *head = NULL;
    *current = NULL;
}