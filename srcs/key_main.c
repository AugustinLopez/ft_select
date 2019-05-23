/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_main.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 16:40:25 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 14:22:01 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

static inline void	flag_column(t_term *term)
{
	if (term->flag & SELECT_C)
	{
		term->dlist->flag &= ~FT_FIRST;
		term->dcursor->next->flag |= FT_FIRST;
		term->dlist = term->dcursor->next;
	}
	else if (term->dlist != term->mem)
	{
		term->dlist->flag &= ~FT_FIRST;
		term->mem->flag |= FT_FIRST;
		term->dlist = term->mem;
	}
}

static inline int	setup_reader(t_term *term, long *key)
{
	t_dlist	*tmp;
	int		i;
	int		ret;

	flag_column(term);
	print_main(term);
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

/*
** raise(SIGTSTP) would be better instead of ioctl.
*/

int					key_signal(t_term *term)
{
	if (term->flag & SELECT_CTRLZ)
	{
		load_saved_terminal(term);
		signal_setup(0);
		ioctl(term->fd, TIOCSTI, "\x1A");
		term->flag &= ~SELECT_CTRLZ;
		signal_setup(1);
		if (get_terminal(term) || load_new_terminal(term))
		{
			ft_dprintf(STDERR_FILENO, "ft_select: cannot reload terminal.\n");
			return (-1);
		}
		if (!(term->flag & SELECT_CC))
			tputs(tgetstr("vi", NULL), 1, term->putchar);
	}
	else if (term->flag & SELECT_RESIZE)
		term->flag &= ~SELECT_RESIZE;
	else
		return (0);
	return (1);
}

int					keypress(t_term *term)
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
		if (!key || key_arrow(term, key) || key_fn(term, key))
			continue ;
		if ((ret = key_special(term, key)))
		{
			if (ret == 1)
				continue ;
			return (ret);
		}
		key_basic(term, key);
	}
	return (0);
}
