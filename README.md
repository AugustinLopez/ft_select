# ft_select
The **ft_select** program is an optional exercice from the *Unix* branch ([42](https://www.42.fr) curriculum).
The goal is to allow an user to easily move around a printed list of argument with the arrow keys. The terminal display is manipulated with the **termcap** library. The original terminal settings is reset to its default value when the program is over.

## Useful links
- The [ft_select PDF subject](https://github.com/AugustinLopez/ft_select/blob/master/rcs/ft_select.pdf)
- The [GNU Termcap Manual](https://www.gnu.org/software/termutils/manual/termcap-1.3/html_mono/termcap.html)
- [Understanding read() behavior with VMIN and VTIME](http://www.unixwiz.net/techtips/termios-vmin-vtime.html)

### Credit:
I always try to find other implementation on Github. You may find similarities between their code and mine:
- [An implementation that goes beyond the subject and an awesome README](https://github.com/R4meau/ft_select)

## Download & Launch

```
git clone https://github.com/AugustinLopez/ft_select.git
cd ft_select
make
```

### Usage:
```
./ft_select [-CGhmpc] [--] [arg1 arg2 ...]
```
The following command is possible but not advised if an argument contains whitespace characters.
```
ls `./ft_select *`
```

### Help
```
./ft_select -h
```

### Options
-h: Help
-G: Color On
-p: Prettier Display On
-c: Column + cursor at bottom On (For huge list of argument)
-m: Non-Circular Movement On
-C: Terminal Cursor Show

### keys
- Arrow keys: move around the list. The default movement is **circular**: one arrow can move through the whole list.
- Space key: Select/Deselect an argument.
- Enter key: End the program and print each selected argument separated by one space without carriage return.
- Escape key: End the program immediately.
- Backspace/delete keys: Remove the cursor argument. If no argument left, end the program immediately.
- F2 key: Select/deselect all.
- F3 key: Color On/Off
- F4 key: Prettier display On/Off
- F5 key: Column + cursor at bottom On/Off
- F6 key: Non-Circular Movement On/Off
- F7 key: Terminal Cursor Show/Hide
- Keys such as 'a', 'b', 'c' can be used to look for an argument starting with the corresponding letter. Case sensitive.
