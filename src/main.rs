mod args;
mod config;
mod core;
mod diff;
mod test;
mod tools;

use core::Core;

fn main() {
    tools::set_hooks();
    std::process::exit(Core::new().run());
}
