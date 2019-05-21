/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 12:07:30 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/21 18:27:46 by aulopez          ###   ########.fr       */
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


static inline void	s_exit(int signo)
{
	if (signo == SIGINT || signo == SIGABRT || signo == SIGQUIT
	|| signo == SIGTERM || signo == SIGHUP)
	{
		load_saved_terminal(g_term);
		signal(SIGWINCH, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGABRT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		ft_dlistdel(&(g_term->dlist));
		if (g_term->flag & SELECT_T)
			close(g_term->fd);
		exit(signo);
	}
}

/*
** SIGWINCH -> Window resize
** SIGTSTP -> Pause program
** SIGCONT -> Continue paused program: not needed here
** SIGINT, SIGABRT, SIGQUIT -> various quit signal
*/

void				signal_setup(void)
{
	signal(SIGWINCH, s_flag);
	signal(SIGTSTP, s_flag);
	signal(SIGINT, s_exit);
	signal(SIGTERM, s_exit);
	signal(SIGABRT, s_exit);
	signal(SIGQUIT, s_exit);
	signal(SIGHUP, s_exit);
}

/*
** raise(SIGTSTP) would be better instead of ioctl.
*/

int					key_signal(t_term *term)
{
	if (term->flag & SELECT_RESIZE)
	{
		term->flag &= ~SELECT_RESIZE;
		return (1);
	}
	if (term->flag & SELECT_CTRLZ)
	{
		load_saved_terminal(term);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGWINCH, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGABRT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		ioctl(term->fd, TIOCSTI, "\x1A");
		term->flag &= ~SELECT_CTRLZ;
		signal_setup();
		if (get_terminal(term) || load_new_terminal(g_term))
		{
			ft_dprintf(STDERR_FILENO, "ft_select: cannot reload terminal.\n");
			return (-1);
		}
		if (!(g_term->flag & SELECT_CC))
		   tputs(tgetstr("vi", NULL), 1, g_term->putchar);
		return (1);
	}
	return (0);
}
