# IO-Tester
<a href="https://github.com/tocola/IO-TESTER"><img src="https://img.shields.io/badge/IO_Tester-v1.4-blue.svg"></a>

## What is IO-Tester

The goal of this software is to take files as parameter that contains lists of inputs and expected ouputs and to tell the user if the tests succeed, failed or crashed.

### Compatibility
Tested on `MacOS`, `Ubuntu`, `Fedora`, `ArchLinux`

## How to use it ?

1. You need to have ```VSCode``` installed
2. Clone this repository
3. Go to the ```IO_Tester``` folder
4. Execute the following command ```sudo make install``` (the binary will be installed at ***usr/local/bin***)
5. Now let's write a test !

## Usage

Test files must follow the following template :
```sh
[NameOfTest] CommandRunYourProgram
theOutputYouWant
[END]
```

Here we test a program that takes parameters and prints them in the standard output.  
In file ```test.io``` :

```sh
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

### Execute

```sh
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
### Details
![](.github/example1.png)  
![](.github/example2.png)  
### VSCode Diff
![](.github/example3.png)  

You can also run multiple test files at once :
```sh
IO_Tester test1.io test2.io test3.io --diff
```

## Annexe
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
