use clap::Parser;

#[derive(Parser, Debug)]
#[clap(version, about, long_about = None)]

pub struct Args {
    /// Path to one or multiple test file(s) to be executed
    #[clap(required = true)]
    pub test_file: Vec<String>,

    /// Display the got/expected difference for each failed test in the shell
    #[clap(long, value_parser)]
    pub diff: bool,

    /// Stop the execution and display the got/expected difference if a test didn't succeed
    #[clap(long, value_parser)]
    pub ci: bool,
}
