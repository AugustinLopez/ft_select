/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:23:47 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/13 15:42:30 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Note :
** may need apt-get install libncurses5-dev to have access to termcap.h
*/

#ifndef FT_SELECT_H
# define FT_SELECT_H

/*
** --- Includes ----------------------------------------------------------------
*/

/*
** unistd: read
** stdlib: malloc, free
*/

# include <libft.h>
# include <unistd.h>
# include <stdlib.h>
# include <termios.h>
# include <termcap.h>
# include <sys/ioctl.h>
# include <signal.h>
# include <errno.h>

/*
** --- Define ------------------------------------------------------------------
*/

/*
** KEY_LEFT binary : 01000100 - 01011011 - 00011011
**                   D        - [        - Esc, 27, ^[
**                 : ^[[D
*/

# define FT_FIRST 1
# define FT_LINE 2
# define FT_CURSOR 4
# define FT_SELECTED 8

# define KEY_LEFT 4479771L
# define KEY_UP 4283163L
# define KEY_DOWN 4348699L
# define KEY_RIGHT 4414235L
# define KEY_ESCAPE 27L
# define KEY_SPACE 32L
# define CTLR_AT 0L
# define CTRL_A 1L

/*
** --- Structure & Global ------------------------------------------------------
*/

/*
** term.ac / term.av: reproduce those taken from main.
** term.fd = fd of /dev/tty
** term.current / term.savec: to modify and restore terminal.
** term.name: name of active terminal.
** term.maxlen: len of the longest argument to be printed, column size.
**
** a t_term is created in the main, the global address g_term is then set.
** We use the global address when a signal is raised.
*/

typedef struct			s_dlist
{
	struct s_dlist		*next;
	struct s_dlist		*prev;
	char				*txt;
	int					flag;
}						t_dlist;

typedef struct			s_term
{
	int					ac;
	char				**av;
	int					fd;
	t_list				*list_av;
	t_dlist				*dlist;
	t_dlist				*dcursor;
	struct termios		saved;
	struct termios		current;
	char				*name;
	int					maxlen;
	int					col;
	int					row;
	int					flag;
}						t_term;

t_term	*g_term;


/*
** ---- Prototype --------------------------------------------------------------
*/

char					*get_terminal(t_term *term);
int						init_select(t_term *term, int ac, char **av);
int						load_new_terminal(t_term *term);
int						load_saved_terminal(t_term *term);
int						putchar_in(int c);
void					display_arg(t_term *term);
void					signal_test(void);
t_dlist					*ft_dlistnew(char *src, int flag, t_dlist *prev);
t_dlist					*ft_dlistfree(t_dlist **elem);
void					ft_dlistdel(t_dlist **elem);
#endif
