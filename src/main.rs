mod core;
mod menu;
mod config;
mod tools;

fn set_panic_hook() {
    std::panic::set_hook(Box::new(|err| {
        if let Some(msg) = err.payload().downcast_ref::<&str>() {
            println!("{}", msg);
        } else if let Some(msg) = err.payload().downcast_ref::<String>() {
            println!("{}", msg);
        } else {
            println!("{}", err);
        }
    }));
}

fn main() {
    set_panic_hook();
    std::process::exit(core::Core::new().run());
}