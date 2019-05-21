/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:23:47 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/21 16:32:03 by aulopez          ###   ########.fr       */
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
# include <sys/stat.h>
# include <signal.h>
# include <errno.h>
# include <fcntl.h>

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
# define FT_DELETED 16
# define FT_ERROR 32

# define KEY_LEFT 4479771L
# define KEY_UP 4283163L
# define KEY_DOWN 4348699L
# define KEY_RIGHT 4414235L
# define KEY_ESCAPE 27L
# define KEY_BACKSPACE 127L
# define KEY_DELETE 2117294875L
# define KEY_F2 5328667L
# define KEY_F3 5394203L
# define KEY_F4 5459739L
# define KEY_F5 542058306331L
# define KEY_F6 542091860763L
# define KEY_F7 542108637979L

# define CTLR_AT 0L
# define CTRL_A 1L

# define ERR_USAGE 1
# define ERR_MEM 2
# define ERR_EMPTYARG 3
# define ERR_TERMENV 4
# define ERR_BADFDTTY 5
# define ERR_BADSTDIN 6
# define ERR_NOTERMINFO 7
# define ERR_TCGET 8
# define ERR_TCSET 9
# define ERR_TPUTS 10
# define ERR_KEYREAD 11

# define SELECT_M 1
# define SELECT_P 2
# define SELECT_C 4
# define SELECT_G 8
# define SELECT_H 16
# define SELECT_CC 32
# define SELECT_T 64
# define SELECT_RESIZE 128
# define SELECT_CTRLZ 256
# define SELECT_KILL 512


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
	int					selected;
	int					col;
	int					row;
	int					flag;
	void				(*up)(struct s_term *);
	void				(*down)(struct s_term *);
	void				(*left)(struct s_term *);
	void				(*right)(struct s_term *);
	int					(*putchar)(int);
}						t_term;

t_term	*g_term;


/*
** ---- Prototype --------------------------------------------------------------
*/

int						key_signal(t_term *term);
void					signal_setup(void);

int						get_terminal(t_term *term);
long					read_keypress(t_term *term);
int						init_select(t_term *term, int ac, char **av);
int						load_new_terminal(t_term *term);
int						load_saved_terminal(t_term *term);
int						singleton_fd(int c);
int						putchar_in(int c);
int						putchar_fd(int c);
void					display_arg(t_term *term);
void					signal_test(void);
t_dlist					*ft_dlistnew(char *src, int flag, t_dlist *prev);
t_dlist					*ft_dlistfree(t_dlist **elem);
void					ft_dlistdel(t_dlist **elem);
int						feed_dlist(t_term *term, char **av);
int						errmsg(int error);

void					arrow_up_cir(t_term *term);
void					arrow_down_cir(t_term *term);
void					arrow_left_cir(t_term *term);
void					arrow_right_cir(t_term *term);
void					arrow_up_mat(t_term *term);
void					arrow_down_mat(t_term *term);
void					arrow_left_mat(t_term *term);
void					arrow_right_mat(t_term *term);
#endif
