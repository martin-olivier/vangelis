# Vangelis
[![Version](https://img.shields.io/badge/Vangelis-v2.0.0-blue.svg)](https://github.com/martin-olivier/Vangelis/releases/tag/v2.0.0)
[![MIT license](https://img.shields.io/badge/License-MIT-darkgreen.svg)](https://github.com/martin-olivier/Vangelis/blob/main/LICENSE)
[![Language](https://img.shields.io/badge/Language-Rust-orange.svg)](https://www.rust-lang.org/)

[![GitHub watchers](https://img.shields.io/github/watchers/martin-olivier/Vangelis?style=social)](https://github.com/martin-olivier/Vangelis/watchers/)
[![GitHub forks](https://img.shields.io/github/forks/martin-olivier/Vangelis?style=social)](https://github.com/martin-olivier/Vangelis/network/members/)
[![GitHub stars](https://img.shields.io/github/stars/martin-olivier/Vangelis?style=social)](https://github.com/martin-olivier/Vangelis/stargazers/)

[![workflow](https://github.com/martin-olivier/Vangelis/actions/workflows/CI.yml/badge.svg)](https://github.com/martin-olivier/Vangelis/actions/workflows/CI.yml)

This software takes test files as parameter that contains lists of commands to be run and to tell the user if the tests succeed, failed or crashed.

`⭐ Don't forget to put a star if you like the project!`

## ⚠️ Requirements
This software works on `Linux`, `MacOS`, and `Windows` 

To build and install the project, you will need [cargo](https://www.rust-lang.org/tools/install)

## 💻 Installation

```
git clone https://github.com/martin-olivier/Vangelis
cargo install --path Vangelis
```

## 📒 Manual

```
USAGE:
        vangelis [TESTFILES] [OPTIONS]

TESTFILES:
        path to one or multiple test files

OPTIONS:
        --help                  Display this help menu
        --version               Display the current version
        --changelog             Display the changelog
        --verbose               Display the output difference in the shell
        --diff                  Display the output difference in Visual Studio Code
        --stop_on_failure       Stop the execution if a test didn't succeed

RETURN VALUE:
        0                       if all tests succeed
        1                       if one or more tests failed or crashed
        84                      if a critical error occured
```

## 🗂 Test File

Test files must follow the following pattern in [toml](https://toml.io/en/)
```
Default {
    runs_on: Option<Array<String>>,
    unix_shell: Option<String>,
    windows_shell: Option<String>,
    exit_status: Option<Integer>,
    timeout: Option<Float>,
    working_dir: Option<String>,
}

Test {
    runs_on: Option<Array<String>>,
    unix_shell: Option<String>,
    windows_shell: Option<String>,
    exit_status: Option<Integer>,
    timeout: Option<Float>,
    working_dir: Option<String>,
    cmd: String,
    stdin: Option<String>,
    stdout: Option<String>,
    stderr: Option<String>,
}

TestFile {
    default: Option<Default>,
    test: Map<String, Test>,
}
```

## 📝 Example

```toml
# testfile.toml

[default]

windows_shell = "powershell"
timeout = 5

[test.echo]

cmd = "echo hello"
stdout = "hello\n"

[test.power]

runs_on = ["linux", "macos"]

cmd = "whoami"
stdout = "root\n"

[test.sleep]

timeout = 5

cmd = "sleep 10"
stdout = ""
stderr = ""

[test."print args"]

cmd = "python3 print_args.py hey brother"

stdout = """
hey
brother
"""
stderr = ""

[test."print stdin"]

cmd = "python3 print_stdin.py"

stdin = """
hey
brother
"""
stdout = """
hey

brother

"""
stderr = ""
```

```
vangelis examples/testfile.toml
```

## ⚙️ Parameters

```toml
[runs_on]

# An array of strings that defines on witch os the test is allowed to run

# Default value
runs_on = ["linux", "windows", "macos"]

# Example

# The test will only run under linux
runs_on = ["linux"]
```

```toml
[unix_shell]

# Set the shell to use to run commands under unix systems

# Default value
unix_shell = "sh"

# Example

# The command will run under "zsh" instead of "sh" under linux
unix_shell = "zsh"
```

```toml
[windows_shell]

# Set the shell to use to run commands under windows

# Default value
windows_shell = "cmd"

# Example

# The command will run under "powershell" instead of "cmd" under windows
windows_shell = "powershell"
```

```toml
[exit_status]

# Set the expected exit status of a command

# Default value
exit_status = 0

# Example

# The test will expect a return value of 1
exit_status = 1
cmd = "git"
```

```toml
[timeout]

# Set the timeout of a command (in seconds)

# Default value
timeout = 60

# Example

# The command must not exceed 5 seconds
timeout = 5
cmd = "reload"
```

```toml
[working_dir]

# Set the working directory of a test
# by default, the path is set where the testfile is located
# working_dir value can be relative or a full path

# Default value
working_dir = "."

# Example

# The working directory of the command to root folder
working_dir = "/"
cmd = "ls"

# The working directory of the parent folder of the folder where the testfile is located
working_dir = ".."
cmd = "ls"
```

```toml
[cmd]

# Command to run (Mandatory value)

# Example

# Build a software

cmd = "make"
```

```toml
[stdin]

# stdin content to be forwarded to the command

# Example

# Build a software

cmd = "print_stdin.py"
stdin = "hey"

# expected stdout
stdout = "hey\n"
```

```toml
[stdout]

# stdin content to be forwarded to the command

# Example

# Build a software

cmd = "whoami"
# expected stdout
stdout = "root"
```

```toml
[stderr]

# stdin content to be forwarded to the command

# Example

# Build a software

cmd = "whoami"
# expected stderr
stderr = "root"
```

## 💡 Build Tips
If you want to uninstall Vangelis:  
`cargo uninstall`  

If you just want to get the binary without installing:  
`cargo build`  
