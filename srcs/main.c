/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:27:05 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/20 12:48:36 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

static inline int	select_available_option(char *av, int *flag)
{
	int	i;

	while (*(++av))
	{
		if (!(i = ft_strchri("mpcGhC", av[0])))
			return (-1);
		*flag |= (1 << (i - 1));
	}
	return (0);
}

int					select_option(int ac, char **av, int *flag)
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


int					print_help(void)
{
	ft_putstr(FT_UNDER "usage" FT_EOC ": " FT_BOLD "./ft_select " FT_EOC);
	ft_putstr("[" FT_BOLD "-mpcGh" FT_EOC "] [" FT_UNDER "arg1" FT_EOC " ");
	ft_putendl(FT_UNDER "arg2" FT_EOC " " FT_UNDER "..." FT_EOC "]");
	ft_putendl("\n\tThe following options are available:");
	ft_putendl("\n\t(" FT_ITALIC "F2" FT_EOC ")\tSelect/Deselect all");
	ft_putstr(FT_BOLD "\t-G" FT_EOC " (" FT_ITALIC "F3" FT_EOC);
	ft_putendl(")\tColor On/Off");
	ft_putstr(FT_BOLD "\t-p" FT_EOC " (" FT_ITALIC "F4" FT_EOC);
	ft_putendl(")\tPretty display On/Off");
	ft_putstr(FT_BOLD "\t-c" FT_EOC " (" FT_ITALIC "F5" FT_EOC);
	ft_putendl(")\tCircular column On/Off");
	ft_putstr(FT_BOLD "\t-m" FT_EOC " (" FT_ITALIC "F6" FT_EOC);
	ft_putendl(")\tNon-circular movement On/Off");
	ft_putstr(FT_BOLD "\t-C" FT_EOC " (" FT_ITALIC "F7" FT_EOC);
	ft_putendl(")\tTerminal Cursor On/Off");
	ft_putendl(FT_BOLD "\t-h" FT_EOC "\tHelp");
	ft_putendl("\n\tIf your arguments contains '-', use the following format:");
	ft_putstr("\t" FT_UNDER "usage" FT_EOC ": " FT_BOLD "./ft_select " FT_EOC);
	ft_putstr("[" FT_BOLD "-mpcGh" FT_EOC "] -- [" FT_UNDER "arg1" FT_EOC " ");
	ft_putendl(FT_UNDER "arg2" FT_EOC " " FT_UNDER "..." FT_EOC "]");
	return (ERR_USAGE);
}

/*
** The global variable is a pointer to our structure. Used for signal handling.
** return different error if usage ? should return 0 in the end
*/

int					init_select(t_term *term, int ac, char **av)
{
	int	ret;

	if (ac < 2 || !(ret = select_option(ac, av, &(term->flag))))
		return (errmsg(ERR_USAGE));
	if (term->flag & SELECT_H)
		return (print_help());
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
	if ((ret = feed_dlist(term, av + ret - 1)))
		return (errmsg(ret));
	if ((ret = get_terminal(term)))
		return (ret);
	g_term = term;
	return (0);
}

void				finish_select(t_term *term, int key)
{
	int	ret;

	term->dcursor = term->dlist;
	ret = 0;
	while (key)
	{
		if (term->dcursor->flag & FT_SELECTED)
		{
			if (!ret)
			{
				ft_dprintf(STDOUT_FILENO, "%s", term->dcursor->txt);
				ret = 1;
			}
			else
				ft_dprintf(STDOUT_FILENO, " %s", term->dcursor->txt);
		}
		term->dcursor = term->dcursor->next;
		if (term->dcursor->flag & FT_FIRST)
			break ;
	}
	ft_dlistdel(&(term->dlist));
}

int					main(int ac, char **av)
{
	t_term	term;
	long	key;
	int		ret;

	if ((ret = init_select(&term, ac, av)))
		return (ret == ERR_USAGE ? 0 : ret);
	if ((ret = load_new_terminal(&term)))
	{
		load_saved_terminal(&term);
		ft_dlistdel(&(term.dlist));
		return (ret);
	}
	if (!(term.flag & SELECT_CC))
	   tputs(tgetstr("vi", NULL), 1, putchar_in);
	signal_setup();
	key = read_keypress(&term);
	load_saved_terminal(&term);
	finish_select(&term, key);
	return (0);
}
