# test.io

# set the default timeout value to 1 sec
@default timeout 1

[Simple Print] python3 printer test
test

[END]

[Multiple Prints] python3 printer Hello World !
Hello
World
!

[END]

[Oooops] python3 printer I am bad
You
are
bad

[END]

[Power] whoami
root

[END]

# disable stdout and stderr catch for this test
@stdout false
@stderr false

[ZZZzzz] sleep 5
[END]