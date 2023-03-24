use crate::test::Test;
use indexmap::IndexMap;
use std::io::Read;
use std::path::PathBuf;

#[derive(serde::Deserialize)]
struct RawDefault {
    runs_on: Option<Vec<String>>,
    exit_status: Option<i32>,
    timeout: Option<f32>,
    working_dir: Option<String>,
    unix_shell: Option<String>,
    windows_shell: Option<String>,
}

#[derive(serde::Deserialize)]
struct RawTest {
    runs_on: Option<Vec<String>>,
    unix_shell: Option<String>,
    windows_shell: Option<String>,
    exit_status: Option<i32>,
    timeout: Option<f32>,
    working_dir: Option<String>,
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

struct Default {
    runs_on: Vec<String>,
    exit_status: i32,
    timeout: f32,
    working_dir: String,
    unix_shell: String,
    windows_shell: String,
}

impl Default {
    pub fn new(default: Option<RawDefault>) -> Self {
        let mut this = Default {
            runs_on: vec!["linux".to_owned(), "macos".to_owned(), "windows".to_owned()],
            exit_status: 0,
            timeout: 60.0,
            working_dir: ".".to_owned(),
            unix_shell: "sh".to_owned(),
            windows_shell: "cmd".to_owned(),
        };
        match default {
            Some(def) => {
                if let Some(runs_on) = def.runs_on {
                    this.runs_on = runs_on;
                }
                if let Some(exit_status) = def.exit_status {
                    this.exit_status = exit_status;
                }
                if let Some(timeout) = def.timeout {
                    this.timeout = timeout;
                }
                if let Some(working_dir) = def.working_dir {
                    this.working_dir = working_dir;
                }
                if let Some(unix_shell) = def.unix_shell {
                    this.unix_shell = unix_shell;
                }
                if let Some(windows_shell) = def.windows_shell {
                    this.windows_shell = windows_shell;
                }
                this
            }
            None => this,
        }
    }
}

impl TestFile {
    pub fn new(path: &str) -> Self {
        let mut test_file = TestFile {
            name: path.to_owned(),
            tests: vec![],
        };
        let mut buff = String::new();
        let mut file = match std::fs::File::open(path) {
            Ok(val) => val,
            Err(err) => panic!("Could not read \"{}\": {}", path, err),
        };
        match file.read_to_string(&mut buff) {
            Ok(_) => (),
            Err(err) => panic!("Could not read \"{}\": {}", path, err),
        };
        let raw_test_file: RawTestFile = match toml::from_str(&buff) {
            Ok(val) => val,
            Err(err) => panic!("Parsing error: {} in test file \"{}\"", err, path),
        };
        let default = Default::new(raw_test_file.default);
        for test in raw_test_file.test.into_iter() {
            let working_directory = PathBuf::from(path).parent().unwrap().join(
                test.1
                    .working_dir
                    .unwrap_or_else(|| default.working_dir.clone()),
            );
            if !working_directory.exists() {
                panic!(
                    "Working directory \"{}\" does not exist",
                    working_directory.to_str().unwrap()
                );
            }
            test_file.tests.push(Test {
                name: test.0,
                cmd: test.1.cmd,
                stdin: test.1.stdin,
                stdout: test.1.stdout,
                stderr: test.1.stderr,
                exit_status: test.1.exit_status.unwrap_or(default.exit_status),
                timeout: test.1.timeout.unwrap_or(default.timeout),
                working_dir: working_directory,
                runs_on: test.1.runs_on.unwrap_or_else(|| default.runs_on.clone()),
                unix_shell: test
                    .1
                    .unix_shell
                    .unwrap_or_else(|| default.unix_shell.clone()),
                windows_shell: test
                    .1
                    .windows_shell
                    .unwrap_or_else(|| default.windows_shell.clone()),
            });
        }
        test_file
    }
}
