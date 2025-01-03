/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_command.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 16:28:31 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/03 12:11:25 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int			count_word_tokens(t_token *token);
static int			init_command_args(t_ast_node *node, int arg_count);
static t_ast_node	*free_command_node(t_ast_node *node, int i);


static char	*process_quoted_content(const char *content)
{
    char	*processed;
    int		i;
    int		j;
    //char    outer_quote;

    processed = ft_calloc(ft_strlen(content) + 1, sizeof(char));
    if (!processed)
        return (NULL);
    i = 0;
    j = 0;
    //outer_quote = 0;
    while (content[i])
    {
        // if (!outer_quote && (content[i] == '\'' || content[i] == '"'))
        //     outer_quote = content[i];
        // else if (content[i] == outer_quote)
        //     outer_quote = 0;
        // else
        processed[j++] = content[i];
        i++;
    }
    processed[j] = '\0';
    return (processed);
}

static int count_word_tokens(t_token *token)
{
    int count;

    count = 0;
    while (token && token->type == TOKEN_WORD)
    {
        count++;
        token = token->next;
    }
    return (count);
}

t_ast_node *build_command_node(t_token **tokens)
{
    t_ast_node  *node;
    int         arg_count;
    int         i;
    t_token     *current;

    if (!tokens || !*tokens)
        return (NULL);
    node = ft_calloc(1, sizeof(t_ast_node));
    if (!node)
        return (NULL);
    node->type = TOKEN_WORD;
    arg_count = count_word_tokens(*tokens);
    node->args = ft_calloc(arg_count + 1, sizeof(char *));
    if (!node->args)
    {
        free(node);
        return (NULL);
    }
    i = 0;
    current = *tokens;
    while (i < arg_count)
    {
        node->args[i] = ft_strdup(current->content);
        if (!node->args[i])
        {
            while (--i >= 0)
                free(node->args[i]);
            free(node->args);
            free(node);
            return (NULL);
        }
        current = current->next;
        i++;
    }
    node->args[arg_count] = NULL;
    *tokens = current;
    return (node);
}

static int	init_command_args(t_ast_node *node, int arg_count)
{
	node->args = malloc(sizeof(char *) * (arg_count + 1));
	if (!node->args)
		return (0);
	return (1);
}

static t_ast_node	*free_command_node(t_ast_node *node, int i)
{
	while (i > 0)
		free(node->args[--i]);
	free(node->args);
	free(node);
	return (NULL);
}