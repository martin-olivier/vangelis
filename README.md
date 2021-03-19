# IO-Tester
<a href="https://github.com/tocola/IO-TESTER/releases/tag/v1.6.1"><img src="https://img.shields.io/badge/IO_Tester-v1.6.1-blue.svg"></a>

## What is IO-Tester ?

The goal of this software is to take files as parameter that contains lists of inputs and expected ouputs and to tell the user if the tests succeed, failed or crashed.

## Compatibility
Tested on `MacOS`, `Ubuntu`, `Fedora`, `ArchLinux`

## Installation

1. Clone this repository
2. Go to the ```IO-TESTER``` folder
3. Execute the following command :  
   ```sudo make install``` (the binary will be installed at ***usr/local/bin***)

## Usage

Test files must follow this patern :
```
[Test Name] command
expected output
[END]
```

Here we want to test a program that takes parameters and prints them in the standard output.  
Let's create a test file :

```
# test.io

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
```

Then execute `IO_Tester` with the file containing the tests as argument.

You can add `--details` as final argument to display the real and the expected output in the shell when a test fails.
![](.github/example_details.png)   
You can add `--diff` as final argument to display the diff between the real and the expected output in VS Code when a test fails.
![](.github/example_diff.png)  

## Manual

```
USAGE:
        IO_Tester test.io [OPTIONS]

DESCRIPTION:
        test.io         file that contains functional tests

OPTIONS:
        -h --help       Display help menu
        -v --version    Display actual version
        -c --changelog  Display the changelog
        -u --update     Update this software (sudo)
        --details       Display details of all tests
        --diff          Display difference in VSCode

RETURN VALUE:
        0               if all tests succeed
        1               if one or more tests failed or crashed
        84              if IO_Tester failed to load the test file
```

You can also run multiple test files at once :
```sh
IO_Tester test1.io test2.io test3.io --diff
```

## Build Tips
If you want to uninstall IO-Tester :  
```make uninstall```


If you just want to get the binary without installing :  
```make```


If you want to remove all objects files :  
```make clean```


If you want to remove all objects files and the binary :  
```make fclean```



## Contributors

 - [Benjamin Reigner](https://github.com/Breigner01)
 - [Alexandre Chetrit](https://github.com/chetrit)
 - [Coline Seguret](https://github.com/Cleopha)
