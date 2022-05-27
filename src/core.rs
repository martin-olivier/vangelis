use crate::test::{TestResult, Status};
use crate::config::{TestFile};
use crate::menu;
use crate::tools;
use crate::diff;

use colored::Colorize;

enum Details {
    No,
    Shell,
    VSCode,
}

pub struct Core {
    tests: u32,
    passed: u32,
    failed: u32,
    crashed: u32,
    timeout: u32,
    skipped: u32,
    details: Details,
    stop_on_fail: bool,
}

impl Core {
    pub fn new() -> Self {
        Core {
            tests: 0,
            passed: 0,
            failed: 0,
            crashed: 0,
            timeout: 0,
            skipped: 0,
            details: Details::No,
            stop_on_fail: false,
        }
    }

    fn parse(&mut self) -> Vec<TestFile> {
        let mut files: Vec<TestFile> = vec![];
        let mut verbose = false;
        let mut diff = false;

        for ref arg in std::env::args().skip(1) {
            match arg.as_str() {
                "--help" => menu::help(0),
                "--version" => menu::version(),
                "--changelog" => menu::changelog(),
                "--verbose" => verbose = true,
                "--diff" => diff = true,
                "--stop_on_fail" => self.stop_on_fail = true,
                _ => {
                    if arg.starts_with("-") {
                        panic!("Unknown option: {}", arg.as_str())
                    } else {
                        files.push(TestFile::new(arg.as_str()));
                    }
                }
            }
        }
        if files.is_empty() {
            menu::help(1);
        }
        if verbose && diff {
            panic!("Please choose between --verbose and --diff");
        }
        if verbose {
            self.details = Details::Shell;
        }
        if diff {
            tools::get_vscode_bin().expect("Visual Studio Code is not installed, could not use --diff");
            self.details = Details::VSCode;
        }
        files
    }

    fn apply_result(&mut self, name: &str, result: TestResult) {
        let date_format = format!("{:.1}s", result.exec_time);
        let date_padding = format!("{}{}", tools::get_padding(name, date_format.as_str()), date_format.blue());

        match result.status {
            Status::Passed  => {self.tests += 1; self.passed  += 1},
            Status::Failed  => {self.tests += 1; self.failed  += 1},
            Status::Timeout => {self.tests += 1; self.timeout += 1},
            Status::Skipped => {self.tests += 1; self.skipped += 1},
            Status::Crashed => {self.tests += 1; self.crashed += 1},
        }
        match result.status {
            Status::Passed  => println!("{} {}{}", "[V]".green(), name.green(), date_padding),
            Status::Failed  => println!("{} {}{}", "[X]".red(), name.red(), date_padding),
            Status::Crashed => println!("{} {}{}", "[!]".yellow(), name.yellow(), date_padding),
            Status::Timeout => println!("{} {}{}", "[?]".magenta(), name.magenta(), date_padding),
            Status::Skipped => println!("{} {}{}", "[>]".white(), name.white(), date_padding),
        }
        match self.details {
            Details::No     => {},
            Details::Shell  => diff::shell(name, result),
            Details::VSCode => diff::vscode(name, result),
        }
    }

    pub fn run(&mut self) -> i32 {
        let test_files = self.parse();

        tools::hide_cursor();
        for test_file in test_files.into_iter() {
            println!("\n{}\n", tools::center(format!("{}", test_file.name.bold().blue().to_string())));
            for test in test_file.tests.into_iter() {
                self.apply_result(test.name.as_str(), test.run());
                if self.stop_on_fail && self.tests != self.passed + self.skipped {
                    break
                }
            }
        }
        tools::show_cursor();

        println!("\n> Tests: {} | Passed: {} | Failed: {} | Crashed: {} | Timeout: {} | Skipped: {}",
            self.tests.to_string().blue(),
            self.passed.to_string().green(),
            self.failed.to_string().red(),
            self.crashed.to_string().yellow(),
            self.timeout.to_string().magenta(),
            self.skipped.to_string().white(),
        );
        if self.tests == self.passed + self.skipped {0} else {1}
    }
}
