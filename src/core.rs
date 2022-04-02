use crate::menu;
use crate::config;
use crate::tools;

use colored::Colorize;
use std::io::Read;
use std::io::Write;

enum Details {
    No,
    Shell,
    VSCode,
}

pub enum Status {
    Passed,
    Crashed,
    Failed,
    Timeout,
    Skipped,
}

pub struct TestResult {
    pub status: Status,
    pub exec_time: f32,
    pub got: String,
    pub expected: String,
}

impl TestResult {
    pub fn new() -> Self {
        TestResult {
            status: Status::Passed,
            exec_time: 0.0,
            got: "".to_owned(),
            expected: "".to_owned()
        }
    }
}

pub struct Test {
    pub name: String,
    pub cmd: String,
    pub stdin: Option<String>,
    pub stdout: Option<String>,
    pub stderr: Option<String>,
    pub code: i32,
    pub timeout: f32,
    pub runs_on: Vec<String>,
}

struct ProcessResult {
    timeout: bool,
    exec_time: f32,
    code: Option<i32>,
    stdout: String,
    stderr: String,
}

impl ProcessResult {
    pub fn new() -> Self {
        ProcessResult {
            timeout: false,
            exec_time: 0.0,
            code: None,
            stdout: "".to_owned(),
            stderr: "".to_owned(),
        }
    }
}

pub struct TestFile {
    pub name: String,
    pub tests: Vec<Test>,
}

fn get_exit_code(test: &Test, mut child: std::process::Child) -> ProcessResult {
    let mut process_result = ProcessResult::new();
    let limit = std::time::Instant::now() + std::time::Duration::from_millis((test.timeout * 1000.0) as u64);
    let init_time = std::time::Instant::now();
    loop {
        match child.try_wait() {
            Ok(Some(code)) => {
                process_result.code = code.code();
                break;
            }
            Ok(None) => {
                if std::time::Instant::now() < limit {
                    let date_formated = format!("{:.1}s ", (std::time::Instant::now() - init_time).as_secs_f32());
                    let date_padding = format!("{}{}", tools::get_padding(test.name.as_str(), date_formated.as_str()), date_formated.blue());
                    print!("\r{} {}{}", "[+]".bright_blue().bold(), test.name, date_padding);
                    std::io::stdout().flush().unwrap();
                    std::thread::sleep(std::time::Duration::from_millis(10));
                }
                else {
                    child.kill().unwrap();
                    process_result.timeout = true;
                    process_result.code = child.wait().unwrap().code();
                    break;
                }
            },
            Err(_) => {
                panic!("Child process is not responding");
            }
        }
    }
    print!("\r");
    process_result.exec_time = (std::time::Instant::now() - init_time).as_secs_f32();
    child.stdout.unwrap().read_to_string(&mut process_result.stdout).unwrap();
    child.stderr.unwrap().read_to_string(&mut process_result.stderr).unwrap();
    process_result
}

fn run_command(test: &Test) -> Option<ProcessResult> {
    if cfg!(target_os = "windows") && !test.runs_on.contains(&"windows".to_owned()) {
        return None;
    }
    else if cfg!(target_os = "linux") && !test.runs_on.contains(&"linux".to_owned()) {
        return None;
    }
    else if cfg!(target_os = "macos") && !test.runs_on.contains(&"macos".to_owned()) {
        return None;
    }
    let mut child = std::process::Command::new(if cfg!(target_os = "windows") {"cmd"} else {"sh"})
        .args([if cfg!(target_os = "windows") {"/C"} else {"-c"}, test.cmd.as_str()])
        .stdin(if test.stdin.is_some() {std::process::Stdio::piped()} else {std::process::Stdio::null()})
        .stdout(std::process::Stdio::piped())
        .stderr(std::process::Stdio::piped())
        .spawn()
        .expect("failed to execute process");

    if let Some(ref stdin) = test.stdin {
        child.stdin.take().unwrap().write_all(stdin.as_bytes()).unwrap();
    }
    let process_result = get_exit_code(test, child);
    Some(process_result)
}

