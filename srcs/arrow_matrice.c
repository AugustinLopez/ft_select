/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arrow_matrice.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 16:40:52 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/15 11:50:42 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

void				arrow_left_mat(t_term *term)
{
	int	i;

	term->dcursor->flag &= ~FT_CURSOR;
	if (term->dcursor->flag & FT_LINE)
	{
		i = 1;
		while (i++ < term->col && !(term->dcursor->next->flag & FT_LINE))
			term->dcursor = term->dcursor->next;
		term->dcursor->flag |= FT_CURSOR;
	}
	else
	{
		term->dcursor->prev->flag |= FT_CURSOR;
		term->dcursor = term->dcursor->prev;
	}
}

void				arrow_right_mat(t_term *term)
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

static inline int	init_arrow_updown(t_term *term, long key)
{
	if (term->row == 1)
		return (1);
	if (term->col == 1)
	{
		term->dcursor->flag &= ~FT_CURSOR;
		if (key == KEY_UP)
			term->dcursor->prev->flag |= FT_CURSOR;
		else
			term->dcursor->next->flag |= FT_CURSOR;
		term->dcursor = key == KEY_UP ?
			term->dcursor->prev : term->dcursor->next;
		return (1);
	}
	return (0);
}

void				arrow_up_mat(t_term *term)
{
	int		i;
	int		j;
	t_dlist	*tmp;

	if (init_arrow_updown(term, KEY_UP))
		return ;
	term->dcursor->flag &= ~FT_CURSOR;
	tmp = term->dcursor;
	i = term->col;
	j = 0;
	while (i--)
	{
		j++;
		if (tmp->flag & FT_FIRST)
			i = (term->ac % term->col >= j) ?
				term->ac % term->col - j : i + term->ac % term->col;
		tmp = tmp->prev;
	}
	term->dcursor = tmp;
	tmp->flag |= FT_CURSOR;
}

void				arrow_down_mat(t_term *term)
{
	int		i;
	int		j;
	t_dlist	*tmp;

	if (init_arrow_updown(term, KEY_DOWN))
		return ;
	term->dcursor->flag &= ~FT_CURSOR;
	tmp = term->dcursor;
	i = term->col;
	j = 0;
	while (i--)
	{
		j++;
		if (tmp->next->flag & FT_FIRST)
			i = (term->ac % term->col >= j) ?
				term->ac % term->col - j : i + term->ac % term->col;
		tmp = tmp->next;
	}
	term->dcursor = tmp;
	tmp->flag |= FT_CURSOR;
}
