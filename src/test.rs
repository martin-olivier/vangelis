use crate::tools;

use std::io::Read;
use std::io::Write;
use colored::Colorize;

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

pub struct Test {
    pub name: String,
    pub cmd: String,
    pub stdin: Option<String>,
    pub stdout: Option<String>,
    pub stderr: Option<String>,
    pub code: i32,
    pub timeout: f32,
    pub runs_on: Vec<String>,
    pub unix_shell: String,
    pub windows_shell: String,
}

impl Test {
    fn wait(&self, mut child: std::process::Child) -> ProcessResult {
        let mut process_result = ProcessResult::new();
        let limit = std::time::Instant::now() + std::time::Duration::from_millis((self.timeout * 1000.0) as u64);
        let init_time = std::time::Instant::now();
        loop {
            match child.try_wait() {
                Ok(Some(code)) => {
                    process_result.code = code.code();
                    break;
                }
                Ok(None) => {
                    if std::time::Instant::now() < limit {
                        let date_formated = format!("{:.1}s", (std::time::Instant::now() - init_time).as_secs_f32());
                        let date_padding = format!("{}{}", tools::get_padding(self.name.as_str(), date_formated.as_str()), date_formated.blue());
                        if atty::is(atty::Stream::Stdout) {
                            print!("\r{} {}{}\r", "[>]".blue(), self.name.blue(), date_padding);
                            std::io::stdout().flush().unwrap();
                        }
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
        process_result.exec_time = (std::time::Instant::now() - init_time).as_secs_f32();

        let mut outbuf = vec![];
        child.stdout.unwrap().read_to_end(&mut outbuf).unwrap();
        process_result.stdout = String::from_utf8_lossy(&outbuf).to_string().replace("\r\n", "\n");

        let mut errbuf = vec![];
        child.stderr.unwrap().read_to_end(&mut errbuf).unwrap();
        process_result.stderr = String::from_utf8_lossy(&errbuf).to_string().replace("\r\n", "\n");

        process_result
    }

    fn run_command(&self) -> Option<ProcessResult> {
        if cfg!(target_os = "windows") && !self.runs_on.contains(&"windows".to_owned()) {
            return None;
        }
        else if cfg!(target_os = "linux") && !self.runs_on.contains(&"linux".to_owned()) {
            return None;
        }
        else if cfg!(target_os = "macos") && !self.runs_on.contains(&"macos".to_owned()) {
            return None;
        }
        let mut child = std::process::Command::new(if cfg!(target_os = "windows") {self.windows_shell.as_str()} else {self.unix_shell.as_str()})
            .args([if cfg!(target_os = "windows") {"/C"} else {"-c"}, self.cmd.as_str()])
            .stdin(if self.stdin.is_some() {std::process::Stdio::piped()} else {std::process::Stdio::null()})
            .stdout(std::process::Stdio::piped())
            .stderr(std::process::Stdio::piped())
            .spawn()
            .expect("failed to execute process");
    
        if let Some(ref stdin) = self.stdin {
            child.stdin.take().unwrap().write_all(stdin.as_bytes()).unwrap();
        }
        Some(self.wait(child))
    }

    pub fn run(&self) -> TestResult {
        let mut result = TestResult::new();
        match self.run_command() {
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
                        }
                        result.got.push_str(&("[EXIT CODE]\nvalue = ".to_owned() + code.to_string().as_str() + "\n\n"));
                        result.expected.push_str(&("[EXIT CODE]\nvalue = ".to_owned() + self.code.to_string().as_str() + "\n\n"));
                    }
                    None => result.status = Status::Crashed,
                }
                if let Some(ref stdout) = self.stdout {
                    if *stdout != val.stdout {
                        result.status = match result.status {
                            Status::Crashed => result.status,
                            _ => Status::Failed,
                        };
                    }
                    result.got.push_str(&("[STDOUT]\n".to_string() + val.stdout.as_str() + "\n\n"));
                    result.expected.push_str(&("[STDOUT]\n".to_string() + stdout.as_str() + "\n\n"));
                }
                if let Some(ref stderr) = self.stderr {
                    if *stderr != val.stderr {
                        result.status = match result.status {
                            Status::Crashed => result.status,
                            _ => Status::Failed,
                        };
                    }
                    result.got.push_str(&("[STDERR]\n".to_owned() + val.stderr.as_str() + "\n\n"));
                    result.expected.push_str(&("[STDERR]\n".to_owned() + stderr.as_str() + "\n\n"));
                }
                if result.got.ends_with("\n\n") && result.expected.ends_with("\n\n") {
                    result.got.pop();
                    result.got.pop();
                    result.expected.pop();
                    result.expected.pop();
                }
                if val.timeout { result.status = Status::Timeout };
            }
            None => result.status = Status::Skipped,
        }
        result
    }
}