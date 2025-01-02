/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:11:55 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/02 11:59:18 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	handle_env_var(t_token **head, t_token **current, const char *input, int *i)
{
	t_token     *token;
	t_env_data  data;
	
	(*i)++;
	if (!input[*i])
		return ;
	if (input[*i] == '?')
	{
		token = new_token(TOKEN_EXIT_STATUS, &input[*i], 1);
		if (token)
			link_token(head, current, token);
		(*i)++;
		return ;
	}
	data.input = input;
	data.start = *i;
	data.len = get_env_var_len(&input[*i]);
	if (data.len > 0)
	{
		token = new_token(TOKEN_ENV_VAR, &input[data.start], data.len);
		if (token)
			link_token(head, current, token);
		(*i) += data.len;
	}
}

t_token	*handle_env_var_token(const char *input, int *i)
{
	t_token	*token;
	int     len;
	
	if (input[*i] == '?')
	{
		token = new_token(TOKEN_EXIT_STATUS, "?", 1);
		(*i)++;
		return (token);
	}
	len = get_env_var_len(&input[*i + 1]);
	if (len <= 0)
		return (NULL);
	token = new_token(TOKEN_ENV_VAR, &input[*i + 1], len);
	*i += len + 1;
	return (token);
}

void	add_exit_status_token(t_token **head, t_token **currnt)
{
    t_token *token;
    
    token = new_token(TOKEN_EXIT_STATUS, "?", 1);
    if (!token)
    {
        clean_parse_error(head, currnt);
        return ;
    }
    link_token(head, currnt, token);
}

int	get_env_var_len(const char *str)
{
	int len;

	len = 0;
	if (!str)
		return (0);
	while (str[len])
	{
		if (!ft_isalnum(str[len]) && str[len] != '_')
			break ;
		len++;
	}
	return (len);
}
