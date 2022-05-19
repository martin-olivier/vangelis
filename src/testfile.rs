use std::io::Read;
use crate::test::Test;

#[derive(serde::Deserialize)]
struct RawDefault {
    runs_on: Option<Vec<String>>,
    code: Option<i32>,
    timeout: Option<f32>,
}

#[derive(serde::Deserialize)]
struct RawTest {
    runs_on: Option<Vec<String>>,
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
    test: indexmap::IndexMap<String, RawTest>,
}

pub struct TestFile {
    pub name: String,
    pub tests: Vec<Test>,
}

fn get_config_from_file(path: &str) -> RawTestFile {
    let mut file = match std::fs::File::open(path) {
        Ok(val) => val,
        Err(err) => panic!("Could not read file \"{}\": {}", path, err)
    };
    let mut buff = String::new();
    match file.read_to_string(&mut buff) {
        Ok(_) => (),
        Err(err) => panic!("Could not read file \"{}\": {}", path, err)
    };
    match toml::from_str(&buff) {
        Ok(val) => val,
        Err(err) => panic!("Parsing error: {} in test file \"{}\"", err, path)
    }
}

struct Default {
    runs_on: Vec<String>,
    code: i32,
    timeout: f32,
}

fn get_defaults(default: Option<RawDefault>) -> Default {
    match default {
        Some(def) => {
            Default {
                runs_on: def.runs_on.unwrap_or(vec!["linux".to_owned(), "macos".to_owned(), "windows".to_owned()]),
                code: def.code.unwrap_or(0),
                timeout: def.timeout.unwrap_or(60.0)
            }
        },
        None => {
            Default {
                runs_on: vec!["linux".to_owned(), "macos".to_owned(), "windows".to_owned()],
                code: 0,
                timeout: 60.0
            }
        }
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
        runs_on: raw_test.1.runs_on.unwrap_or(default.runs_on.clone())
    }
}

fn parse_config(raw_test_file: RawTestFile, path: &str) -> TestFile {
    let mut test_file =  TestFile{name: path.to_owned(), tests: vec![]};
    let default = get_defaults(raw_test_file.default);
    for test in raw_test_file.test.into_iter() {
        test_file.tests.push(parse_test(test, &default));
    }
    test_file
}

impl TestFile {
    pub fn new(path: &str) -> Self {
        parse_config(get_config_from_file(path), path)
    }
}