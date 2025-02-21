/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/01 15:05:13 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/02/21 15:18:33 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

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

int	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

t_quote_state	get_quote_state(char quote)
{
	if (quote == '"')
		return (QUOTE_DOUBLE);
	return (QUOTE_SINGLE);
}
