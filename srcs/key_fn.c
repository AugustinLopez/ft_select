/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_fn.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 16:40:25 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 14:17:31 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

static inline void	clear_all(t_term *term)
{
	t_dlist	*tmp;

	tmp = term->dlist;
	term->selected = 0;
	while (1)
	{
		tmp->flag &= ~FT_SELECTED;
		tmp = tmp->next;
		if (tmp->flag & FT_FIRST)
			break ;
	}
}

static inline void	fill_all(t_term *term)
{
	t_dlist	*tmp;

	tmp = term->dlist;
	while (1)
	{
		if (!(tmp->flag & FT_SELECTED))
		{
			term->selected++;
			tmp->flag |= FT_SELECTED;
		}
		tmp = tmp->next;
		if (tmp->flag & FT_FIRST)
			break ;
	}
}

static inline void	key_farrow(t_term *term)
{
	term->flag ^= SELECT_M;
	term->up = (term->flag & SELECT_M) ? arrow_up_mat : arrow_up_cir;
	term->down = (term->flag & SELECT_M) ? arrow_down_mat : arrow_down_cir;
	term->left = (term->flag & SELECT_M) ? arrow_left_mat : arrow_left_cir;
	term->right = (term->flag & SELECT_M) ? arrow_right_mat : arrow_right_cir;
}

int					key_fn(t_term *term, long key)
{
	if (key == KEY_F2)
		term->selected == term->ac ? clear_all(term) : fill_all(term);
	else if (key == KEY_F3)
		term->flag ^= SELECT_GG;
	else if (key == KEY_F4)
		term->flag ^= SELECT_P;
	else if (key == KEY_F5)
		term->flag ^= SELECT_C;
	else if (key == KEY_F6)
		key_farrow(term);
	else if (key == KEY_F7)
	{
		term->flag ^= SELECT_CC;
		if (term->flag & SELECT_CC)
			tputs(tgetstr("ve", NULL), 1, term->putchar);
		else
			tputs(tgetstr("vi", NULL), 1, term->putchar);
	}
	else
		return (0);
	return (1);
}
