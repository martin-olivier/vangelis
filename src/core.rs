use crate::args::Args;
use crate::config::TestFile;
use crate::diff;
use crate::test::{Status, TestResult};
use crate::tools;

use clap::Parser;
use colored::Colorize;

#[derive(PartialEq)]
enum Mode {
    NoDiff,
    Diff,
    CI,
}

pub struct Core {
    tests: u32,
    passed: u32,
    failed: u32,
    crashed: u32,
    timeout: u32,
    skipped: u32,
    mode: Mode,
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
            mode: Mode::NoDiff,
        }
    }

    fn parse(&mut self) -> Vec<TestFile> {
        let args = Args::parse();
        let mut files = vec![];

        for arg in args.test_file {
            files.push(TestFile::new(&arg));
        }

        if args.ci && args.diff {
            panic!("--diff argument is useless when using --ci");
        }
        if args.ci {
            self.mode = Mode::CI;
        }
        if args.diff {
            self.mode = Mode::Diff;
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
        match self.mode {
            Mode::NoDiff => {}
            Mode::Diff | Mode::CI => diff::shell(result),
        }
    }

    pub fn run(&mut self) -> i32 {
        let test_files = self.parse();

        tools::hide_cursor();

        'main_loop: for test_file in test_files.into_iter() {
            println!("\n{}\n", tools::center(&test_file.name).bold().cyan());
            for test in test_file.tests.into_iter() {
                self.apply_result(&test.name, test.run());
                if self.mode == Mode::CI && self.tests != self.passed + self.skipped {
                    break 'main_loop;
                }
            }
        }
        println!();

        tools::show_cursor();

        match self.tests == self.passed + self.skipped {
            true => 0,
            false => 1,
        }
    }
}
