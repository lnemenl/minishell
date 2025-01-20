/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/20 16:58:46 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	realloc_buffer(char **buf, size_t *buffer_size)
{
	t_mshell *obj;
	obj = NULL;
	*buffer_size *= 2;
	free(*buf);
	*buf = ft_calloc(*buffer_size, sizeof(char));
	if (!*buf)
	{
		error_ret(5, NULL, obj);
		return ;
	}
}

int	open_dir(const char *dir)
{
	char	*buf;
	char	*path;
	char	*full_path;
	size_t	buffer_size;
	t_mshell *obj;

	obj = NULL;
	buffer_size = 50;
	if (!dir || !*dir)
		chdir(getenv("HOME"));
	buf = ft_calloc(buffer_size, sizeof(char));
	if (!buf)
	{
		//cleanup struct
		return (error_ret(5, NULL, obj));
	}
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	path = ft_strjoin(buf, "/");
	full_path = ft_strjoin(path, dir);
	free(path);
	chdir(full_path);
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	free(buf);
	free(full_path);
	return (1);
}

void	pwd(void)
{
	size_t		buffer_size;
	char		*buf;
	t_mshell	*obj;

	obj = NULL;
	buffer_size = 50;
	buf = ft_calloc(buffer_size, sizeof(char));
	if (!buf)
	{
		//cleanup struct
		error_ret(5, NULL, obj);
	}
	while (!getcwd(buf, buffer_size))
		realloc_buffer(&buf, &buffer_size);
	printf("%s\n", buf);
	free(buf);
}

void	set_env_args(t_mshell *obj, t_ast_node *node)
{
	int	i;

	i = 0;
	while (node->args[i])
		i++;
	ft_free_strs(node->args, i);
	node->args = ft_calloc(3, sizeof(char *));
	if (!node->args)
	{
		clean_mshell(obj);
		return ;
	}
	node->args[0] = ft_strdup("cat");
	node->args[1] = ft_strdup(".env_temp.txt");
	node->args[2] = NULL;
}

int	env(void) //(t_mshell *obj) //, t_ast_node *node)
{
	int			fd;
	char		*str;
	t_mshell	*obj;

	obj = NULL;
	printf("env runs\n");
	fd = open(".env_temp.txt", O_RDONLY);
	if (fd == -1)
	{
		//cleanup
		return (error_ret(6, NULL, obj));
	}
	str = get_next_line(fd);
	while (str)
	{
		printf("%s", str);
		free(str);
		str = get_next_line(fd);
	}
	close(fd);
	//execve(obj->cur_path, node->args, NULL);
	return (1);
}

void	echo(char **args)
{
	int	i;

	if (!args || !*args)
	{
		printf("\n");
		return ;
	}
	i = 1;
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	printf("\n");
}
