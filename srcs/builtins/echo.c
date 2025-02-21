/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 13:46:25 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/21 16:58:36 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	check_multiple_n(char *arg)
{
	int	i;

	if (!arg || !*arg)
		return (0);
	if (ft_strncmp(arg, "-n", 2) != 0)
		return (0);
	i = 2;
	while (arg[i])
	{
		if (arg[i] == ' ')
			return (i + 1);
		else if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (i);
}

static size_t	check_no_nl(char **args, int *i, int is_quote)
{
	size_t	no_nl_len;

	no_nl_len = 0;
	if (ft_strcmp(args[1], "-n") == 0)
		(*i)++;
	else if (is_quote == 0)
	{
		no_nl_len = check_multiple_n(args[1]);
		if (no_nl_len != 0 && no_nl_len == ft_strlen(args[1]))
		{
			(*i)++;
			no_nl_len = 0;
		}
	}
	return (no_nl_len);
}

int	echo(char **args, t_mshell *obj, int is_quote)
{
	int		i;
	size_t	no_nl_len;

	if (!args[1])
	{
		printf("\n");
		return (1);
	}
	i = 1;
	no_nl_len = check_no_nl(args, &i, is_quote);
	while (args[i])
	{
		if (*(args[i] + no_nl_len) != '\0')
			printf("%s", args[i] + no_nl_len);
		if (args[i + 1])
			printf(" ");
		i++;
		no_nl_len = 0;
	}
	if (is_quote == 1 || (ft_strcmp(args[1], "-n") != 0
			&& check_multiple_n(args[1]) == 0))
		printf("\n");
	obj->exit_code = 0;
	return (1);
}
