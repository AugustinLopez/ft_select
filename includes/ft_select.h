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


// term.name is not malloced

typedef struct		s_term
{
	struct termios	saved;
	struct termios	current;
	char			*name;
}					t_term;
#endif
