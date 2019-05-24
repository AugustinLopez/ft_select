/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:23:47 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/24 17:44:49 by aulopez          ###   ########.fr       */
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
** fcntl: open
** sys/stat: lstat (color bonus)
*/

# include <libft.h>
# include <unistd.h>
# include <stdlib.h>
# include <termios.h>
# include <termcap.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <signal.h>
# include <fcntl.h>

/*
** --- Define ------------------------------------------------------------------
*/

/*
** KEY_LEFT binary : 01000100 - 01011011 - 00011011
**                   D        - [        - Esc, 27, ^[
**                 : ^[[D
*/

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
# define KEY_PUP 2117425947L
# define KEY_PDOWN 2117491483L

/*
** Flags for each element of the list
*/

# define FT_FIRST 1
# define FT_LINE 2
# define FT_CURSOR 4
# define FT_SELECTED 8

/*
** Value for spacing and some True/False
*/

# define MAX_SPACING 5
# define PRETTY_SPACING 4
# define ACTIVATE 1
# define DESACTIVATE 0

/*
** Error num and their associated message
*/

# define END_USAGE 1
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
# define ERR_RELOAD 12

# define MSG_USAGE "usage: ./ft_select [-chmptCG] [--] [arg1 arg2 ...]\n"
# define MSG_MEM "ft_select: not enough memory.\n"
# define MSG_EMPTYARG "ft_select: argument list is empty\n"
# define MSG_TERMENV "ft_select: could not retrieve the name of the terminal\n"
# define MSG_BADFDTTY "ft_select: could not get access to the active terminal\n"
# define MSG_BADSTDIN "ft_select: STDIN is not a TTY. Use the -t option\n"
# define MSG_NOTERMINFO "ft_select: could not retrieve terminal information\n"
# define MSG_TTY "ft_select: could not set terminal status\n"
# define MSG_KEYREAD "ft_select: cannot capture keystroke\n"
# define MSG_RELOAD "ft_select: cannot properly reload terminal\n"

/*
** Program flag: option and signal
*/

# define SELECT_OPTION "chmptCG"
# define SELECT_C 1
# define SELECT_H 2
# define SELECT_M 4
# define SELECT_P 8
# define SELECT_T 16
# define SELECT_CC 32
# define SELECT_GG 64
# define SELECT_RESIZE 128
# define SELECT_CTRLZ 256
# define SELECT_KILL 512

/*
** --- Structure & Global ------------------------------------------------------
*/

/*
** Txt will be a pointer to (*av)[x], do not free !
*/

typedef struct			s_dlist
{
	struct s_dlist		*next;
	struct s_dlist		*prev;
	char				*txt;
	int					flag;
}						t_dlist;

/*
** Because of -t option we have to keep track of our fd.
** Because of -c option we have to keep track of our very first elem with mem
** We keep in memory the size of the biggest av. We do not recalculate it.
** We keep track of the number of av selected.
** Because of -m option we may have different arrow mode. We use fun_ptr
** Because of -t we also need a fun_ptr for a system putchar.
** (I could use the singleton for that but i won't use the singleton for that).
** We use ac to easily keep track of live argument
*/

typedef struct			s_term
{
	int					ac;
	char				**av;
	int					fd;
	t_dlist				*dlist;
	t_dlist				*mem;
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

/*
** Global variable only used with signal. We only use term->flag
** but a pointer is a pointer.
*/

t_term	*g_term;

/*
** ---- Prototype --------------------------------------------------------------
*/

void					print_column(t_term *term, int col, int row,
							int offset);
int						print_help(void);
int						print_main(t_term *term);
int						print_usage(void);

int						errmsg(int error);
int						singleton_fd(int c);
int						putchar_fd(int c);
int						putchar_in(int c);

int						term_winsize(t_term *term, int *col, int *row);
void					term_cursor(t_term *term, int colt, int row,
							int offset);

void					signal_setup(int option);

void					key_basic(t_term *term, long key);
int						key_special(t_term *term, long key);
int						key_fn(t_term *term, long key);
int						key_arrow(t_term *term, long key);
int						keypress(t_term *term);

int						get_terminal(t_term *term);
int						load_new_terminal(t_term *term);
int						load_saved_terminal(t_term *term);
int						reload_terminal(t_term *term);
void					ft_dlistdel(t_dlist **elem);
int						feed_dlist(t_term *term, char **av);

void					arrow_up_cir(t_term *term);
void					arrow_down_cir(t_term *term);
void					arrow_left_cir(t_term *term);
void					arrow_right_cir(t_term *term);
void					arrow_up_mat(t_term *term);
void					arrow_down_mat(t_term *term);
void					arrow_left_mat(t_term *term);
void					arrow_right_mat(t_term *term);
#endif
