mod core;
mod menu;
mod test;
mod config;
mod diff;
mod tools;

fn main() {
    tools::set_panic_hook();
    std::process::exit(core::Core::new().run());
}