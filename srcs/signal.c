/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 12:07:30 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/20 17:31:05 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** ioctl is needed to get out of the read loop.
** if it fails, we need to use raise in the main program.
** ctrl+Z will then wait for next keyboard event before properly pausing.
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
** exit is here in case of ioctl failure. But a raise(SIGINT) would be better.
*/

static inline void	s_exit(int signo)
{
	int	fd;

	if (signo == SIGINT || signo == SIGABRT || signo == SIGQUIT
	|| signo == SIGTERM || signo == SIGHUP)
	{
		fd = g_term->fd;
		ft_dlistdel(&(g_term->dlist));
		load_saved_terminal(g_term);
		signal(SIGINT, SIG_DFL);
		if (g_term->flag & SELECT_T)
		{
			close(fd);
			ioctl(STDIN_FILENO, TIOCSTI, "\x03");
		}
		else
			ioctl(fd, TIOCSTI, "\x03");
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
		ioctl(term->fd, TIOCSTI, "\x1a");
		term->flag &= ~SELECT_CTRLZ;
		signal_setup();
		if (get_terminal(term) || load_new_terminal(g_term))
		{
			ft_dprintf(STDERR_FILENO, "ft_select: cannot reload terminal.\n");
			return (-1);
		}
		return (1);
	}
	return (0);
}
