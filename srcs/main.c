/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/09 16:08:44 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

long				read_keypress(t_term *term)
{
	long	key;
	long	mem;
	int		ret;
	t_dlist	*tmp;
	t_dlist	*ctrl;
	int		sign;
	int		i;
	int		bef;
	int		aft;

	(void)bef;
	(void)aft;
	bef = 0;
	aft = 0;
	mem = 0;
	sign = 0;
	while (1)
	{
		key = 0;
		display_arg(term);
		ret = read(STDIN_FILENO, &key, sizeof(key));
		ret = 0;
		(void)ret;
		if (key == KEY_LEFT)
		{
			term->dcursor->flag &= ~ FT_CURSOR;
			term->dcursor->prev->flag |= FT_CURSOR;
			term->dcursor = term->dcursor->prev;
		}
		else if (key == KEY_UP)
		{
			if (term->row == 1)
				continue ;
			if (term->col == 1)
			{
				term->dcursor->flag &= ~ FT_CURSOR;
				term->dcursor->prev->flag |= FT_CURSOR;
				term->dcursor = term->dcursor->prev;
			}
			i = term->col;
			term->dcursor->flag &= ~FT_CURSOR;
			tmp = term->dcursor->prev;
			ctrl = term->dcursor->next;
			while (--i)
			{
				if (tmp->flag & FT_FIRST)
				{
					sign |= 1;
					aft = term->col - i;
				}
				if (ctrl->flag & FT_FIRST)
				{
					sign |= 2;
					bef = term->col - i;
				}
				ctrl = ctrl->next;
				tmp = tmp->prev;
			}
			//if (sign == 3)
			//	tmp = term->dcursor;
			//if (sign == 1)
			//	tmp = ctrl;
			term->dcursor = tmp;
			tmp->flag |= FT_CURSOR;
		}
		else if (key == KEY_DOWN)
		{
			if (term->row == 1)
				continue ;
			if (term->col == 1)
			{
				term->dcursor->flag &= ~ FT_CURSOR;
				term->dcursor->prev->flag |= FT_CURSOR;
				term->dcursor = term->dcursor->next;
			}
			i = term->col;
			term->dcursor->flag &= ~FT_CURSOR;
			tmp = term->dcursor;
			while (i--)
				tmp = tmp->next;
			term->dcursor = tmp;
			tmp->flag |= FT_CURSOR;
		}
		else if (key == KEY_RIGHT)
		{
			term->dcursor->flag &= ~ FT_CURSOR;
			term->dcursor->next->flag |= FT_CURSOR;
			term->dcursor = term->dcursor->next;
		}
		else if (key == KEY_ESCAPE)
		{
			return (mem);
		}
		else
		{
			mem = key;
			ft_dprintf(STDIN_FILENO, "%lc\n",key);
		}
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

void	s_ctrl_z(int signo)
{
	if (signo == SIGTSTP)
	{
		load_saved_terminal(g_term);
		signal(SIGTSTP, SIG_DFL);
		ioctl(STDIN_FILENO, TIOCSTI, "\x1A");
	}
}

void	free_list(t_term *term)
{
	t_list	*tmp;

	tmp = term->list_av;
	while (tmp)
	{
		term->list_av = tmp->next;
		free(tmp);
		tmp = term->list_av;
	}
}

void	s_fg(int signo)
{
	if (signo == SIGCONT)
	{
		if (!(g_term->name = get_terminal(g_term->name))
			|| load_new_terminal(g_term))
		{
			ft_dprintf(STDERR_FILENO, "ft_select: cannot reload terminal.\n");
			load_saved_terminal(g_term);
			free_list(g_term);
			exit(1);
		}
		signal_test();
		display_arg(g_term);
	}
}

void	signal_test(void)
{
	signal(SIGWINCH, s_resize);
	signal(SIGTSTP, s_ctrl_z);
	signal(SIGCONT, s_fg);
}


int					main(int ac, char **av)
{
	t_term	term;
	long	mem;

	term.name = 0;
	term.flag = 0;
	if (!init_select(&term, &ac, &av) || !(term.name = get_terminal(term.name)))
		return (1);
	if (load_new_terminal(&term))
	{
		load_saved_terminal(&term);
		free_list(&term);
		ft_dlistdel(&(term.dlist));
		return (1);
	}
	mem = 0;
	signal_test();
	mem = read_keypress(&term);
	load_saved_terminal(&term);
	free_list(&term);
	ft_dlistdel(&(term.dlist));
	ft_printf("%zu %lc\n",term.maxlen, mem);
	return (0);
}
