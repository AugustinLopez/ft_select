/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_link_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/09 14:02:23 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/14 14:47:53 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

t_dlist				*ft_dlistnew(char *src, int flag, t_dlist *prev)
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

/*t_dlist				*ft_dlistfree(t_dlist **elem)
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
}*/

void				ft_dlistdel(t_dlist **elem)
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

/*
** RETURN_DLIST: join both end of the circular list + final return value.
** Should be put at the end of FEED_DLIST when norm is no longer an issue.
** DEBUG:
** 1st if: case where av is only composed of empty arg.
*/

static inline int	return_dlist(t_term *term)
{
	if (!(term->dlist))
		return (ERR_EMPTYARG);
	term->dlist->next = term->dcursor;
	term->dcursor->prev = term->dlist;
	term->dlist = term->dcursor;
	return (0);
}

/*
** FEED_DLIST: feed the circular list of argument. Call by init_select
** The double-link structure will be easier to handle in the long run.
** DEBUG CASE:
** 1st if: Empty element are completely ignored
** 3rd if: return on malloc error.
*/

int					feed_dlist(t_term *term, char **av)
{
	int	i;
	int	len;

	len = 0;
	i = 0;
	while (av[++i])
	{
		if (!av[i][0] && (term->ac)--)
			continue ;
		if (!term->dcursor)
		{
			term->dcursor = ft_dlistnew(av[i], FT_CURSOR | FT_FIRST, 0);
			term->dlist = term->dcursor;
		}
		else
			term->dlist = ft_dlistnew(av[i], 0, term->dlist);
		if (!term->dlist)
			return (ERR_MEM);
		len = ft_strlen(av[i]);
		term->maxlen = len > term->maxlen ? len : term->maxlen;
	}
	return (return_dlist(term));
}

