mod core;
mod menu;
mod test;
mod testfile;
mod tools;

fn main() {
    tools::set_panic_hook();
    tools::hide_cursor();
    let ret = core::Core::new().run();
    tools::show_cursor();
    std::process::exit(ret);
}