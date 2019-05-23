/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 14:39:07 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** COL_PER_ROW: Calculate how many arguments can be displayed per row.
** 2 mains case: all arguments can be displayed on 1 row (loop_inf)
** or not (loop_sup)
** We also calculate an offset value to create some spacing when possible
*/

static inline int	loop_sup(t_term *term, int colterm, int *column, int offset)
{
	int tmp;
	int	base_spacing;

	base_spacing = term->maxlen + PRETTY_SPACING * (term->flag & SELECT_P);
	while (1)
	{
		tmp = colterm / (base_spacing + offset);
		if (tmp != *column || offset > MAX_SPACING)
			return (offset - 1);
		offset += 1;
	}
}

static inline int	loop_inf(t_term *term, int colterm, int *column, int offset)
{
	int tmp;
	int	base_spacing;

	base_spacing = term->maxlen + PRETTY_SPACING * (term->flag & SELECT_P);
	*column = term->ac;
	while (1)
	{
		tmp = (base_spacing + offset) * term->ac;
		if (tmp >= colterm || offset > MAX_SPACING)
			return (offset - 1);
		offset += 1;
	}
}

static inline void	calcul_col_per_row(t_term *term, int colterm, int *offset)
{
	int		column;
	int		tmp;

	tmp = term->maxlen + PRETTY_SPACING * (term->flag & SELECT_P) + *offset;
	if (!(column = colterm / tmp) || term->flag & SELECT_C)
	{
		*offset = 0;
		column = 1;
	}
	else if (term->ac >= column)
		*offset = loop_sup(term, colterm, &column, *offset);
	else
		*offset = loop_inf(term, colterm, &column, *offset);
	term->col = column == 0 ? 1 : column;
	term->row = term->ac / term->col;
	if (term->ac % term->col)
		(term->row)++;
}

int					print_main(t_term *term)
{
	int				colterm;
	int				rowterm;
	int				offset;

	offset = 1;
	term->col = 0;
	term->row = 0;
	if (tputs(tgetstr("cl", NULL), 1, term->putchar))
	{
		ft_dprintf(STDERR_FILENO, "ft_select: could not clear the screen.\n");
		return (-1);
	}
	if (!(term->av) || term_winsize(term, &colterm, &rowterm))
		return (-1);
	calcul_col_per_row(term, colterm, &offset);
	print_column(term, term->col, term->row, offset);
	if (term->flag & SELECT_CC)
		term_cursor(term, colterm, rowterm, offset);
	return (0);
}
