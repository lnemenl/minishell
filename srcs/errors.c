/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:57:23 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/20 15:37:08 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/*	Prints an error and exit the program

	The first argument:
	`1` - incorrect number of arguments
	`2` - perror
	`3` - pipe failed
	`4` - fork failed
	`5` - malloc failed
	`6` - file handling error

	The second argument:
	name of a command or `NULL`
*/
int	error_ret(int type, char *arg)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (type == 1)
		ft_putstr_fd("Do not provide any arguments\n", STDERR_FILENO);
	else if (type == 2)
		perror(arg);
	else if (type == 3)
		ft_putstr_fd("Pipe failed\n", STDERR_FILENO);
	else if (type == 4)
		ft_putstr_fd("Fork failed\n", STDERR_FILENO);
	else if (type == 5)
		ft_putstr_fd("Malloc failed\n", STDERR_FILENO);
	else if (type == 6)
		ft_putstr_fd("File handling error\n", STDERR_FILENO);
	exit(1);
}

void	print_exit(char *mes, char *cmd, t_mshell *obj)
{
	char	*full_msg;
	int		mes_len;

	mes_len = ft_strlen(mes) + ft_strlen(cmd) + 3;
	full_msg = ft_calloc(mes_len, sizeof(char));
	if (!full_msg)
		ft_putstr_fd("Malloc failed\n", 2);
	else
	{
		if (cmd)
			ft_strlcpy(full_msg, cmd, mes_len);
		ft_strlcat(full_msg, ": ", mes_len);
		ft_strlcat(full_msg, mes, mes_len);
		ft_putstr_fd(full_msg, 2);
		check_free_str(&full_msg);
	}
	clean_exit(obj);
}

void	exit_child(t_mshell *obj, char *arg, int exit_code, int is_builtin)
{
	obj->exit_code = exit_code;
	close_fds(obj);
	if (arg && !*arg)
		ft_putstr_fd(": ", 2);
	if (arg && obj->exit_code != 0 && is_builtin == 0)
		perror(arg);
	if (errno == EACCES && obj->exit_code != 1)
		obj->exit_code = 126;
	clean_exit(obj);
}
