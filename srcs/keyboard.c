/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 16:40:25 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/20 13:02:33 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

//Need original_first for option_c

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
	return (ret == -1 && errmsg(ERR_KEYREAD));
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
	else if (key == KEY_F6)
	{
		term->flag ^= SELECT_M;
		term->up = (term->flag & SELECT_M) ? arrow_up_mat : arrow_up_cir;
		term->down = (term->flag & SELECT_M) ? arrow_down_mat : arrow_down_cir;
		term->left = (term->flag & SELECT_M) ? arrow_left_mat : arrow_left_cir;
		term->right = (term->flag & SELECT_M) ?
			arrow_right_mat : arrow_right_cir;
	}
	else
		return (0);
	return (1);
}

int					key_specialtouch(t_term *term, long key)
{
	if (key == ' ')
	{
		term->dcursor->flag ^= FT_SELECTED;
		term->selected += term->dcursor->flag & FT_SELECTED ? 1 : -1;
	}
	else if (key == KEY_ESCAPE)
		return (2);
	else if (key == '\n')
		return (key);
	else if (key == KEY_BACKSPACE || key == KEY_DELETE)
	{
		if (delete_arg(term))
			return (2);
	}
	else if (key == KEY_F2)
		term->selected == term->ac ? clear_all(term) : fill_all(term);
	else if (key == KEY_F3)
		term->flag ^= SELECT_G;
	else if (key == KEY_F4)
		term->flag ^= SELECT_P;
	else if (key == KEY_F5)
		term->flag ^= SELECT_C;
	else
		return (0);
	return (1);
}

void				key_basictouch(t_term *term, long key)
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

long				read_keypress(t_term *term)
{
	long	key;
	int		ret;

	while (1)
	{
		key = 0;
		if (setup_reader(term, &key))
			return (-1);
		if ((ret = key_signal(term)))
		{
			if (ret == 1)
				continue ;
			return (0);
		}
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
		else if (key == KEY_F3)
			term->flag ^= SELECT_G;
		else if (key == KEY_F4)
			term->flag ^= SELECT_P;
		else if (key == KEY_F5)
			term->flag ^= SELECT_C;
		else if (key == KEY_F7)
		{
			term->flag ^= SELECT_CC;
			if (term->flag & SELECT_CC)
				tputs(tgetstr("ve", NULL), 1, putchar_in);
			else
				tputs(tgetstr("vi", NULL), 1, putchar_in);
		}
		else if (key == KEY_F6)
		{
			term->flag ^= SELECT_M;
			term->up = (term->flag & SELECT_M) ? arrow_up_mat : arrow_up_cir;
			term->down = (term->flag & SELECT_M) ? arrow_down_mat : arrow_down_cir;
			term->left = (term->flag & SELECT_M) ? arrow_left_mat : arrow_left_cir;
			term->right = (term->flag & SELECT_M) ? arrow_right_mat : arrow_right_cir;
		}
		if (key_arrow(term, key))
			continue ;
		if ((ret = key_specialtouch(term, key)))
		{
			if (ret == 1)
				continue ;
			return (ret);
		}
		key_basictouch(term, key);
	}
	return (0);
}
