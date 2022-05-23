use colored::Colorize;
use std::io::Write;
use crate::tools;

pub fn set_panic_hook() {
    std::panic::set_hook(Box::new(|err| {
        tools::show_cursor();
        if let Some(msg) = err.payload().downcast_ref::<&str>() {
            println!("{}", msg.bold().red());
        } else if let Some(msg) = err.payload().downcast_ref::<String>() {
            println!("{}", msg.bold().red());
        } else {
            println!("{}", err);
        }
        std::process::exit(84);
    }));
}

pub fn hide_cursor() {
    if atty::is(atty::Stream::Stdout) {
        print!("\x1b[?25l");
        std::io::stdout().flush().unwrap();
    }
}

pub fn show_cursor() {
    if atty::is(atty::Stream::Stdout) {
        print!("\x1b[?25h");
        std::io::stdout().flush().unwrap();
    }
}

pub fn get_padding(name: &str, duration: &str) -> String {
    let termsize: usize = match atty::is(atty::Stream::Stdout) {
        true => term_size::dimensions().unwrap_or((100, 20)).0,
        false => 100,
    };
    std::iter::repeat(" ").take(termsize - (name.len() + 5 + duration.len())).collect::<String>()
}

pub fn get_vscode_bin() -> Option<String> {
    if cfg!(target_os = "macos") {
        let vscode_path = "/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code";
        if std::path::Path::new(vscode_path).is_file() {
            return Some(vscode_path.to_owned());
        }
    }
    std::env::var_os("PATH").and_then(|paths| {
        std::env::split_paths(&paths).filter_map(|dir| {
            let full_path = dir.join("code");
            if full_path.is_file() {
                Some("code".to_owned())
            } else {
                None
            }
        }).next()
    })
}