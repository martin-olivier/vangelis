mod core;
mod menu;
mod test;
mod testfile;
mod tools;

fn main() {
    tools::set_panic_hook();
    std::process::exit(core::Core::new().run());
}