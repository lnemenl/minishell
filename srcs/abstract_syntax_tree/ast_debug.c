/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_debug.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:17:20 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/01 16:35:56 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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

