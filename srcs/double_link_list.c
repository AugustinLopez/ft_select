/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   double_link_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/09 14:02:23 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/22 11:51:28 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** FT_DLISTNEW: create a link for a double-linked list.
** The link receives a flag and, if it exists, the last elem of the list
** it should be appended to.
*/

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

/*
** FT_DLISTDEL: delete the circular list and its content.
** Should also works if the list is not circular.
** DEBUG:
** 1st if: set up a stop condition for circular list.
** Beware: the content of the list is assumed to have been malloc'd.
*/

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
** RETURN_DLIST: join both end of the circular list.
** Function required because of the 25 lines max rule.
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
** FEED_DLIST: create a circular list of argument from **av.
** The size of the longest argument is tracked here.
** DEBUG CASE:
** 1st if: Empty element are completely ignored
** 2nd if: set the first av as the cursor target and first element top left.
** else normal element.
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
	term->mem = term->dlist;
	return (return_dlist(term));
}
