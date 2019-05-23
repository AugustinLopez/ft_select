/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   term_liveinfo.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 18:19:54 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int					term_winsize(t_term *term, int *col, int *row)
{
	struct winsize	w;

	if (ioctl(term->fd, TIOCGWINSZ, &w) < 0)
	{
		ft_dprintf(STDERR_FILENO, "ft_select: could not get terminal size.\n");
		return (-1);
	}
	*col = w.ws_col;
	*row = w.ws_row;
	if (*col <= 0 || *row <= 0)
	{
		ft_dprintf(STDERR_FILENO, "ft_select: invalid terminal size.\n");
		return (-1);
	}
	return (0);
}

void				term_cursor(t_term *term, int col, int row, int offset)
{
	int		x;
	int		y;
	t_dlist	*tmp;

	x = (term->flag & SELECT_P) != 0 ? PRETTY_SPACING : 0;
	if (term->row > row || term->maxlen + x >= col)
	{
		tputs(tgoto(tgetstr("cm", NULL), col, row), 1, term->putchar);
		return ;
	}
	x = 0;
	y = 0;
	tmp = term->dlist;
	while (!(tmp->flag & FT_CURSOR))
	{
		x += (term->maxlen + offset) + 4 * ((term->flag & SELECT_P) != 0);
		if (x >= (term->col
				* (term->maxlen + offset + 4 * ((term->flag & SELECT_P) != 0))))
		{
			x = 0;
			y++;
		}
		tmp = tmp->next;
	}
	tputs(tgoto(tgetstr("cm", NULL), x, y), 1, term->putchar);
}
