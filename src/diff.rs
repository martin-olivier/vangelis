use crate::test::{Status, TestResult};

use colored::Colorize;

fn print_diff(got: String, expected: String) {
    for diff in diff::lines(got.as_str(), expected.as_str()) {
        match diff {
            diff::Result::Left(l) => println!("{} {}", " + ".bold().green().on_white(), l.green()),
            diff::Result::Both(l, _) => println!("{} {}", "   ".on_white(), l),
            diff::Result::Right(r) => println!("{} {}", " - ".bold().red().on_white(), r.red()),
        }
    }
}

pub fn shell(result: TestResult) {
    if result.status == Status::Passed || result.status == Status::Skipped {
        return;
    }

    println!("{}", " ^ ".bold().blue());
    println!(
        "{}\n{} {}\n{}",
        "   ".on_blue(),
        " > ".white().on_blue(),
        "[exit status]".blue(),
        "   ".on_blue()
    );
    print_diff(
        format!(
            "value = {}",
            if let Some(exit_status) = result.got_exit_status {
                exit_status.to_string()
            } else {
                "None".to_string()
            }
        ),
        format!("value = {}", result.expected_exit_status),
    );
    if let Some(expected_stdout) = result.expected_stdout {
        println!(
            "{}\n{} {}\n{}",
            "   ".on_blue(),
            " > ".white().on_blue(),
            "[stdout]".blue(),
            "   ".on_blue()
        );
        print_diff(result.got_stdout, expected_stdout);
    }
    if let Some(expected_stderr) = result.expected_stderr {
        println!(
            "{}\n{} {}\n{}",
            "   ".on_blue(),
            " > ".white().on_blue(),
            "[stderr]".blue(),
            "   ".on_blue()
        );
        print_diff(result.got_stderr, expected_stderr);
    }
}
