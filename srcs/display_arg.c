/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_arg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/20 13:02:40 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int	loop_sup(t_term *term, int termcol, int *column, int offset)
{
	int tmp;

	while (1)
	{
		tmp = termcol / (term->maxlen + 4 * (term->flag & SELECT_P) + offset);
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
		tmp = (term->maxlen + 4 * (term->flag & SELECT_P) + offset) * term->ac;
		if (tmp >= termcol || offset > 5)
			return (offset - 1);
		offset += 1;
	}
}

int	col_per_row(t_term *term, int termcol, int *offset)
{
	int		column;
	int		tmp;

	tmp = term->maxlen + 4 * (term->flag & SELECT_P) + *offset;
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

void	print_column(t_term *term, int col, int row, int offset)
{
	int		c;
	int		r;
	int		x;
	t_dlist	*tmp;
	struct stat data;

	r = 0;
	c = 0;
	tmp = term->dlist;
	while (r < row)
	{
		while (c < col)
		{
			if (tmp && tmp->flag & FT_DELETED)
			{
				tmp = tmp->next;
				continue ;
			}
			x = 0;
			if (term->flag & SELECT_G && lstat(tmp->txt, &data) >= 0)
				x = 1;
			if (tmp && term->flag & SELECT_P)
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
				if (x)
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
				ft_dprintf(term->fd, "%s%s%*c", tmp->txt, FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
			}
			else
			{
				if (x)
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
				if (tmp && tmp->flag & FT_CURSOR && tmp->flag & FT_SELECTED)
					ft_dprintf(term->fd, "%s%s%s%s%-*c", FT_REV, FT_UNDER, tmp->txt,
						FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, '*');
				else if (tmp && tmp->flag & FT_CURSOR)
					ft_dprintf(term->fd, "%s%s%s%*c",FT_UNDER, tmp->txt,
						FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
				else if (tmp && tmp->flag & FT_SELECTED)
					ft_dprintf(term->fd, "%s%s%s%*c",FT_REV, tmp->txt,
						FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
				else if (tmp)
					ft_dprintf(term->fd, "%-*s%s",
						term->maxlen + offset, tmp->txt, FT_EOC);
			}
			if (!tmp || (tmp->next->flag & FT_FIRST))
				break ;
			tmp = tmp->next;
			c++;
		}
		ft_dprintf(term->fd, "\n");
		if (!tmp || tmp->flag & FT_FIRST)
			break ;
		c = 0;
		r++;
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
	int	x;
	int	y;
	t_dlist	*tmp;

	if (term->row > rowterm || term->maxlen > colterm)
	{
		tputs(tgoto(tgetstr("cm", NULL), colterm, rowterm), 1, putchar_in);
		return ;
	}
	x = ((term->flag & SELECT_P) != 0);
	y = 0;
	tmp = term->dlist;
	while (!(tmp->flag & FT_CURSOR))
	{
		x += (term->maxlen + offset) + 4 * ((term->flag & SELECT_P) != 0);
		if (x >= (term->col * (term->maxlen + offset + 4 * ((term->flag & SELECT_P) != 0))))
		{
			x = ((term->flag & SELECT_P) != 0);
			y++;
		}
		tmp = tmp->next;
	}
	tputs(tgoto(tgetstr("cm", NULL), x, y), 1, putchar_in);
}

void	display_arg(t_term *term)
{
	int				col;
	int				colterm;
	int				rowterm;
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
	if (!(term->av) || get_winsize(term, &colterm, &rowterm) /*|| term->maxlen > col*/)
		return ;
	col = col_per_row(term, colterm, &offset);
	row = term->ac / col;
	if (term->ac % col)
		row++;
	term->col = col;
	term->row = row;
	print_column(term, col, row, offset);
	if (term->flag & SELECT_CC)
	{
		place_term_cursor(term, colterm, rowterm, offset);
	}
}
