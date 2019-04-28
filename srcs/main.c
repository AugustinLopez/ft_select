#include <libft.h>
#include <ft_select.h>
#include <unistd.h> //STDERR_FILENO
#include <stdlib.h> //malloc getenv
#include <termios.h>
#include <termcap.h>

inline static int	parse_arg(int ac, char **av)
{
	(void)av;
	if (ac < 2)
	{
		ft_dprintf(STDERR_FILENO, "usage: ./ft_select [argument ...]\n");
		return (0);
	}
	return (1);
}

int					putschar(int c)
{
	return (write(STDOUT_FILENO, &c, 1));
}

inline static char	*get_terminal(void)
{
	char	*s;

	if (!(s = getenv("TERM")))
	{
		ft_dprintf(STDERR_FILENO, "ft_select: Could not get terminal name.\n");
		return (NULL);
	}
	if (!isatty(STDOUT_FILENO))
	{
		ft_dprintf(STDERR_FILENO, "ft_select: unexpected fd for stdout.\n");
		free(s);
		return (NULL);
	}

}

int					main(int ac, char **av)
{
	int		flags;
	char	*s;

	if (!(flags = parse_arg(ac, av)))
		return (1);
	(void)flags;
	if (!(s = getenv("TERM")))
	{
		ft_dprintf(STDERR_FILENO, "Could not get terminal name.\n");
		return (0);
	}
	tputs(tgetstr("ve", NULL), 1, putschar);
	ft_putendl(s);
	return (0);
}
