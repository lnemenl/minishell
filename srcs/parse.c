/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:47:52 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/10 16:53:24 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../include/parser.h"

// char	**fetch_paths(char **envp)
// {
// 	int		i;
// 	char	*check_path;
// 	char	**paths;

// 	i = 0;
// 	paths = NULL;
// 	check_path = NULL;
// 	while (envp[i])
// 	{
// 		if (ft_strnstr(envp[i], "PATH=", 5))
// 		{
// 			check_path = ft_strdup(envp[i]);
// 			if (!check_path)
// 				error_ret(6, NULL);
// 		}
// 		i++;
// 	}
// 	if (check_path)
// 	{
// 		paths = ft_split(check_path + 5, ':');
// 		free(check_path);
// 		if (!paths)
// 			error_ret(6, NULL);
// 	}
// 	return (paths);
// }

// void	parse(t_mshell *obj)
// {
// 	ft_printf("%s", obj->cmd_line);
// }


char **parse_line(const char *input_line)
{
	char *trimmed;
	char **tokens;
	
	if (!input_line)
		return (NULL);
	trimmed = ft_strtrim(input_line, "\t\n\r\f\v");
	if (!trimmed)
		return (NULL);
	if (trimmed[0] == '\0')
	{
		free(trimmed);
		return (NULL);
	}
	
	tokens = ft_split(trimmed, ' ');
	free(trimmed);
	return (tokens);
}

t_token_type get_token_type(const char *str)
{
	if (ft_strcmp(str, "|") == 0)
		return (TOKEN_PIPE);
	else if (ft_strcmp(str, ">") == 0)
		return (TOKEN_REDIRECT_OUT);
	else if (ft_strcmp(str, ">>") == 0)
		return (TOKEN_REDIRECT_APPEND);
	else if (ft_strcmp(str, "<") == 0)
		return (TOKEN_REDIRECT_IN);
	else if (ft_strcmp(str, "<<") == 0)
		return (TOKEN_HEREDOC);
	else
		return (TOKEN_WORD);
}
