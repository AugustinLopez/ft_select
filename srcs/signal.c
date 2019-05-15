/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 12:07:30 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/15 17:24:46 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

static inline void	s_resize(int signo)
{
	if (signo == SIGWINCH && !(g_term->flag & SELECT_RESIZE))
	{
		g_term->flag |= SELECT_RESIZE;
		display_arg(g_term);
		g_term->flag &= ~SELECT_RESIZE;
	}
}

/*
** ioctl does not works on WSL :'(
** use raise(SIGTSTP) instead
*/

static inline void	s_ctrl_z(int signo)
{
	if (signo == SIGTSTP)
	{
		load_saved_terminal(g_term);
		signal(SIGWINCH, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGABRT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		ioctl(g_term->fd, TIOCSTI, "\x1A");
	}
}

static inline void	s_fg(int signo)
{
	if (signo == SIGCONT)
	{
		if (get_terminal(g_term) || load_new_terminal(g_term))
		{
			ft_dprintf(STDERR_FILENO, "ft_select: cannot reload terminal.\n");
			load_saved_terminal(g_term);
			ft_dlistdel(&(g_term->dlist));
			exit(1);
		}
		signal_setup();
		display_arg(g_term);
	}
}

static inline void	s_exit(int signo)
{
	if (signo == SIGINT || signo == SIGABRT || signo == SIGQUIT)
	{
		ft_dlistdel(&(g_term->dlist));
		load_saved_terminal(g_term);
		exit(signo);
	}
}

/*
** SIGWINCH -> Window resize
** SIGTSTP, SIGSTOP -> Pause program
** SIGCONT -> Continue paused program
** SIGINT, SIGABRT, SIGQUIT -> various quit signal
*/

void				signal_setup(void)
{
	signal(SIGWINCH, s_resize);
	signal(SIGTSTP, s_ctrl_z);
	signal(SIGCONT, s_fg);
	signal(SIGINT, s_exit);
	signal(SIGABRT, s_exit);
	signal(SIGQUIT, s_exit);
}
