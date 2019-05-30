Some still reachable leaks might happens due to tputs, tgetent, tgetstr.
It is "normal" according to the curse FAQ.
Signal may not work properly with valgrind.

## TEST 1-EASY: NO ARG
Check behavior with no arg.

- ``` ./ft_select ```
- ``` `./ft_select` ```
- ``` valgrind ./ft_select ```
Expect reasonable behavior.

## TEST 2-EASY: ONE ARG
Check behavior with one arg.

End with Escape.
- ``` ./ft_select a ```
- ``` `./ft_select a` ```
- ``` valgrind ./ft_select a ```
Expect No bug.

## TEST 3-EASY: SEVERAL ARGS
Check behavior with a number of arg that fit in the window.

End with Escape.
- ``` ./ft_select * ```
- ``` `./ft_select *` ```
- ``` valgrind ./ft_select * ```
Expect no bug.

## TEST 4-EASY: LOTS OF ARGS
Check behavior with a number of arg that does not fit in the window.

End with Escape.
- ``` ./ft_select / /* /*/* ```
- ``` `./ft_select / /* /*/*` ```
- ``` valgrind ./ft_select / /* /*/* ```
Expect reasonable behavior.

## TEST 5-MEDIUM: SINGLE NULL ARG
Check behavior with a single NULL arg.

End with Escape if necessary. Test movement, space, etc... if not returned immediately.
- ``` ./ft_select '' ```
- ``` `./ft_select ''` ```
- ``` valgrind ./ft_select '' ```
Expect reasonable behavior.

## TEST 6-MEDIUM: VALID AND NULL ARGS
Check behavior with both valid and NULL args.

End with Escape if necessary. Test movement, space, etc... if empty arg are shown.
- ``` ./ft_select '' ```
- ``` ./ft_select a '' ```
- ``` ./ft_select '' a ```
- ``` ./ft_select a '' b ```
- ``` ./ft_select '' a '' ```
-``` `./ft_select ''` ```
- ``` `./ft_select '' a` ```
- ``` `./ft_select a '' b` ```
- ``` `./ft_select '' a ''` ```
-``` valgrind ./ft_select '' ```
- ``` valgrind ./ft_select a '' ```
- ``` valgrind ./ft_select '' a ```
- ``` valgrind ./ft_select a '' b ```
- ``` valgrind ./ft_select '' a '' ```
Expect reasonable behavior.

## TEST 7-MEDIUM: SPACE ARGS
Check behavior with argument containing space.

End with Escape if necessary.
- ``` ./ft_select a ' b' ' c ' 'd e' 'f ' ```
- ``` `./ft_select a ' b' ' c ' 'd e' 'f '` ```
- ``` valgrind ./ft_select a ' b' ' c ' 'd e' 'f '```
Expect reasonable behavior.

## TEST 8-EASY: CIRCULAR ARROW WITH ARGS
Check behavior of arrow keys with a number of arg that fit in the window.

Test each arrow keys. End with Escape.
- ``` ./ft_select * ```
- ``` `./ft_select *` ```
- ``` valgrind ./ft_select * ```
Expect no bug.

## TEST 9-EASY: CIRCULAR ARROW WITH ONE ARG
Check the behavior of the arrow keys in a on argument scenario.

Test each arrow keys. End with Escape.
- ``` ./ft_select a ```
- ``` `./ft_select a` ```
- ``` valgrind ./ft_select a ```
Expect no bug.

## TEST 10-EASY: CIRCULAR ARROW WITH LOT OF ARGS
Check behavior of arrow keys with a number of arg that does not fit in the window.

Test each arrow keys. End with Escape.
- ``` ./ft_select / /* ```
- ``` `./ft_select / /*` ```
- ``` valgrind ./ft_select / /* ```
Expect reasonable behavior.

## TEST 11-EASY: SPACE WITH ARGS
Check behavior of space key with a number of arg that fit in the window.

Held space key on several arg. Escape.
- ``` ./ft_select * */*```
- ``` `./ft_select * */*` ```
-``` valgrind ./ft_select * */*```
Expect no bug.

## TEST 12-EASY: ENTER WITH NO ARG SELECTED
Check behavior of Enter key with no argument selected.

Press enter.
-``` ./ft_select *```
-``` `./ft_select *` ```
-``` valgrind ./ft_select *```
Expect no bug.

## TEST 13-EASY: ENTER WITH ARG SELECTED
Check behavior of Enter key with one/several/all arguments selected.

Select one/several/all arguments. Press enter.
- ```./ft_select *```
-``` ls `./ft_select *` ```
- ``` valgrind ./ft_select *```
Expect no bug.

## TEST 14-MEDIUM: RESIZE
./ft_select * */*
Zoom, dezoom, reduce size, increase size. Escape.

## TEST 15-EASY: BACKSPACE
./ft_select *
Held backspace.

## TEST 16-EASY: DELETE
./ft_select a
Held delete.

## TEST 17-EASY: SPACE AND DELETE
./ft_select *
Select all. Held backspace.

./ft_select *
Select three. Delete one. Enter.

./ft_select *
Select three. Backspace them. Enter.

## TEST 18-EASY: SIGINT
./ft_select *
Ctrl+C

./ft_select *
Select one. Ctrl+C

## TEST 19-MEDIUM: KILL SIGNAL
./ft_select *
Get PID with top. kill PID
(HUP, INT, QUIT, ABRT, TERM, KILL, SEGV)
HUP, INT, QUIT, ABRT, TERM should be handled.
KILL is not interceptable but should not destroy the terminal.
Other signal should not destroy the terminal either.

## TEST 20-MEDIUM: SIGTSTP
./ft_select *
Select two with space.
Ctrl+Z
fg ./ft_select (if backquote, use kill -CONT)
Enter

./ft_select *
Select two with space.
Ctrl+Z
fg ./ft_select
Held backspace.


./ft_select *
Select two with space.
Get PID with top. kill -SIGTSTP PID
fg ./ft_select
Enter.

## TEST 21-MEDIUM: SIGSTOP
./ft_select *
Select two with space
get PID with top. kill -SIGTSTOP PID
fg ./ft_select (if backquote, use kill -CONT)
Expect reasonable behavior: SIGSTOP is not interceptable.

## TEST 22-HARD: TTY OUTPUT
./ft_select *
Select two with space.
Get PID with top. kill -SIGTSTP PID
kill -SIGCONT PID
jobs (you might see (tty output))
fg ./ft_select
Check if Ctrl+Z is still working.

./ft_select * &
jobs (you might see (tty output))
fg ./ft_select
Check if Ctrl+Z is still working.

## TEST 23-EASY: EMPTY ENV
env -i ./ft_select *

env -i "TERM=" ./ft_select *

bash
./ft_select *
Ctrl+Z
TERM=
fg ./ft_select
Escape
exit

## TEST 24-HARD: REDIRECTION
./ft_select * < /dev/null
