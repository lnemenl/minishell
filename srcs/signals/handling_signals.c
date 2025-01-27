/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handling_signals.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:18:33 by rkhakimu          #+#    #+#             */
/*   Updated: 2025/01/27 14:36:51 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*volatile: tells compiler to always read from memory (not cache)
sig_atomic_t: guarantees atomic (uninterruptible) operations
Used to track which signal was received*/

volatile sig_atomic_t g_signo = 0;

void    handle_sigint(int sig)
{
    g_signo = sig;
    
    // If we're in interactive mode (at prompt)
    if (rl_on_new_line() == -1)
        return;
    write(STDERR_FILENO, "\n", 1);
    rl_replace_line("", 0);
    rl_redisplay();
}

void    handle_sigquit(int sig)
{
    g_signo = sig;
}


void    setup_signals(t_shell_state state)
{
    struct sigaction    sa_int;
    struct sigaction    sa_quit;

    sigemptyset(&sa_int.sa_mask);
    sigemptyset(&sa_quit.sa_mask);
    
    if (state == SHELL_INTERACTIVE)
    {
        sa_int.sa_handler = handle_sigint;
        sa_quit.sa_handler = SIG_IGN;  // Ignore SIGQUIT in interactive mode
    }
    else if (state == SHELL_EXECUTING)
    {
        sa_int.sa_handler = SIG_DFL;   // Default behavior for running commands
        sa_quit.sa_handler = SIG_DFL;
    }
    else if (state == SHELL_HEREDOC)
    {
        sa_int.sa_handler = handle_sigint;
        sa_quit.sa_handler = SIG_IGN;
    }

    sa_int.sa_flags = SA_RESTART;
    sa_quit.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa_int, NULL);
    sigaction(SIGQUIT, &sa_quit, NULL);
}

/*
** Save original terminal settings when shell starts
** Need these to restore them when shell exits
*/
void    save_terminal_settings(t_signal_state *state)
{
    if (tcgetattr(STDIN_FILENO, &state->original_term) == -1)
    {
        perror("tcgetattr");
        exit(1);
    }
    // Make a copy for shell's modified settings
    state->shell_term = state->original_term;
}

/*
** Configure terminal settings for shell operation
** Main purpose: disable ECHOCTL to prevent ^C ^\ from being displayed
*/
void    configure_terminal_settings(t_signal_state *state)
{
    // Make a copy of original settings
    state->shell_term = state->original_term;
    
    // Disable ECHOCTL flag (controls display of ^C, ^\)
    state->shell_term.c_lflag &= ~ECHOCTL;
    
    // Apply these settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &state->shell_term) == -1)
    {
        perror("tcsetattr");
        exit(1);
    }
}

/*
** Restore original terminal settings
** Important: call this before shell exits
*/
void    restore_terminal_settings(t_signal_state *state)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &state->original_term) == -1)
    {
        perror("tcsetattr");
        exit(1);
    }
}

/*
** Initialize terminal settings and signal state
*/
void    init_signal_state(t_signal_state *state)
{
    state->current_state = SHELL_INTERACTIVE;
    state->last_exit_code = 0;
	    
    // Only modify terminal settings if we're in a terminal
    if (isatty(STDIN_FILENO))
    {
        save_terminal_settings(state);
        configure_terminal_settings(state);
    }
}

/*
** Changes the shell state and updates signal handlers accordingly
** This is needed because different shell states require different signal handling:
** - Interactive mode: Custom SIGINT handler, ignore SIGQUIT
** - Executing mode: Default handlers for both signals
** - Heredoc mode: Custom SIGINT handler, ignore SIGQUIT
*/
void    change_shell_state(t_signal_state *state, t_shell_state new_state)
{
    state->current_state = new_state;
    setup_signals(new_state);
}

/*
** Special signal handler for heredoc
** 1. Handle SIGINT (Ctrl-C) differently than regular interactive mode
** 2. Clean up and exit gracefully when interrupted
** 3. Not display a new prompt when interrupted
*/
void    handle_heredoc_sigint(int sig)
{
    g_signo = sig;
    write(STDERR_FILENO, "\n", 1);
    close(STDIN_FILENO);  // This will cause readline to return NULL
}

/*
** Sets up signal handlers specifically for heredoc
** Returns to previous handlers when done
*/
void    setup_heredoc_signals(void)
{
    struct sigaction    sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handle_heredoc_sigint;
    sa.sa_flags = 0;  // Don't use SA_RESTART here
    
    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

/*
** Reset signals to default behavior for child processes
** This ensures child processes handle signals properly
** (like cat, grep, etc. responding to Ctrl-C correctly)
*/
void    reset_signals_default(void)
{
    struct sigaction    sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

/*
** Set up signal handling for parent shell during command execution
** Parent should ignore signals and let them pass to child
*/
void    setup_parent_signals(void)
{
    struct sigaction    sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

/*
** Cleanup function for proper shell exit
** - Restores terminal settings
** - Clears readline history
** Should be called before shell exits
*/
void    cleanup_signal_state(t_signal_state *state)
{
    if (isatty(STDIN_FILENO))
        restore_terminal_settings(state);
    rl_clear_history();
}