use crate::test::Test;
use indexmap::IndexMap;
use std::fs::File;
use std::io::Read;
use std::path::PathBuf;

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
    default: Option<RawDefaults>,
    test: IndexMap<String, RawTest>,
}

#[derive(serde::Deserialize)]
struct RawDefaults {
    runs_on: Option<Vec<String>>,
    exit_status: Option<i32>,
    timeout: Option<f32>,
    working_dir: Option<String>,
    unix_shell: Option<String>,
    windows_shell: Option<String>,
}

struct Defaults {
    runs_on: Vec<String>,
    exit_status: i32,
    timeout: f32,
    working_dir: String,
    unix_shell: String,
    windows_shell: String,
}

impl Defaults {
    pub fn new(default: RawDefaults) -> Self {
        Self {
            runs_on: default.runs_on.unwrap_or(Self::default().runs_on),
            exit_status: default.exit_status.unwrap_or(Self::default().exit_status),
            timeout: default.timeout.unwrap_or(Self::default().timeout),
            working_dir: default.working_dir.unwrap_or(Self::default().working_dir),
            unix_shell: default.unix_shell.unwrap_or(Self::default().unix_shell),
            windows_shell: default
                .windows_shell
                .unwrap_or(Self::default().windows_shell),
        }
    }
}

impl Default for Defaults {
    fn default() -> Self {
        Self {
            runs_on: vec!["linux".to_owned(), "macos".to_owned(), "windows".to_owned()],
            exit_status: 0,
            timeout: 60.0,
            working_dir: ".".to_owned(),
            unix_shell: "sh".to_owned(),
            windows_shell: "cmd".to_owned(),
        }
    }
}

pub struct TestFile {
    pub name: String,
    pub tests: Vec<Test>,
}

impl TestFile {
    pub fn new(path: &str) -> Self {
        let mut test_file = TestFile {
            name: path.to_owned(),
            tests: vec![],
        };
        let mut file = match File::open(path) {
            Ok(val) => val,
            Err(err) => panic!("Could not read \"{}\": {}", path, err),
        };
        let mut buff = String::new();

        if let Err(err) = file.read_to_string(&mut buff) {
            panic!("Could not read \"{}\": {}", path, err);
        }

        let raw_test_file: RawTestFile = match toml::from_str(&buff) {
            Ok(val) => val,
            Err(err) => panic!("Parsing error: {} in test file \"{}\"", err, path),
        };

        let default = match raw_test_file.default {
            Some(raw_defaults) => Defaults::new(raw_defaults),
            None => Defaults::default(),
        };

        for (test_name, test) in raw_test_file.test.into_iter() {
            let working_directory = PathBuf::from(path).parent().unwrap().join(
                test.working_dir
                    .unwrap_or_else(|| default.working_dir.clone()),
            );
            if !working_directory.exists() {
                panic!(
                    "Working directory \"{}\" does not exist",
                    working_directory.to_str().unwrap()
                );
            }
            test_file.tests.push(Test {
                name: test_name,
                cmd: test.cmd,
                stdin: test.stdin,
                stdout: test.stdout,
                stderr: test.stderr,
                exit_status: test.exit_status.unwrap_or(default.exit_status),
                timeout: test.timeout.unwrap_or(default.timeout),
                working_dir: working_directory,
                runs_on: test.runs_on.unwrap_or_else(|| default.runs_on.clone()),
                unix_shell: test
                    .unix_shell
                    .unwrap_or_else(|| default.unix_shell.clone()),
                windows_shell: test
                    .windows_shell
                    .unwrap_or_else(|| default.windows_shell.clone()),
            });
        }
        test_file
    }
}
