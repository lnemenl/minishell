/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:17:20 by rkhakimu          #+#    #+#             */
/*   Updated: 2024/12/24 14:12:15 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

t_ast_node	*create_ast_node(t_token_type type)
{
	t_ast_node	*node;
	
	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->args = NULL;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

t_ast_node	*build_command_node(t_token **tokens)
{
	t_ast_node	*node;
	t_token		*current;
	int			arg_count;
	int			i;

	current = *tokens;
	arg_count = 0;
	while (current && current->type == TOKEN_WORD)
	{
		arg_count++;
		current = current->next;
	}
	if (arg_count == 0)
		return (NULL);
	node = create_ast_node(TOKEN_WORD);
	node->args = malloc(sizeof(char *) * (arg_count + 1));
	if (!node->args)
		return (NULL);
	i = 0;
	while (i < arg_count)
	{
		node->args[i] = ft_strdup((*tokens)->start);
		*tokens = (*tokens)->next;
		i++;
	}
	node->args[i] = NULL;
	return (node);
}

t_ast_node	*parse_pipeline(t_token **tokens)
{
	t_ast_node	*left;
	t_ast_node	*pipe_node;
	t_token		*current;

	if (!tokens || !*tokens)
		return (NULL);
	left = parse_command(tokens);
	if (!left)
		return (NULL);
	current = *tokens;
	if (current && current->type == TOKEN_PIPE)
	{
		pipe_node = create_ast_node(TOKEN_PIPE);
		*tokens = (*tokens)->next;
		pipe_node->left = left;
		pipe_node->right = parse_pipeline(tokens);
		return (pipe_node);
	}
	return (left);
}

t_ast_node	*handle_redirection(t_token **tokens, t_ast_node *cmd_node)
{
	t_ast_node	*redir_node;
	t_token		*current;

	current = *tokens;
	if (!current || !cmd_node)
		return (cmd_node);
	if (current->type == TOKEN_REDIRECT_IN ||
			current->type == TOKEN_REDIRECT_OUT ||
			current->type == TOKEN_REDIRECT_APPEND ||
			current->type == TOKEN_HEREDOC)
	{
		redir_node = create_ast_node(current->type);
		*tokens = (*tokens)->next;
		if (!*tokens || (*tokens)->type != TOKEN_WORD)
			return (NULL);
		redir_node->left = cmd_node;
		redir_node->args = malloc(sizeof(char *) * 2);
		redir_node->args[0] = ft_strdup((*tokens)->start);
		redir_node->args[1] = NULL;
		*tokens = (*tokens)->next;
		return(handle_redirection(tokens, redir_node));
	}
	return (cmd_node);
}

t_ast_node	*parse_command(t_token **tokens)
{
	t_ast_node	*cmd_node;
	cmd_node = build_command_node(tokens);
	if (!cmd_node)
		return (NULL);
	return (handle_redirection(tokens, cmd_node));
}

void	free_ast(t_ast_node *node)
{
	int	i;
	
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->args)
	{
		i = 0;
		while (node->args[i])
		{
			free(node->args[i]);
			i++;
		}
		free(node->args);
	}
	free(node);
}
//TO see whether this all works at the moment
void print_ast(t_ast_node *node, int depth)
{
    int i;
    int j;

    if (!node)
        return;
    i = 0;
    while (i < depth)
    {
        ft_printf("  ");
        i++;
    }
    ft_printf("Type: ");
    if (node->type == TOKEN_WORD)
    {
        ft_printf("WORD\n");
        i = 0;
        while (i < depth + 1)
        {
            ft_printf("  ");
            i++;
        }
        ft_printf("Args: ");
        j = 0;
        while (node->args && node->args[j])
        {
            ft_printf("[%s] ", node->args[j]);
            j++;
        }
        ft_printf("\n");
    }
    else if (node->type == TOKEN_PIPE)
        ft_printf("PIPE\n");
    else if (node->type == TOKEN_REDIRECT_OUT)
        ft_printf("REDIRECT_OUT: %s\n", node->args[0]);
    else if (node->type == TOKEN_REDIRECT_IN)
        ft_printf("REDIRECT_IN: %s\n", node->args[0]);
    else if (node->type == TOKEN_REDIRECT_APPEND)
        ft_printf("REDIRECT_APPEND: %s\n", node->args[0]);
    else if (node->type == TOKEN_HEREDOC)
        ft_printf("HEREDOC: %s\n", node->args[0]);

    // Recursively print children
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
}

