/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arrow_circuit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 16:41:43 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/14 16:42:18 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

void					arrow_left_cir(t_term *term)
{
	term->dcursor->flag &= ~FT_CURSOR;
	term->dcursor->prev->flag |= FT_CURSOR;
	term->dcursor = term->dcursor->prev;
}

void					arrow_right_cir(t_term *term)
{
	term->dcursor->flag &= ~FT_CURSOR;
	term->dcursor->next->flag |= FT_CURSOR;
	term->dcursor = term->dcursor->next;
}

/*
** INIT_UPDOWN_CIR: simplify case up and down with COL == 1 || ROW == 1
*/

static inline t_dlist	*init_updown_cir(t_term *term, long key)
{
	if (term->col == 1 || term->row == 1)
	{
		if (key == KEY_UP)
			arrow_left_cir(term);
		else
			arrow_right_cir(term);
		return (0);
	}
	term->dcursor->flag &= ~FT_CURSOR;
	return (term->dcursor);
}

/*
** ARROW_UP/DOWN_CIR: handle the up/down key for circular movement.
** I used non-circular up and down movement because they were already made.
** Making the calculation properly would probably increase performance. Indeed.
*/

void					arrow_up_cir(t_term *term)
{
	int		i;
	int		j;
	t_dlist	*tmp;

	if (!(tmp = init_updown_cir(term, KEY_UP)))
		return ;
	i = term->col;
	j = 0;
	while (i--)
	{
		if (tmp->flag & FT_FIRST)
			break ;
		j++;
		tmp = tmp->prev;
	}
	if (tmp->flag & FT_FIRST && j != term->col)
	{
		arrow_left_mat(term);
		arrow_up_mat(term);
	}
	else
	{
		term->dcursor = tmp;
		tmp->flag |= FT_CURSOR;
	}
}

void					arrow_down_cir(t_term *term)
{
	int		i;
	int		j;
	t_dlist	*tmp;

	if (!(tmp = init_updown_cir(term, KEY_DOWN)))
		return ;
	i = term->col;
	j = 0;
	while (i--)
	{
		if (tmp->flag & FT_FIRST && j)
			break ;
		j++;
		tmp = tmp->next;
	}
	if (tmp->flag & FT_FIRST)
	{
		arrow_down_mat(term);
		arrow_right_mat(term);
	}
	else
	{
		term->dcursor = tmp;
		tmp->flag |= FT_CURSOR;
	}
}
