/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 12:07:30 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/14 12:17:26 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

static inline void	s_resize(int signo)
{
	if (signo == SIGWINCH && g_term->flag == 0)
	{
		g_term->flag = 1;
		display_arg(g_term);
		g_term->flag = 0;
	}
}

/*
** ioctl does not works on WSL :'(
** use raise(SIGTSTP) instead
*/

static inline void	s_ctrl_z(int signo)
{
	if (signo == SIGTSTP || signo == SIGSTOP)
	{
		load_saved_terminal(g_term);
		signal(SIGTSTP, SIG_DFL);
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
		load_saved_terminal(g_term);
		ft_dlistdel(&(g_term->dlist));
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
	signal(SIGSTOP, s_ctrl_z);
	signal(SIGCONT, s_fg);
	signal(SIGINT, s_exit);
	signal(SIGABRT, s_exit);
	signal(SIGQUIT, s_exit);
}
