/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/14 13:33:12 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int					setup_reader(t_term *term, long *key)
{
	t_dlist	*tmp;
	int		i;
	int		ret;

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
	free(term->dcursor);
	term->dcursor = tmp;
	term->ac--;
	return (0);
}

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
		term->dcursor->prev->flag |= FT_CURSOR;
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

long				read_keypress(t_term *term)
{
	long	key;
	t_dlist	*tmp;
	int		i;

	while (1)
	{
		if (setup_reader(term, &key))
			return (-1);
		if (key == ' ')
			term->dcursor->flag ^= FT_SELECTED;
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
			arrow_left_mat(term);
		else if (key == KEY_UP)
			arrow_up_mat(term);
		else if (key == KEY_DOWN)
			arrow_down_mat(term);
		else if (key == KEY_RIGHT)
			arrow_right_mat(term);
		else if (key == KEY_F1)
		{
			i = 0;
			tmp = term->dlist;
			while (1)
			{
				if (tmp->flag & FT_SELECTED)
					i++;
				else
					tmp->flag |= FT_SELECTED;
				tmp = tmp->next;
				if (tmp->flag & FT_FIRST)
					break ;
			}
			if (i == term->ac)
			{
				tmp = term->dlist;
				while (1)
				{
				tmp->flag &= ~FT_SELECTED;
				tmp = tmp->next;
				if (tmp->flag & FT_FIRST)
					break ;
				}
			}
		}
	}
	return (0);
}
