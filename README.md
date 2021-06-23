# IO Tester
[![Version](https://img.shields.io/badge/IO_Tester-v1.8.0-blue.svg)](https://github.com/tocola/IO-TESTER/releases/tag/v1.8.0)
[![MIT license](https://img.shields.io/badge/License-MIT-orange.svg)](https://github.com/tocola/IO-TESTER/blob/main/LICENSE)
[![CPP Version](https://img.shields.io/badge/C++-17-darkgreen.svg)](https://isocpp.org/)

[![GitHub watchers](https://img.shields.io/github/watchers/tocola/IO-TESTER?style=social)](https://github.com/tocola/IO-TESTER/watchers/)
[![GitHub forks](https://img.shields.io/github/forks/tocola/IO-TESTER?style=social)](https://github.com/tocola/IO-TESTER/network/members/)
[![GitHub stars](https://img.shields.io/github/stars/tocola/IO-TESTER?style=social)](https://github.com/tocola/IO-TESTER/stargazers/)

The goal of this software is to take files as parameter that contains lists of inputs and expected outputs and to tell the user if the tests succeed, failed or crashed.

## Compatibility
Tested on `MacOS`, `Ubuntu`, `Fedora`, `ArchLinux`

## Installation

1. Clone this repository
2. Go to the `IO-TESTER` folder
3. Execute the following command :  
   `sudo make install` (the binary will be installed at ***usr/local/bin***)

`Don't forget to leave a star 🌟`

## Manual

```
USAGE:
        IO_Tester test.io [OPTIONS]

DESCRIPTION:
        test.io         file that contains functional tests

OPTIONS:
        -h --help       Display this help menu
        -v --version    Display the actual version
        -c --changelog  Display the changelog
        -u --update     Update this software (sudo)
        --details       Display the output difference in the shell
        --diff          Display the output difference in Visual Studio Code

RETURN VALUE:
        0               if all tests succeed
        1               if one or more tests failed or crashed
        84              if IO_Tester failed to load a test file
```

## Usage

Test files must follow this pattern :
```
[Test Name] command
expected output
[END]
```

Here we want to test a program that takes parameters and prints them in the standard output. We will also test some system commands  
Let's create a test file :

```
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
```

Then execute `IO_Tester` with the file containing the tests as argument.

You can add `--details` to display the difference between the real, and the expected output in the shell when a test fails.
![example_details](https://github.com/tocola/IO-TESTER/blob/main/.github/example_details.png?raw=true)   
You can add `--diff` to display the difference between the real, and the expected output in Visual Studio Code when a test fails.
![example_diff](https://github.com/tocola/IO-TESTER/blob/main/.github/example_diff.png?raw=true)   

You can also run multiple test files at once :
```
IO_Tester test1.io test2.io test3.io --diff
```

## Test Parameters
Parameters:  
You can change the value of a parameter for the next test:
```
@stdout <bool> (default: true)
@stderr <bool> (default: true)
@return <int> (default: 0)
@timeout <float> (default: 3.0)
```
You can also change the default value of a parameter :
```
@default parameter value
```

## Build Tips
If you want to uninstall IO Tester :  
`make uninstall`  

If you just want to get the binary without installing :  
`make`  

If you want to remove all objects files :  
`make clean`  

If you want to remove all objects files and the binary :  
`make fclean`  

## Contributors

 - [Martin Olivier](https://github.com/tocola)
 - [Benjamin Reigner](https://github.com/Breigner01)
 - [Alexandre Chetrit](https://github.com/chetrit)
 - [Coline Seguret](https://github.com/Cleopha)
