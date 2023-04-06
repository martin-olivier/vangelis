use crate::tools;

use colored::Colorize;
use std::io::{Read, Write};
use std::path::PathBuf;
use std::process::{Child, Command, Stdio};
use std::time::{Duration, Instant};

#[derive(PartialEq)]
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
    pub got_exit_status: Option<i32>,
    pub got_stdout: String,
    pub got_stderr: String,
    pub expected_exit_status: i32,
    pub expected_stdout: Option<String>,
    pub expected_stderr: Option<String>,
}

impl TestResult {
    pub fn new() -> Self {
        TestResult {
            status: Status::Skipped,
            exec_time: 0.0,
            got_exit_status: None,
            got_stdout: "".to_owned(),
            got_stderr: "".to_owned(),
            expected_exit_status: 0,
            expected_stdout: None,
            expected_stderr: None,
        }
    }
}

struct ProcessResult {
    timeout: bool,
    exec_time: f32,
    exit_status: Option<i32>,
    stdout: String,
    stderr: String,
}

impl ProcessResult {
    pub fn new() -> Self {
        ProcessResult {
            timeout: false,
            exec_time: 0.0,
            exit_status: None,
            stdout: "".to_owned(),
            stderr: "".to_owned(),
        }
    }
}

pub struct Test {
    pub name: String,
    pub cmd: String,
    pub stdin: Option<String>,
    pub stdout: Option<String>,
    pub stderr: Option<String>,
    pub exit_status: i32,
    pub timeout: f32,
    pub working_dir: PathBuf,
    pub runs_on: Vec<String>,
    pub unix_shell: String,
    pub windows_shell: String,
}

impl Test {
    fn wait(&self, mut child: Child) -> ProcessResult {
        let mut process_result = ProcessResult::new();
        let init_time = Instant::now();
        let limit = init_time + Duration::from_millis((self.timeout * 1000.0) as u64);

        loop {
            match child.try_wait() {
                Ok(Some(exit_status)) => {
                    process_result.exit_status = exit_status.code();
                    break;
                }
                Ok(None) => {
                    if Instant::now() < limit {
                        let date_format =
                            format!("{:.1}s", (Instant::now() - init_time).as_secs_f32());
                        let date_padding = format!(
                            "{}{}",
                            tools::get_padding(&self.name, &date_format),
                            date_format.blue(),
                        );
                        if atty::is(atty::Stream::Stdout) {
                            print!("\r{} {}{}\r", "[>]".blue(), self.name.blue(), date_padding);
                            std::io::stdout().flush().unwrap();
                        }
                        std::thread::sleep(Duration::from_millis(10));
                    } else {
                        child.kill().unwrap();
                        process_result.timeout = true;
                        process_result.exit_status = None;
                        break;
                    }
                }
                Err(_) => {
                    panic!("Child process is not responding");
                }
            }
        }
        process_result.exec_time = (Instant::now() - init_time).as_secs_f32();

        let mut outbuf = vec![];
        child.stdout.unwrap().read_to_end(&mut outbuf).unwrap();
        process_result.stdout = String::from_utf8_lossy(&outbuf)
            .to_string()
            .replace("\r\n", "\n");

        let mut errbuf = vec![];
        child.stderr.unwrap().read_to_end(&mut errbuf).unwrap();
        process_result.stderr = String::from_utf8_lossy(&errbuf)
            .to_string()
            .replace("\r\n", "\n");

        process_result
    }

    fn run_command(&self) -> Option<ProcessResult> {
        if !self.runs_on.contains(&std::env::consts::OS.to_string()) {
            return None;
        }

        let mut child = Command::new(if cfg!(target_os = "windows") {
            &self.windows_shell
        } else {
            &self.unix_shell
        })
        .current_dir(&self.working_dir)
        .args([
            if cfg!(target_os = "windows") {
                "/C"
            } else {
                "-c"
            },
            &self.cmd,
        ])
        .stdin(if self.stdin.is_some() {
            Stdio::piped()
        } else {
            Stdio::null()
        })
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()
        .expect("failed to execute process");

        if let Some(ref stdin) = self.stdin {
            child
                .stdin
                .take()
                .unwrap()
                .write_all(stdin.as_bytes())
                .unwrap();
        }
        Some(self.wait(child))
    }

    fn parse_result(&self, process_result: ProcessResult) -> TestResult {
        let mut test_result = TestResult::new();
        test_result.status = Status::Passed;
        test_result.exec_time = process_result.exec_time;
        test_result.expected_exit_status = self.exit_status;

        match process_result.exit_status {
            Some(exit_status) => {
                if exit_status != self.exit_status {
                    if (132..=139).contains(&exit_status) {
                        test_result.status = Status::Crashed;
                    } else {
                        test_result.status = Status::Failed;
                    }
                }
                test_result.got_exit_status = Some(exit_status);
            }
            None => test_result.status = Status::Crashed,
        }

        if let Some(ref stdout) = self.stdout {
            if *stdout != process_result.stdout {
                test_result.status = match test_result.status {
                    Status::Crashed => test_result.status,
                    _ => Status::Failed,
                };
            }
            test_result.got_stdout = process_result.stdout;
            test_result.expected_stdout = Some(stdout.to_owned());
        }

        if let Some(ref stderr) = self.stderr {
            if *stderr != process_result.stderr {
                test_result.status = match test_result.status {
                    Status::Crashed => test_result.status,
                    _ => Status::Failed,
                };
            }
            test_result.got_stderr = process_result.stderr;
            test_result.expected_stderr = Some(stderr.to_owned());
        }

        if process_result.timeout {
            test_result.status = Status::Timeout
        }

        test_result
    }

    pub fn run(&self) -> TestResult {
        match self.run_command() {
            Some(res) => self.parse_result(res),
            None => TestResult::new(),
        }
    }
}
