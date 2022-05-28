use colored::Colorize;
use std::io::Write;

static mut SHELL_SIZE: Option<usize> = None;

pub fn set_hooks() {
    ctrlc::set_handler(move || {
        show_cursor();
        std::process::exit(84);
    }).ok();

    std::panic::set_hook(Box::new(|err| {
        show_cursor();
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

pub fn get_shell_size() -> usize {
unsafe {
    if SHELL_SIZE.is_none() {
        SHELL_SIZE = match atty::is(atty::Stream::Stdout) {
            true  => Some(term_size::dimensions().unwrap_or((100, 20)).0),
            false => Some(100),
        };
    }
    SHELL_SIZE.unwrap()
}
}

pub fn center(text: String) -> String {
    format!("{}[{}]", std::iter::repeat(" ").take((get_shell_size() - text.len()) / 2).collect::<String>(), text)
}

pub fn get_padding(name: &str, duration: &str) -> String {
    std::iter::repeat(" ").take(get_shell_size() - (name.len() + 5 + duration.len())).collect::<String>()
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
            if dir.join("code").is_file() {
                Some("code".to_owned())
            } else {
                None
            }
        }).next()
    })
}