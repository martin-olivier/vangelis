use crate::test::Test;
use std::io::Read;
use indexmap::IndexMap;

#[derive(serde::Deserialize)]
struct RawDefault {
    runs_on: Option<Vec<String>>,
    code: Option<i32>,
    timeout: Option<f32>,
    unix_shell: Option<String>,
    windows_shell: Option<String>,
}

#[derive(serde::Deserialize)]
struct RawTest {
    runs_on: Option<Vec<String>>,
    unix_shell: Option<String>,
    windows_shell: Option<String>,
    code: Option<i32>,
    timeout: Option<f32>,
    cmd: String,
    stdin: Option<String>,
    stdout: Option<String>,
    stderr: Option<String>,
}

#[derive(serde::Deserialize)]
struct RawTestFile {
    default: Option<RawDefault>,
    test: IndexMap<String, RawTest>,
}

pub struct TestFile {
    pub name: String,
    pub tests: Vec<Test>,
}

impl TestFile {
    pub fn new(path: &str) -> Self {
        parse_config(get_config(path), path)
    }
}

struct Default {
    runs_on: Vec<String>,
    code: i32,
    timeout: f32,
    unix_shell: String,
    windows_shell: String,
}

impl Default {
    pub fn new(default: Option<RawDefault>) -> Self {
        let mut this = Default {
            runs_on: vec!["linux".to_owned(), "macos".to_owned(), "windows".to_owned()],
            code: 0,
            timeout: 60.0,
            unix_shell: "sh".to_owned(),
            windows_shell: "cmd".to_owned(),
        };
        match default {
            Some(def) => {
                if let Some(runs_on) = def.runs_on {
                    this.runs_on = runs_on;
                }
                if let Some(code) = def.code {
                    this.code = code;
                }
                if let Some(timeout) = def.timeout {
                    this.timeout = timeout;
                }
                if let Some(unix_shell) = def.unix_shell {
                    this.unix_shell = unix_shell;
                }
                if let Some(windows_shell) = def.windows_shell {
                    this.windows_shell = windows_shell;
                }
                this
            },
            None => this
        }
    }
}

fn get_config(path: &str) -> RawTestFile {
    let mut buff = String::new();
    let mut file = match std::fs::File::open(path) {
        Ok(val) => val,
        Err(err) => panic!("Could not read file \"{}\": {}", path, err)
    };
    match file.read_to_string(&mut buff) {
        Ok(_) => (),
        Err(err) => panic!("Could not read file \"{}\": {}", path, err)
    };
    match toml::from_str(&buff) {
        Ok(val) => val,
        Err(err) => panic!("Parsing error: {} in test file \"{}\"", err, path)
    }
}

fn parse_test(raw_test: (std::string::String, RawTest), default: &Default) -> Test {
    Test {
        name: raw_test.0,
        cmd: raw_test.1.cmd,
        stdin: raw_test.1.stdin,
        stdout: raw_test.1.stdout,
        stderr: raw_test.1.stderr,
        code: raw_test.1.code.unwrap_or(default.code),
        timeout: raw_test.1.timeout.unwrap_or(default.timeout),
        runs_on: raw_test.1.runs_on.unwrap_or(default.runs_on.clone()),
        unix_shell: raw_test.1.unix_shell.unwrap_or(default.unix_shell.clone()),
        windows_shell: raw_test.1.windows_shell.unwrap_or(default.windows_shell.clone()),
    }
}

fn parse_config(raw_test_file: RawTestFile, path: &str) -> TestFile {
    let mut test_file =  TestFile{name: path.to_owned(), tests: vec![]};
    let default = Default::new(raw_test_file.default);
    for test in raw_test_file.test.into_iter() {
        test_file.tests.push(parse_test(test, &default));
    }
    test_file
}