/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 15:34:32 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/02 14:16:18 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static int	is_env_created(char *arg, char **strs)
{
	int		i;
	size_t	len;

	len = 0;
	while (arg[len] != '=')
		len++;
	i = 0;
	while (strs[i])
	{
		if (ft_strncmp(strs[i], arg, len) == 0)
			return (i);
		i++;
	}
	return (-1);
}

static char	*check_env_arg(char *arg)
{
	char	*new_arg;
	size_t	len;
	int		equal;

	len = ft_strlen(arg);
	equal = 1;
	if (ft_strchr(arg, '=') && !ft_strchr(arg, '=') + 1)
		equal = 0;
	if (!ft_strchr(arg, '='))
	{
		new_arg = ft_calloc(len + 2, 1);
		if (!new_arg)
		{
			//cleanup
		}
		new_arg = ft_memcpy(new_arg, arg, len);
		if (equal == 1)
			ft_strlcat(new_arg, "=", len + 2);
		else
			ft_strlcat(new_arg, "", len + 1);
		return (new_arg);
	}
	return (ft_strdup(arg));
}

static void	put_env_var(char **strs, char *new_arg, size_t i)
{
	int		pos;
	size_t	arg_len;
	char	*last_str;

	arg_len = ft_strlen(new_arg);
	pos = is_env_created(new_arg, strs);
	if (pos == -1)
	{
		last_str = strs[i];
		strs[i] = ft_calloc(arg_len + 2, 1);
		if (!strs[i])
		{
			//ft_free_strs(strs, i);
			exit(error_ret(6, NULL));
		}
		strs[i] = ft_memcpy(strs[i], new_arg, arg_len);
		strs[i][arg_len] = '\n';
		
		strs[++i] = last_str;
		strs[++i] = NULL;
		return ;
	}
	strs[pos] = ft_calloc(arg_len + 2, 1);
	if (!strs[pos])
	{
		//ft_free_strs(strs, strs_len);
		exit(error_ret(6, NULL));
	}
	strs[pos] = ft_memcpy(strs[pos], new_arg, arg_len);
	strs[pos][arg_len] = '\n';
}

static void	append_env(char *arg, char **strs, size_t i)
{
	int		fd;
	int		j;
	char	*new_arg;

	unlink(".env_temp.txt");
	fd = open(".env_temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		//cleanup struct
		ft_free_strs(strs, i);
		unlink(".env_temp.txt");
		exit(error_ret(6, NULL));
	}
	new_arg = check_env_arg(arg);
	put_env_var(strs, new_arg, i);
	free(new_arg);
	j = 0;
	while (strs[j])
	{
		if (write(fd, strs[j], ft_strlen(strs[j])) == -1)
		{
			close(fd);
			unlink(".env_temp.txt");
			exit(error_ret(6, NULL));
		}
		j++;
	}
	close(fd);
}

int	export(char **args)
{
	char	**strs;
	size_t	i;
	int		fd;

	if (!args || !args[1] || !*args[1])
		return (1);
	fd = open(".env_temp.txt", O_RDONLY);
	if (fd == -1)
	{
		//cleanup
		error_ret(6, NULL);
	}
	i = 0;
	strs = read_alloc(fd, &i);
	append_env(args[1], strs, --i);
	ft_free_strs(strs, ++i);
	return (1);
}
