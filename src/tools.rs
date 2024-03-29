use colored::Colorize;
use std::io::Write;

pub fn set_hooks() {
    ctrlc::set_handler(move || {
        show_cursor();
        std::process::exit(84);
    })
    .ok();

    std::panic::set_hook(Box::new(|err| {
        show_cursor();
        if let Some(msg) = err.payload().downcast_ref::<&str>() {
            eprintln!("{}", msg.bold().red());
        } else if let Some(msg) = err.payload().downcast_ref::<String>() {
            eprintln!("{}", msg.bold().red());
        } else {
            eprintln!("{}", err);
        }
        std::process::exit(84);
    }));
}

pub fn hide_cursor() {
    if atty::is(atty::Stream::Stdout) {
        print!("\x1b[?25l");
        std::io::stdout().flush().ok();
    }
}

pub fn show_cursor() {
    if atty::is(atty::Stream::Stdout) {
        print!("\x1b[?25h");
        std::io::stdout().flush().ok();
    }
}

pub fn get_shell_size() -> usize {
    match atty::is(atty::Stream::Stdout) {
        true => term_size::dimensions().unwrap_or((100, 20)).0,
        false => 100,
    }
}

pub fn center(text: &str) -> String {
    format!(
        "{}[{}]",
        " ".repeat((get_shell_size() - text.len()) / 2),
        text
    )
}

pub fn get_padding(name: &str, duration: &str) -> String {
    " ".repeat(get_shell_size() - (name.len() + 5 + duration.len()))
}
