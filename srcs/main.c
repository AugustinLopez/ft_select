/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/23 17:45:18 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** SELECT_OPTION: set flag for shell option. Works like `ls` flags.
*/

static inline int	select_available_option(char *av, int *flag)
{
	int	i;

	while (*(++av))
	{
		if (!(i = ft_strchri(SELECT_OPTION, av[0])))
			return (-1);
		*flag |= (1 << (i - 1));
	}
	return (0);
}

static inline int	select_option(int ac, char **av, int *flag)
{
	int	i;

	*flag = 0;
	i = 0;
	while (++i < ac && av[i][0] == '-' && av[i][1])
	{
		if (*flag & SELECT_H)
			return (1);
		if (av[i][1] == '-' && !av[i][2])
		{
			if (!(av[i + 1]))
				return (0);
			return (i + 1);
		}
		if (select_available_option(av[i], flag))
			return (0);
	}
	return (i);
}

/*
** FINISH_SELECT:
** Restore terminal session. We attempt to proceed even in case of error
** 1st if+ loop: Print selected argument if last keystroke was '\n'
** delete circular list.
** check if -t flag has been used and close the fd associated with /dev/tty.
** if an error has been detected, we return an error and print a message.
** We print error immediately because we won't override previous error.
*/

static inline int	finish_select(t_term *term, int key)
{
	int	ret;
	int	err;

	ret = 0;
	term->dcursor = term->dlist;
	err = load_saved_terminal(term);
	if (key == '\n')
	{
		while (1)
		{
			if (term->dcursor->flag & FT_SELECTED)
				(!ret && (ret = 1)) ?
					ft_dprintf(STDOUT_FILENO, "%s", term->dcursor->txt) :
					ft_dprintf(STDOUT_FILENO, " %s", term->dcursor->txt);
			term->dcursor = term->dcursor->next;
			if (term->dcursor->flag & FT_FIRST)
				break ;
		}
	}
	ft_dlistdel(&(term->dlist));
	if (term->flag & SELECT_T)
		close(term->fd);
	return (errmsg(err));
}

/*
** INIT_SELECT:
** 1st if: print usage if bad or not enough arguments
** 2nd if: print help if -h
** Initialize variables, see structure in header.
** 3rd if: feed a circular list with av if possible
** 4th if: get terminal environment variable.
** set global variable for signal.
** set signal
*/

static inline int	init_select(t_term *term, int ac, char **av)
{
	int	ret;

	if ((ac < 2
			|| !(ret = select_option(ac, av, &(term->flag)))) && print_usage())
		return (END_USAGE);
	if ((term->flag & SELECT_H) && print_help())
		return (END_USAGE);
	term->name = 0;
	term->selected = 0;
	term->ac = ac - ret;
	term->av = av + ret;
	term->maxlen = 1;
	term->dlist = 0;
	term->dcursor = 0;
	term->up = (term->flag & SELECT_M) ? arrow_up_mat : arrow_up_cir;
	term->down = (term->flag & SELECT_M) ? arrow_down_mat : arrow_down_cir;
	term->left = (term->flag & SELECT_M) ? arrow_left_mat : arrow_left_cir;
	term->right = (term->flag & SELECT_M) ? arrow_right_mat : arrow_right_cir;
	ret = feed_dlist(term, av + ret - 1);
	if (ret || (ret = get_terminal(term)))
		return (ret);
	g_term = term;
	signal_setup(ACTIVATE);
	return (0);
}

int					main(int ac, char **av)
{
	t_term	term;
	int		ret;

	if ((ret = init_select(&term, ac, av)))
	{
		if (ret == END_USAGE)
			return (0);
		return (errmsg(ret));
	}
	if ((ret = load_new_terminal(&term)))
	{
		(void)finish_select(&term, 0);
		return (errmsg(ret));
	}
	if ((ret = keypress(&term)) && ret != '\n' && ret != KEY_ESCAPE)
	{
		(void)finish_select(&term, 0);
		return (errmsg(ret));
	}
	return (finish_select(&term, ret));
}
