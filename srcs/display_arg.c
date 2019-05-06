/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_arg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/06 12:46:46 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int	col_per_row(t_term *term, int termcol)
{
	int	column;
	int	test;

	column = termcol;
	if (column < 0)
		return (-1);
	term->coloff = 1;
	column /= (term->colsize + term->coloff);
	if (term->ac >= column)
	{
		while (1)
		{
			test = termcol / (term->colsize + term->coloff);
			if (test != column || term->coloff > 5)
			{
				term->coloff--;
				break ;
			}
			term->coloff++;
		}
	}
	else if (term->ac < column)
	{
		while ((term->colsize + term->coloff) * term->ac < termcol)
		{
			if (term->coloff > 5)
				break ;
			term->coloff++;
		}
		column = term->ac;
	}
	if (column == 0)
	{
		term->coloff = 0;
		return (1);
	}
	return (column);
}

void	print_column(t_term *term, int col, int row)
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
				ft_dprintf(STDIN_FILENO, "%-*s",term->colsize + term->coloff, term->av[i++]);
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

/*
** have to use ioctl instead of tgetnum: it does not update)
*/

void	display_arg(t_term *term)
{
	int		col;
	int		row;
	struct winsize w;

	ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
	col = w.ws_col;
	row = w.ws_row;
	if (!(term->av) || col <= 0 || row <= 0 || term->colsize > col)
		return ;
	tputs(tgetstr("cl", NULL), 1, putchar_in);
	ft_dprintf(STDIN_FILENO, "%dx%d\n", col, row);
	if ((col = col_per_row(term, col)) <= 0)
		return ;
	row = term->ac / col;
	if (term->ac % col)
		row++;
	print_column(term, col, row);
}
