/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 14:14:26 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/23 16:24:13 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	open_dir(const char *dir)
{
	char	*buf;
	char	*path;
	char	*full_path;
	
	buf = ft_calloc(50, sizeof(char));
	getcwd(buf, 50);
	ft_printf("buf: %s\n", buf);
	path = ft_strjoin(buf, "/");
	full_path = ft_strjoin(path, dir);
	free(path);
	ft_printf("path: %s\n", full_path);
	chdir(full_path);
	getcwd(buf, 50);
	ft_printf("buf: %s\n", buf);
	free(buf);
	free(full_path);
}

void	pwd(void)
{
	int		buffer_size;
	char	*buf;

	buffer_size = 50;
	buf = ft_calloc(buffer_size, sizeof(char));
	if (!buf)
	{
		ft_fprintf(2, "Malloc error!\n");
		exit(1);
	}
	while (!getcwd(buf, buffer_size))
	{
		buffer_size *= 2;
		free(buf);
		buf = ft_calloc(buffer_size, sizeof(char));
		if (!buf)
		{
			ft_fprintf(2, "Malloc error!\n");
			exit(1);
		}
	}
	printf("%s\n", buf);
	free(buf);
}

void	env(char **envp) // ????
{
	int	i;

	i = 0;
	while (envp[i])
		printf("%s\n", envp[i++]);
	//execve("/usr/bin/env", envp, NULL);
}

void	echo(char **args)
{
	execve("/usr/bin/echo", args, NULL);
}

// int main(int argc, char **argv, char **envp)
// {
// 	char *args[] = {"echo", "-n", "hello", NULL};
// 	char *env_arg[] = {"env", NULL};
// 	open_dir("srcs");
// 	open_dir("..");
// 	pwd();
// 	env(envp);
// 	echo(args);
// }