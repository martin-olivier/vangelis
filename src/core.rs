use crate::menu;
use crate::tools;
use crate::test::{TestResult, Status};
use crate::testfile::{TestFile};

use colored::Colorize;
use std::io::Write;

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
        }
    }

    fn parse_args(&mut self) -> Vec<String> {
        let mut files: Vec<String> = vec![];
        let mut verbose = false;
        let mut diff = false;

        for ref arg in std::env::args().skip(1) {
            match arg.as_str() {
                "--help" => menu::help(0),
                "--version" => menu::version(),
                "--changelog" => menu::changelog(),
                "--verbose" => verbose = true,
                "--diff" => diff = true,
                _ => {
                    if arg.starts_with("-") {
                        panic!("bad option: {}", arg.as_str())
                    } else {
                        files.push(arg.to_string());
                    }
                }
            }
        }
        if files.is_empty() {
            menu::help(1);
        }
        if verbose && diff {
            panic!("bad option: please choose between --verbose and --diff");
        }
        if verbose {
            self.details = Details::Shell;
        }
        if diff {
            tools::get_vscode_bin().expect("Visual Studio Code is not installed");
            self.details = Details::VSCode;
        }
        files
    }

    fn parse_files(&mut self, files_list: Vec<String>) -> Vec<TestFile> {
        let mut test_files: Vec<TestFile> = vec![];
        for file in files_list.into_iter() {
            test_files.push(TestFile::new(&file));
        }
        test_files
    }

    fn parse(&mut self) -> Vec<TestFile> {
        let files_list = self.parse_args();
        self.parse_files(files_list)
    }

    fn apply_result(&mut self, name: &str, result: TestResult) {
        let date_formated = format!("{:.1}s", result.exec_time);
        let date_padding = format!("{}{}", tools::get_padding(name, date_formated.as_str()), date_formated.blue());
        match result.status {
            Status::Passed => {self.tests += 1; self.passed += 1},
            Status::Failed => {self.tests += 1; self.failed += 1},
            Status::Timeout => {self.tests += 1; self.timeout += 1},
            Status::Skipped => {self.tests += 1; self.skipped += 1},
            Status::Crashed => {self.tests += 1; self.crashed += 1},
        }
        match result.status {
            Status::Passed => println!("{} {}{}", "[✓]".bright_green().bold(), name.bright_green(), date_padding),
            Status::Failed => println!("{} {}{}", "[✗]".bright_red().bold(), name.bright_red(), date_padding),
            Status::Crashed => println!("{} {}{}", "[!]".bright_yellow().bold(), name.bright_yellow(), date_padding),
            Status::Timeout => println!("{} {}{}", "[?]".bright_magenta().bold(), name.bright_magenta(), date_padding),
            Status::Skipped => println!("{} {}{}", "[>]".bright_white().bold(), name.bright_white(), date_padding),
        }
        if result.got == result.expected {
            return;
        }
        match self.details {
            Details::Shell => {
                for diff in diff::lines(result.got.as_str(), result.expected.as_str()) {
                    match diff {
                        diff::Result::Left(l)    => println!("{} {}", "+".green(), l.green()),
                        diff::Result::Both(l, _) => println!("{} {}", " ", l),
                        diff::Result::Right(r)   => println!("{} {}", "-".red(), r.red()),
                    }
                }
            }
            Details::VSCode => {
                let file_got = "/tmp/GOT(".to_owned() + name + ")";
                std::fs::File::create(&file_got).unwrap().write_fmt(format_args!("{}", result.got)).unwrap();

                let file_expected = "/tmp/EXPECTED(".to_owned() + name + ")";
                std::fs::File::create(&file_expected).unwrap().write_fmt(format_args!("{}", result.expected)).unwrap();

                std::process::Command::new(tools::get_vscode_bin().unwrap().as_str())
                    .args(["--diff", file_got.as_str(), file_expected.as_str()])
                    .output()
                    .expect("failed to execute vscode");
            }
            _ => {}
        }
    }

    pub fn run(&mut self) -> i32 {
        let test_files = self.parse();
        let multiple_files = test_files.len() != 1;

        for test_file in test_files.into_iter() {
            if multiple_files {
                println!("{}\n", test_file.name.bold().blue());
            }
            for test in test_file.tests.into_iter() {
                self.apply_result(test.name.as_str(), test.run());
            }
            println!("");
        }
        println!("> Tests: {} | Passed: {} | Failed: {} | Crashed: {} | Timeout: {} | Skipped: {}",
            self.tests.to_string().bold().bright_blue(),
            self.passed.to_string().bold().bright_green(),
            self.failed.to_string().bold().bright_red(),
            self.crashed.to_string().bold().bright_yellow(),
            self.timeout.to_string().bold().bright_magenta(),
            self.skipped.to_string().bold().bright_white(),
        );
        if self.tests == self.passed + self.skipped {0} else {1}
    }
}
