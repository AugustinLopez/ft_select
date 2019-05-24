/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 12:07:30 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/24 18:20:18 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** look for PID with top, kill with kill pid, foreground with kill -CONT pid
*/

/*
** ioctl is needed to get out of the read loop.
** if ioctls fails, changes will happens at next keyboard event.
*/

static inline void	s_flag(int signo)
{
	if (signo == SIGTSTP)
		g_term->flag |= SELECT_CTRLZ;
	else if (signo == SIGWINCH)
		g_term->flag |= SELECT_RESIZE;
	ioctl(g_term->fd, TIOCSTI, "a");
}

/*
** Best practice would be to raise the signal instead of exiting
*/

static inline void	s_exit(int signo)
{
	if (signo == SIGINT || signo == SIGABRT || signo == SIGQUIT
	|| signo == SIGTERM || signo == SIGHUP)
	{
		load_saved_terminal(g_term);
		signal_setup(DESACTIVATE);
		ft_dlistdel(&(g_term->dlist));
		if (g_term->flag & SELECT_T)
			close(g_term->fd);
		exit(signo);
	}
}

/*
** SIGWINCH -> Window resize
** SIGTSTP -> Pause program (can be catched)
** SIGHUP, SIGINT, SIGQUIT, SIGABRT, SIGTERM are termination signal
** We do not handle SIGCONT:
*/

void				signal_setup(int option)
{
	if (option)
	{
		signal(SIGHUP, s_exit);
		signal(SIGINT, s_exit);
		signal(SIGQUIT, s_exit);
		signal(SIGABRT, s_exit);
		signal(SIGTERM, s_exit);
		signal(SIGWINCH, s_flag);
		signal(SIGTSTP, s_flag);
	}
	else
	{
		signal(SIGHUP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGABRT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGWINCH, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
	}
}
