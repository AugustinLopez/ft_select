/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_arg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/06 17:29:57 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int	loop_sup(t_term *term, int termcol, int *column, int offset)
{
	int tmp;

	while (1)
	{
		tmp = termcol / (term->maxlen + offset);
		if (tmp != *column || offset > 5)
			return (offset - 1);
		offset += 1;
	}
}

int	loop_inf(t_term *term, int termcol, int *column, int offset)
{
	int tmp;

	*column = term->ac;
	while (1)
	{
		tmp = (term->maxlen + offset) * term->ac;
		if (tmp >= termcol || offset > 5)
			return (offset - 1);
		offset += 1;
	}
}

int	col_per_row(t_term *term, int termcol, int *offset)
{
	int		column;

	if (!(column = termcol / (term->maxlen + *offset)))
	{
		*offset = 0;
		return (1);
	}
	if (term->ac >= column)
		*offset = loop_sup(term, termcol, &column, *offset);
	else
		*offset = loop_inf(term, termcol, &column, *offset);
	return (column);
}

void	print_column(t_term *term, int col, int row, int offset)
{
	int		c;
	int		r;
	int		i;

	r = 0;
	c = 0;
	i = 0;
	while (r < row)
	{
		while (c < col)
		{
			if (term->av[i])
				ft_dprintf(STDIN_FILENO, "%-*s",term->maxlen + offset, term->av[i++]);
			else
				break ;
			c++;
		}
		ft_dprintf(STDIN_FILENO, "\n");
		if (!term->av[i])
			break ;
		c = 0;
		r++;
	}
}

int		get_winsize(int *col, int *row)
{
	struct winsize	w;

	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &w) < 0)
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

/*
** have to use ioctl instead of tgetnum: it does not update)
*/

void	display_arg(t_term *term)
{
	int				col;
	int				row;
	int				offset;

	col = 0;
	row = 0;
	offset = 1;
	if (tputs(tgetstr("cl", NULL), 1, putchar_in))
	{
		ft_dprintf(STDERR_FILENO, "ft_select: could not clear the screen.\n");
		return ;
	}
	if (!(term->av) || get_winsize(&col, &row) || term->maxlen > col)
		return ;
	col = col_per_row(term, col, &offset);
	row = term->ac / col;
	if (term->ac % col)
		row++;
	term->col = col;
	term->row = row;
	print_column(term, col, row, offset);
}
