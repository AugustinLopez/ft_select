/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/02 13:23:47 by aulopez           #+#    #+#             */
/*   Updated: 2019/05/02 13:49:53 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Note : may need to apt-get install libncurses5-dev to get termcap.h
#ifndef FT_SELECT_H
# define FT_SELECT_H

#include <libft.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <termcap.h>
#include <limits.h>

#define KEY_LEFT 4479771
#define KEY_UP 4283163
#define KEY_DOWN 4348699
#define KEY_RIGHT 4414235
#define KEY_ESCAPE 27

// term.name is not malloced

typedef struct			s_term
{
	struct termios		saved;
	struct termios		current;
	char				*name;
}						t_term;

char					*get_terminal(void);
int						load_new_terminal(t_term *term);
int						load_saved_terminal(t_term *term);
int						putchar_in(int c);
#endif
