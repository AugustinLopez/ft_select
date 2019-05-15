/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/15 11:12:00 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

int					main(int ac, char **av)
{
	t_term	term;
	long	mem;
	int		ret;
	
	if (init_select(&term, ac, av) || get_terminal(&term))
		return (1);
	if (load_new_terminal(&term))
	{
		load_saved_terminal(&term);
		ft_dlistdel(&(term.dlist));
		return (1);
	}
	signal_setup();
	mem = read_keypress(&term);
	load_saved_terminal(&term);
	term.dcursor = term.dlist;
	ret = 0;
	while (mem)
	{
		if (term.dcursor->flag & FT_SELECTED)
		{
			if (!ret)
			{	
				ft_dprintf(STDOUT_FILENO, "%s", term.dcursor->txt);
				ret = 1;
			}
			else
				ft_dprintf(STDOUT_FILENO, " %s", term.dcursor->txt);
		}
		term.dcursor = term.dcursor->next;
		if (term.dcursor->flag & FT_FIRST)
			break ;
	}
	if (ret && mem > 0)
		ft_dprintf(STDOUT_FILENO, "\n");
	ft_dlistdel(&(term.dlist));
	mem = mem > 0 ? 0 : mem;
	return (mem);
}
