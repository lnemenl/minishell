/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:57:23 by msavelie          #+#    #+#             */
/*   Updated: 2024/12/30 11:57:14 by msavelie         ###   ########.fr       */
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
	ft_putstr_fd("minishell: ", 2);
	if (type == 1)
		ft_putstr_fd("Do not provide any arguments\n", 2);
	else if (type == 2)
		perror(arg);
	else if (type == 3)
		ft_putstr_fd("Pipe failed\n", 2);
	else if (type == 4)
		ft_putstr_fd("Fork failed\n", 2);
	else if (type == 5)
		ft_putstr_fd("Malloc failed\n", 2);
	else if (type == 6)
		ft_putstr_fd("File handling error\n", 2);
	exit(1);
}
