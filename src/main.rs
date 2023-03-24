mod config;
mod core;
mod diff;
mod menu;
mod test;
mod tools;

use crate::core::Core;

fn main() {
    tools::set_hooks();
    std::process::exit(Core::new().run());
}
