use crate::test::{TestResult, Status};
use crate::config::{TestFile};
use crate::menu;
use crate::tools;

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
                _ => {
                    if arg.starts_with("-") {
                        panic!("bad option: {}", arg.as_str())
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

    fn show_diff(&self, name: &str, result: TestResult) {
        match self.details {
            Details::Shell => {
                println!("");
                for diff in diff::lines(result.got.as_str(), result.expected.as_str()) {
                    match diff {
                        diff::Result::Left(l)    => println!("{} {}", " + ".bold().white().on_green(), l.green()),
                        diff::Result::Both(l, _) => println!("{} {}", "   ".on_white(), l),
                        diff::Result::Right(r)   => println!("{} {}", " - ".bold().white().on_red(), r.red()),
                    }
                }
                println!("");
            }
            Details::VSCode => {
                let tmp_path = if cfg!(target_os = "windows") {std::env::var("Temp").unwrap() + "/"} else {"/tmp/".to_string()};

                let file_got = tmp_path.to_owned() + "GOT(" + name + ")";
                std::fs::File::create(&file_got).unwrap().write_fmt(format_args!("{}", result.got)).unwrap();

                let file_expected = tmp_path.to_owned() + "EXPECTED(" + name + ")";
                std::fs::File::create(&file_expected).unwrap().write_fmt(format_args!("{}", result.expected)).unwrap();

                let cmd = "\"".to_owned() + tools::get_vscode_bin().unwrap().as_str() + "\" --diff \"" + file_got.as_str() + "\" \"" + file_expected.as_str() + "\"";

                std::process::Command::new(if cfg!(target_os = "windows") {"cmd"} else {"sh"})
                    .args([if cfg!(target_os = "windows") {"/C"} else {"-c"}, cmd.as_str()])
                    .output()
                    .expect("failed to execute vscode process");
            }
            Details::No => {}
        }
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
            Status::Passed => println!("{} {}{}", "[V]".green(), name.green(), date_padding),
            Status::Failed => println!("{} {}{}", "[X]".red(), name.red(), date_padding),
            Status::Crashed => println!("{} {}{}", "[!]".yellow(), name.yellow(), date_padding),
            Status::Timeout => println!("{} {}{}", "[?]".magenta(), name.magenta(), date_padding),
            Status::Skipped => println!("{} {}{}", "[>]".white(), name.white(), date_padding),
        }
        if result.got != result.expected {
            self.show_diff(name, result);
        }
    }

    pub fn run(&mut self) -> i32 {
        let test_files = self.parse();

        tools::hide_cursor();
        for test_file in test_files.into_iter() {
            println!("{}\n", test_file.name.bold().blue());
            for test in test_file.tests.into_iter() {
                self.apply_result(test.name.as_str(), test.run());
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
