/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/13 19:16:40 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

void				set_line(t_term *term)
{
	t_dlist	*tmp;
	int		i;

	i = 0;
	tmp = term->dlist;
	while (1)
	{
		if (!(i++ % term->col) || (tmp->flag & FT_FIRST))
			tmp->flag |= FT_LINE;
		else
			tmp->flag &= ~FT_LINE;
		tmp = tmp->next;
		if (tmp->flag & FT_FIRST)
			return ;
	}
}

long				read_keypress(t_term *term)
{
	long	key;
	t_dlist	*tmp;
	int		i;
	int		bef;
	int		aft;
	int		ret;

	(void)bef;
	(void)aft;
	bef = 0;
	aft = 0;
	while (1)
	{
		key = 0;
		display_arg(term);
		ret = read(term->fd, &key, sizeof(key));
		(void)ret;
		set_line(term);
		//parcourir ici pour setup flag : start of line
		if (key == KEY_BACKSPACE || key == KEY_DELETE)
		{
			tmp = term->dcursor->next;
			tmp->flag |= FT_CURSOR;
			tmp->prev = term->dcursor->prev;
			tmp->prev->next = tmp;
			if (term->dcursor->flag & FT_FIRST)
			{
				term->dlist = tmp;
				tmp->flag |= FT_FIRST;
			}
			if (term->dcursor == tmp)
				return (0);
			free(term->dcursor);
			term->dcursor = tmp;
			term->ac--;
		}
		else if (key == KEY_LEFT)
		{
			term->dcursor->flag &= ~ FT_CURSOR;
			if (term->dcursor->flag & FT_LINE)
			{
				aft = 1;
				while (aft++ < term->col && !(term->dcursor->next->flag & FT_LINE))
					term->dcursor = term->dcursor->next;
				term->dcursor->flag |= FT_CURSOR;
			}
			else
			{
				term->dcursor->prev->flag |= FT_CURSOR;
				term->dcursor = term->dcursor->prev;
			}
		}
		else if (key == KEY_UP)
		{
			if (term->row == 1)
				continue ;
			if (term->col == 1)
			{
				term->dcursor->flag &= ~FT_CURSOR;
				term->dcursor->prev->flag |= FT_CURSOR;
				term->dcursor = term->dcursor->prev;
				continue ;
			}
			i = term->col;
			term->dcursor->flag &= ~FT_CURSOR;
			tmp = term->dcursor;
			aft = 0;
			while (i--)
			{
				aft++;
				if (tmp->flag & FT_FIRST)
				{
					if (term->ac % term->col >= aft)
						i = term->ac % term->col - aft;
					else
						i += (term->ac % term->col);
				}
				tmp = tmp->prev;
			}
			term->dcursor = tmp;
			tmp->flag |= FT_CURSOR;
		}
		else if (key == KEY_DOWN)
		{
			if (term->row == 1)
				continue ;
			if (term->col == 1)
			{
				term->dcursor->flag &= ~FT_CURSOR;
				term->dcursor->next->flag |= FT_CURSOR;
				term->dcursor = term->dcursor->next;
				continue ;
			}
			i = term->col;
			term->dcursor->flag &= ~FT_CURSOR;
			tmp = term->dcursor;
			aft = 0;
			while (i--)
			{
				aft++;
				if (tmp->next->flag & FT_FIRST)
				{
					if (term->ac % term->col >= aft)
						i = term->ac % term->col - aft;
					else
						i += (term->ac % term->col);
				}
				tmp = tmp->next;
			}
			term->dcursor = tmp;
			tmp->flag |= FT_CURSOR;

		}
		else if (key == KEY_RIGHT)
		{
			term->dcursor->flag &= ~FT_CURSOR;
			if (term->dcursor->next->flag & FT_LINE)
			{
				while (!(term->dcursor->flag & FT_LINE))
					term->dcursor = term->dcursor->prev;
				term->dcursor->flag |= FT_CURSOR;
			}
			else
			{
				term->dcursor->next->flag |= FT_CURSOR;
				term->dcursor = term->dcursor->next;
			}
		}
		else if (key == ' ')
			term->dcursor->flag ^= FT_SELECTED;
		else if (key == KEY_ESCAPE)
			break ;
		else if (key == '\n')
			return (key);
	}
	return (0);
}

void	s_resize(int signo)
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
*/

void	s_ctrl_z(int signo)
{
	if (signo == SIGTSTP || signo == SIGSTOP)
	{
		load_saved_terminal(g_term);
		signal(SIGTSTP, SIG_DFL);
		ioctl(g_term->fd, TIOCSTI, "\x1A");
	}
}

void	s_fg(int signo)
{
	if (signo == SIGCONT)
	{
		if (get_terminal(g_term)
			|| load_new_terminal(g_term))
		{
			ft_dprintf(STDERR_FILENO, "ft_select: cannot reload terminal.\n");
			load_saved_terminal(g_term);
			ft_dlistdel(&(g_term->dlist));
			exit(1);
		}
		signal_test();
		display_arg(g_term);
	}
}

void	s_exit(int signo)
{
	if (signo == SIGINT || signo == SIGABRT
		|| signo == SIGQUIT)
	{
		load_saved_terminal(g_term);
		ft_dlistdel(&(g_term->dlist));
		exit(1);
	}
}


void	signal_test(void)
{
	signal(SIGWINCH, s_resize);
	signal(SIGTSTP, s_ctrl_z);
	signal(SIGCONT, s_fg);
	signal(SIGINT, s_exit);
	signal(SIGABRT, s_exit);
	signal(SIGSTOP, s_ctrl_z);
	signal(SIGQUIT, s_exit);
}


int					main(int ac, char **av)
{
	t_term	term;
	long	mem;
	int		ret;
	
	if (init_select(&term, ac, av) || get_terminal(&term))
		return (1);
	if (load_new_terminal(&term))
	{
		load_saved_terminal(&term);
		ft_dlistdel(&(term.dlist));
		return (1);
	}
	signal_test();
	mem = read_keypress(&term);
	load_saved_terminal(&term);
	term.dcursor = term.dlist;
	ret = 0;
	while (mem)
	{
		if (term.dcursor->flag & FT_SELECTED)
		{
			if (!ret)
			{	
				ft_dprintf(STDOUT_FILENO, "%s", term.dcursor->txt);
				ret = 1;
			}
			else
				ft_dprintf(STDOUT_FILENO, " %s", term.dcursor->txt);
		}
		term.dcursor = term.dcursor->next;
		if (term.dcursor->flag & FT_FIRST)
			break ;
	}
	if (ret)
		ft_dprintf(STDOUT_FILENO, "\n");
	ft_dlistdel(&(term.dlist));
	return (0);
}
