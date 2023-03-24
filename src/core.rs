use crate::config::TestFile;
use crate::diff;
use crate::menu;
use crate::test::{Status, TestResult};
use crate::tools;

use colored::Colorize;

#[derive(PartialEq)]
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
    ci: bool,
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
            ci: false,
        }
    }

    fn parse(&mut self) -> Vec<TestFile> {
        let mut files: Vec<TestFile> = vec![];
        let mut verbose = false;
        let mut diff = false;
        let mut ci = false;

        for ref arg in std::env::args().skip(1) {
            match arg.as_str() {
                "--help" => menu::help(0),
                "--version" => menu::version(),
                "--changelog" => menu::changelog(),
                "--tears" => menu::tears(),
                "--verbose" => verbose = true,
                "--diff" => diff = true,
                "--ci" => ci = true,
                _ => {
                    if arg.starts_with('-') {
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
        if ci && diff {
            panic!("Please choose between --ci and --diff");
        }
        if ci && verbose {
            panic!("--verbose argument is useless when using --ci");
        }
        if ci {
            self.ci = true;
        }
        if verbose || ci {
            self.details = Details::Shell;
        }
        if diff {
            tools::get_vscode_bin()
                .expect("Visual Studio Code is not installed, could not use --diff");
            self.details = Details::VSCode;
        }
        files
    }

    fn apply_result(&mut self, name: &str, result: TestResult) {
        let date_format = format!("{:.1}s", result.exec_time);
        let date_padding = format!(
            "{}{}",
            tools::get_padding(name, date_format.as_str()),
            date_format.blue()
        );

        self.tests += 1;
        match result.status {
            Status::Passed => self.passed += 1,
            Status::Failed => self.failed += 1,
            Status::Timeout => self.timeout += 1,
            Status::Skipped => self.skipped += 1,
            Status::Crashed => self.crashed += 1,
        }
        match result.status {
            Status::Passed => println!("{} {}{}", "[✓]".green(), name.green(), date_padding),
            Status::Failed => println!("{} {}{}", "[✗]".red(), name.red(), date_padding),
            Status::Crashed => println!("{} {}{}", "[!]".yellow(), name.yellow(), date_padding),
            Status::Timeout => println!("{} {}{}", "[?]".magenta(), name.magenta(), date_padding),
            Status::Skipped => println!("{} {}{}", "[>]".white(), name.white(), date_padding),
        }
        match self.details {
            Details::No => {}
            Details::Shell => diff::shell(name, result),
            Details::VSCode => diff::vscode(name, result),
        }
    }

    pub fn run(&mut self) -> i32 {
        let test_files = self.parse();

        tools::hide_cursor();

        'main_loop: for test_file in test_files.into_iter() {
            println!(
                "\n{}\n",
                tools::center(test_file.name.to_string()).bold().cyan()
            );
            for test in test_file.tests.into_iter() {
                self.apply_result(test.name.as_str(), test.run());
                if self.ci && self.tests != self.passed + self.skipped {
                    break 'main_loop;
                }
            }
        }
        println!();

        tools::show_cursor();

        if self.tests == self.passed + self.skipped {
            0
        } else {
            1
        }
    }
}
