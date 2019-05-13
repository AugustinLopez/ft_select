/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_link_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/09 14:02:23 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/13 14:05:19 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

t_dlist		*ft_dlistnew(char *src, int flag, t_dlist *prev)
{
	t_dlist	*new;

	if (!(new = (t_dlist*)malloc(sizeof(*new))))
		return (NULL);
	new->txt = src;
	new->flag = flag;
	if (prev)
	{
		prev->next = new;
		new->prev = prev;
	}
	return (new);
}

t_dlist		*ft_dlistfree(t_dlist **elem)
{
	t_dlist	*tmp;
	int		x;

	tmp = (*elem)->next;
	x = ((*elem)->flag & FT_FIRST);
	if ((*elem)->prev)
		(*elem)->prev->next = (*elem)->next;
	if ((*elem)->next)
		(*elem)->next->prev = (*elem)->prev;
	free(*elem);
	*elem = 0;
	if (x)
		tmp->flag |= FT_FIRST;
	return (tmp);
}

void		ft_dlistdel(t_dlist **elem)
{
	t_dlist	*tmp;
	t_dlist	*last;

	tmp = *elem;
	if (*elem)
		last = (*elem)->prev;
	while (tmp)
	{
		*elem = (*elem)->next;
		if (*elem == tmp)
		{
			free(tmp);
			break ;
		}
		free(tmp);
		tmp = *elem;
		if (tmp == last)
		{
			free(tmp);
			break ;
		}
	}
}
