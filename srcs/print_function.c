/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_function.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 18:42:20 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/24 11:42:56 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_select.h>

/*
** PRINT_HELP: print help and stop the program when -h is provided.
*/

int					print_help(void)
{
	ft_putstr(FT_UNDER "usage" FT_EOC ": " FT_BOLD "./ft_select " FT_EOC);
	ft_putstr("[" FT_BOLD "-" SELECT_OPTION FT_EOC "] [" FT_UNDER "arg1");
	ft_putendl(FT_EOC " " FT_UNDER "arg2" FT_EOC " " FT_UNDER "..." FT_EOC "]");
	ft_putendl("\n\tThe following options are available:");
	ft_putstr("\n\t(" FT_ITALIC "P-Up/P-Down" FT_EOC);
	ft_putendl(")\tChange final print order");
	ft_putendl("\t(" FT_ITALIC "F2" FT_EOC ")\t\tSelect/Deselect all");
	ft_putstr(FT_BOLD "\t-G" FT_EOC " (" FT_ITALIC "F3" FT_EOC);
	ft_putendl(")\t\tColor On/Off");
	ft_putstr(FT_BOLD "\t-p" FT_EOC " (" FT_ITALIC "F4" FT_EOC);
	ft_putendl(")\t\tPretty display On/Off");
	ft_putstr(FT_BOLD "\t-c" FT_EOC " (" FT_ITALIC "F5" FT_EOC);
	ft_putendl(")\t\tCircular column On/Off");
	ft_putstr(FT_BOLD "\t-m" FT_EOC " (" FT_ITALIC "F6" FT_EOC);
	ft_putendl(")\t\tNon-circular movement On/Off");
	ft_putstr(FT_BOLD "\t-C" FT_EOC " (" FT_ITALIC "F7" FT_EOC);
	ft_putendl(")\t\tTerminal Cursor On/Off");
	ft_putendl(FT_BOLD "\t-h" FT_EOC "\t\tHelp");
	ft_putendl(FT_BOLD "\t-t" FT_EOC "\t\tUse /dev/tty");
	ft_putendl("\n\tIf your arguments contains '-', use the following format:");
	ft_putstr("\t" FT_UNDER "usage" FT_EOC ": " FT_BOLD "./ft_select " FT_EOC);
	ft_putstr("[" FT_BOLD "-" SELECT_OPTION FT_EOC "] -- [" FT_UNDER "arg1");
	ft_putendl(FT_EOC " " FT_UNDER "arg2" FT_EOC " " FT_UNDER "..." FT_EOC "]");
	return (END_USAGE);
}

static inline void	print_color(t_term *term, t_dlist *tmp)
{
	struct stat data;

	if (lstat(tmp->txt, &data) >= 0)
	{
		if (S_ISDIR(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_CYAN);
		else if (S_ISLNK(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_PURPLE);
		else if (S_ISBLK(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_BLUE);
		else if (S_ISSOCK(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_LBLUE);
		else if (S_ISCHR(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_YELLOW);
		else if (S_ISFIFO(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_LYELLOW);
		else if (S_ISREG(data.st_mode) && (S_IXUSR & data.st_mode))
			ft_dprintf(term->fd, "%s", FT_RED);
		else if (S_ISREG(data.st_mode))
			ft_dprintf(term->fd, "%s", FT_GRAY);
	}
	else
		ft_dprintf(term->fd, "%s", FT_BOLD);
}

static inline void	print_pretty(t_term *term, t_dlist *tmp, int offset)
{
	ft_dprintf(term->fd, "[");
	if (tmp->flag & FT_CURSOR && tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s%s+%s", FT_GREEN, FT_UNDER, FT_EOC);
	else if (tmp->flag & FT_CURSOR)
		ft_dprintf(term->fd, "%s|%s", FT_UNDER, FT_EOC);
	else if (tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s-%s", FT_GREEN, FT_EOC);
	else
		ft_dprintf(term->fd, " ");
	ft_dprintf(term->fd, "] ");
	if (term->flag & SELECT_GG)
		print_color(term, tmp);
	ft_dprintf(term->fd, "%s%s%*c", tmp->txt, FT_EOC,
			term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
}

static inline void	print_basic(t_term *term, t_dlist *tmp, int offset)
{
	if (term->flag & SELECT_GG)
		print_color(term, tmp);
	if (tmp && tmp->flag & FT_CURSOR && tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s%s%s%s%-*c", FT_REV, FT_UNDER, tmp->txt,
				FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, '*');
	else if (tmp && tmp->flag & FT_CURSOR)
		ft_dprintf(term->fd, "%s%s%s%*c", FT_UNDER, tmp->txt,
				FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
	else if (tmp && tmp->flag & FT_SELECTED)
		ft_dprintf(term->fd, "%s%s%s%*c", FT_REV, tmp->txt,
				FT_EOC, term->maxlen - ft_strlen(tmp->txt) + offset, ' ');
	else if (tmp)
		ft_dprintf(term->fd, "%-*s%s", term->maxlen + offset, tmp->txt, FT_EOC);
}

void				print_column(t_term *term, int col, int row, int offset)
{
	int		c;
	int		r;
	t_dlist	*tmp;

	r = 0;
	tmp = term->dlist;
	while (r++ < row)
	{
		c = 0;
		while (c++ < col)
		{
			if (tmp && term->flag & SELECT_P)
				print_pretty(term, tmp, offset);
			else
				print_basic(term, tmp, offset);
			if (!tmp || (tmp->next->flag & FT_FIRST))
				break ;
			tmp = tmp->next;
		}
		ft_dprintf(term->fd, "\n");
		if (!tmp || tmp->flag & FT_FIRST)
			break ;
	}
}
