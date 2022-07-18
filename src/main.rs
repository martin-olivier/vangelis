mod config;
mod core;
mod diff;
mod menu;
mod test;
mod tools;

fn main() {
    tools::set_hooks();
    std::process::exit(core::Core::new().run());
}