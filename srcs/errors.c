/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:57:23 by msavelie          #+#    #+#             */
/*   Updated: 2025/01/20 16:47:27 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/*  Returns appropriate error code without exiting the program
    
    The first argument:
    `1` - incorrect number of arguments
    `2` - syntax error
    `3` - pipe failed
    `4` - fork failed
    `5` - malloc failed
    `6` - file handling error
    `7` - command not found (127)
    
    The second argument:
    token/command name or error message
*/
int error_ret(int type, char *arg, t_mshell *obj)
{
    ft_putstr_fd("minishell: ", 2);
    if (type == 1)
    {
        ft_putstr_fd("Do not provide any arguments\n", 2);
        if (obj)
		    obj->exit_code = GENERAL_ERROR;
        return (GENERAL_ERROR);
    }
    else if (type == 2)
    {
        ft_putstr_fd("syntax error near unexpected token `", 2);
        if (arg)
            ft_putstr_fd(arg, 2);
        else
            ft_putstr_fd("newline", 2);
        ft_putstr_fd("'\n", 2);
		obj->exit_code = SYNTAX_ERROR;
        return (SYNTAX_ERROR);
    }
    else if (type == 3)
    {
        ft_putstr_fd("pipe failed\n", 2);
		obj->exit_code = GENERAL_ERROR;
        return (GENERAL_ERROR);
    }
    else if (type == 4)
    {
        ft_putstr_fd("fork failed\n", 2);
		obj->exit_code = GENERAL_ERROR;
        return (GENERAL_ERROR);
    }
    else if (type == 5)
    {
        ft_putstr_fd("malloc failed\n", 2);
		obj->exit_code = GENERAL_ERROR;
        return (GENERAL_ERROR);
    }
    else if (type == 6)
    {
        ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
		perror("");
		obj->exit_code = GENERAL_ERROR;
		return (GENERAL_ERROR);
    }
    else if (type == 7)
    {
        ft_putstr_fd(arg, 2);
        ft_putstr_fd(": command not found\n", 2);
		obj->exit_code = 127;
        return (127);
    }
	obj->exit_code = GENERAL_ERROR;
    return (GENERAL_ERROR);
}