impl Test {
    pub fn run(&self) -> TestResult {
        let mut result = TestResult::new();
        match run_command(&self) {
            Some(val) => {
                result.exec_time = val.exec_time;
                match val.code {
                    Some(code) => {
                        if code != self.code {
                            if code >= 132 && code <= 139 {
                                result.status = Status::Crashed;
                            } else {
                                result.status = Status::Failed;
                            }
                            result.got.push_str(&("[EXIT CODE]\nvalue = ".to_owned() + code.to_string().as_str() + "\n\n"));
                            result.expected.push_str(&("[EXIT CODE]\nvalue = ".to_owned() + self.code.to_string().as_str() + "\n\n"));
                        }
                    }
                    None => result.status = Status::Crashed,
                }
                if let Some(ref stdout) = self.stdout {
                    if *stdout != val.stdout {
                        result.status = match result.status {
                            Status::Crashed => result.status,
                            _ => Status::Failed,
                        };
                        result.got.push_str(&("[STDOUT]\n".to_owned() + val.stdout.as_str() + "\n\n"));
                        result.expected.push_str(&("[STDOUT]\n".to_owned() + stdout.as_str() + "\n\n"));
                    }
                }
                if let Some(ref stderr) = self.stderr {
                    if *stderr != val.stderr {
                        result.status = match result.status {
                            Status::Crashed => result.status,
                            _ => Status::Failed,
                        };
                        result.got.push_str(&("[STDERR]\n".to_owned() + val.stderr.as_str() + "\n\n"));
                        result.expected.push_str(&("[STDERR]\n".to_owned() + stderr.as_str() + "\n\n"));
                    }
                }
                if val.timeout { result.status = Status::Timeout };
            }
            None => result.status = Status::Skipped,
        }
        result
    }
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
            tools::get_vscode_bin().expect("Please install Visual Studio Code");
            self.details = Details::VSCode;
        }
        files
    }

    fn parse_files(&mut self, files_list: Vec<String>) -> Vec<TestFile> {
        let mut test_files: Vec<TestFile> = vec![];
        for file in files_list.into_iter() {
            test_files.push(config::get_test_file(&file));
        }
        test_files
    }

    fn parse(&mut self) -> Vec<TestFile> {
        let files_list = self.parse_args();
        self.parse_files(files_list)
    }

    fn apply_result(&mut self, name: &str, result: TestResult) {
        let date_formated = format!("{:.1}s ", result.exec_time);
        let date_padding = format!("{}{}", tools::get_padding(name, date_formated.as_str()), date_formated.blue());
        match result.status {
            Status::Passed => {self.tests += 1; self.passed += 1},
            Status::Failed => {self.tests += 1; self.failed += 1},
            Status::Timeout => {self.tests += 1; self.timeout += 1},
            Status::Skipped => {self.skipped += 1;}
            Status::Crashed => {self.tests += 1; self.crashed += 1},
        }
        match result.status {
            Status::Passed => println!("{} {}{}", "[✓]".bright_green().bold(), name, date_padding),
            Status::Failed => println!("{} {}{}", "[✗]".bright_red().bold(), name, date_padding),
            Status::Crashed => println!("{} {}{}", "[!]".bright_yellow().bold(), name, date_padding),
            Status::Timeout => println!("{} {}{}", "[?]".bright_magenta().bold(), name, date_padding),
            Status::Skipped => {println!("{} {}{}", "[>]".bright_white().bold(), name, date_padding); return},
        }
        if result.got == result.expected {
            return;
        }
        match self.details {
            Details::Shell => {
                println!("[GOT]\n{}", result.got);
                println!("[EXPECTED]\n{}", result.expected);
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
        println!("Tests: {} | Passed: {} | Failed: {} | Crashed: {} | Timeout: {} | Skipped: {}",
            self.tests.to_string().bold().bright_blue(),
            self.passed.to_string().bold().bright_green(),
            self.failed.to_string().bold().bright_red(),
            self.crashed.to_string().bold().bright_yellow(),
            self.timeout.to_string().bold().bright_magenta(),
            self.skipped.to_string().bold().bright_white(),
        );
        if self.tests == self.passed {0} else {1}
    }
}
