/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_function.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 16:40:25 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 14:18:15 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int					key_arrow(t_term *term, long key)
{
	if (key == KEY_LEFT)
		term->left(term);
	else if (key == KEY_UP)
		term->up(term);
	else if (key == KEY_DOWN)
		term->down(term);
	else if (key == KEY_RIGHT)
		term->right(term);
	else
		return (0);
	return (1);
}

static inline int	delete_arg(t_term *term)
{
	t_dlist *tmp;

	tmp = term->dcursor->next;
	if (term->dcursor == tmp)
		return (1);
	tmp->flag |= FT_CURSOR;
	tmp->prev = term->dcursor->prev;
	tmp->prev->next = tmp;
	if (term->dcursor->flag & FT_FIRST)
	{
		term->dlist = tmp;
		term->mem = tmp;
		tmp->flag |= FT_FIRST;
	}
	if (term->dcursor->flag & FT_SELECTED)
	{
		term->selected -= 1;
		term->dcursor->flag &= ~FT_SELECTED;
	}
	free(term->dcursor);
	term->dcursor = tmp;
	term->ac--;
	return (0);
}

int					key_special(t_term *term, long key)
{
	if (key == ' ')
	{
		term->dcursor->flag ^= FT_SELECTED;
		term->selected += term->dcursor->flag & FT_SELECTED ? 1 : -1;
		return (1);
	}
	else if (key == '\n')
		return (key);
	else if (key == KEY_ESCAPE)
		return (2);
	else if (key == KEY_BACKSPACE || key == KEY_DELETE)
	{
		if (delete_arg(term))
			return (2);
	}
	return (0);
}

void				key_basic(t_term *term, long key)
{
	long	mask;
	t_dlist	*tmp;
	char	s[9];
	int		i;

	tmp = term->dcursor->next;
	i = 0;
	mask = 255;
	while (key)
	{
		s[i++] = key & mask;
		key &= ~mask;
		mask = mask << 8;
	}
	s[i] = 0;
	while (!(tmp->flag & FT_CURSOR))
	{
		if (!ft_strlcmp(tmp->txt, s))
			break ;
		tmp = tmp->next;
	}
	term->dcursor->flag &= ~FT_CURSOR;
	term->dcursor = tmp;
	tmp->flag |= FT_CURSOR;
}
