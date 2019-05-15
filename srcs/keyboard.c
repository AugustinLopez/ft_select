/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 16:40:25 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/14 18:12:06 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int					setup_reader(t_term *term, long *key)
{
	t_dlist	*tmp;
	int		i;
	int		ret;

	if (term->flag & SELECT_C)
	{
		term->dlist->flag &= ~FT_FIRST;
		term->dcursor->next->flag |= FT_FIRST;
		term->dlist = term->dcursor->next;
	}
	display_arg(term);
	*key = 0;
	ret = read(term->fd, key, sizeof(key));
	i = 0;
	tmp = term->dlist;
	while (1)
	{
		if (!(i++ % term->col) || (tmp->flag & FT_FIRST))
			tmp->flag |= FT_LINE;
		else
			tmp->flag &= ~FT_LINE;
		tmp = tmp->next;
		if (tmp->flag & FT_FIRST)
			break ;
	}
	if (ret == -1 && errmsg(ERR_KEYREAD))
		return (-1);
	return (0);
}

int					delete_arg(t_term *term)
{
	t_dlist *tmp;

	tmp = term->dcursor->next;
	tmp->flag |= FT_CURSOR;
	tmp->prev = term->dcursor->prev;
	tmp->prev->next = tmp;
	if (term->dcursor->flag & FT_FIRST)
	{
		term->dlist = tmp;
		tmp->flag |= FT_FIRST;
	}
	if (term->dcursor == tmp)
		return (1);
	term->dcursor->flag |= FT_DELETED;
	if (term->dcursor->flag & FT_SELECTED)
	{
		term->selected -= 1;
		term->dcursor->flag &= ~FT_SELECTED;
	}
	term->dcursor = tmp;
	term->ac--;
	return (0);
}

void				clear_all(t_term *term)
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

void				fill_all(t_term *term)
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

long				read_keypress(t_term *term)
{
	long	key;

	while (1)
	{
		if (setup_reader(term, &key))
			return (-1);
		if (key == ' ')
		{
			term->dcursor->flag ^= FT_SELECTED;
			term->selected += term->dcursor->flag & FT_SELECTED ? 1 : -1;
		}
		else if (key == KEY_ESCAPE)
			return (0);
		else if (key == '\n')
			return (key);
		else if (key == KEY_BACKSPACE || key == KEY_DELETE)
		{
			if (delete_arg(term))
				return (0);
		}
		else if (key == KEY_LEFT)
			term->left(term);
		else if (key == KEY_UP)
			term->up(term);
		else if (key == KEY_DOWN)
			term->down(term);
		else if (key == KEY_RIGHT)
			term->right(term);
		else if (key == KEY_F2)
			term->selected == term->ac ? clear_all(term) : fill_all(term);
	}
	return (0);
}
