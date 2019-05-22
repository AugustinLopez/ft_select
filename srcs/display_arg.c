/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_arg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/21 17:30:46 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** COL_PER_ROW: Calculate how many arguments can be displayed per row.
** 2 mains case: all arguments can be displayed on 1 row (loop_inf)
** or not (loop_sup)
** We also calculate an offset value to create some spacing when possible
*/

int	loop_sup(t_term *term, int termcol, int *column, int offset)
{
	int tmp;
	int	base_spacing;

	base_spacing = term->maxlen + PRETTY_SPACING * (term->flag & SELECT_P);
	while (1)
	{
		tmp = termcol / (base_spacing + offset);
		if (tmp != *column || offset > MAX_SPACING)
			return (offset - 1);
		offset += 1;
	}
}

int	loop_inf(t_term *term, int termcol, int *column, int offset)
{
	int tmp;
	int	base_spacing;

	base_spacing = term->maxlen + PRETTY_SPACING * (term->flag & SELECT_P);
	*column = term->ac;
	while (1)
	{
		tmp = (base_spacing + offset) * term->ac;
		if (tmp >= termcol || offset > MAX_SPACING)
			return (offset - 1);
		offset += 1;
	}
}

int	col_per_row(t_term *term, int termcol, int *offset)
{
	int		column;
	int		tmp;

	tmp = term->maxlen + PRETTY_SPACING * (term->flag & SELECT_P) + *offset;
	if (!(column = termcol / tmp) || term->flag & SELECT_C)
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

void	print_color(t_term *term, t_dlist *tmp)
{
	struct stat data;

	if (lstat(tmp->txt, &data) >= 0)
	{
		if (S_ISDIR(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_CYAN);
		else if (S_ISLNK(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_PURPLE);
		else if (S_ISBLK(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_BLUE);
		else if (S_ISSOCK(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_LBLUE);
		else if (S_ISCHR(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_YELLOW);
		else if (S_ISFIFO(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_LYELLOW);
		else if (S_ISREG(data.st_mode) && (S_IXUSR & data.st_mode))
			ft_dprintf(term->fd, "%s", FT_RED);
		else if (S_ISREG(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_GRAY);
	}
	else
		ft_dprintf(term->fd, "%s", FT_BOLD);
}

void	print_pretty(t_term *term, t_dlist *tmp, int offset)
{
	ft_dprintf(term->fd, "[");
	if (tmp->flag & FT_CURSOR && tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s%s+%s", FT_GREEN, FT_UNDER, FT_EOC);
	else if (tmp->flag & FT_CURSOR)
		ft_dprintf(term->fd, "%s|%s", FT_UNDER, FT_EOC);
	else if (tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s-%s", FT_GREEN, FT_EOC);
	else
		ft_dprintf(term->fd, " ");
	ft_dprintf(term->fd, "] ");
	if (term->flag & SELECT_GG)
		print_color(term, tmp);
	ft_dprintf(term->fd, "%s%s%*c", tmp->txt, FT_EOC,
			term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
}

void	print_basic(t_term *term, t_dlist *tmp, int offset)
{
	if (term->flag & SELECT_GG)
		print_color(term, tmp);
	if (tmp && tmp->flag & FT_CURSOR && tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s%s%s%s%-*c", FT_REV, FT_UNDER, tmp->txt,
				FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, '*');
	else if (tmp && tmp->flag & FT_CURSOR)
		ft_dprintf(term->fd, "%s%s%s%*c", FT_UNDER, tmp->txt,
				FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
	else if (tmp && tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s%s%s%*c", FT_REV, tmp->txt,
				FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
	else if (tmp)
		ft_dprintf(term->fd, "%-*s%s", term->maxlen + offset, tmp->txt, FT_EOC);
}

void	print_column(t_term *term, int col, int row, int offset)
{
	int		c;
	int		r;
	t_dlist	*tmp;

	r = 0;
	tmp = term->dlist;
	while (r++ < row)
	{
		c = 0;
		while (c++ < col)
		{
			if (tmp && term->flag & SELECT_P)
				print_pretty(term, tmp, offset);
			else
				print_basic(term, tmp, offset);
			if (!tmp || (tmp->next->flag & FT_FIRST))
				break ;
			tmp = tmp->next;
		}
		ft_dprintf(term->fd, "\n");
		if (!tmp || tmp->flag & FT_FIRST)
			break ;
	}
}

int		get_winsize(t_term *term, int *col, int *row)
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

/*
** have to use ioctl instead of tgetnum: it does not update)
*/

void	place_term_cursor(t_term *term, int colterm, int rowterm, int offset)
{
	int		x;
	int		y;
	t_dlist	*tmp;

	if (term->row > rowterm || term->maxlen
			+ (PRETTY_SPACING * ((term->flag & SELECT_P) != 0)) >= colterm)
	{
		tputs(tgoto(tgetstr("cm", NULL), colterm, rowterm), 1, putchar_in);
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
	tputs(tgoto(tgetstr("cm", NULL), x, y), 1, putchar_in);
}

int		display_arg(t_term *term)
{
	int				col;
	int				colterm;
	int				rowterm;
	int				row;
	int				offset;

	col = 0;
	row = 0;
	offset = 1;
	if ((col = tputs(tgetstr("cl", NULL), 1, putchar_in)))
		ft_dprintf(STDERR_FILENO, "ft_select: could not clear the screen.\n");
	if (col || !(term->av) || get_winsize(term, &colterm, &rowterm))
		return (-1);
	row = rowterm;
	col = col_per_row(term, colterm, &offset);
	row = term->ac / col;
	if (term->ac % col)
		row++;
	term->col = col;
	term->row = row;
	print_column(term, col, row, offset);
	if (term->flag & SELECT_CC)
		place_term_cursor(term, colterm, rowterm, offset);
	return (0);
}
