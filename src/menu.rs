pub fn help(return_value: i32) {
    println!("USAGE:");
    println!("\t{} test.toml [OPTIONS]\n", std::env::args().nth(0).unwrap_or("io_tester".to_string()));

    println!("DESCRIPTION:");
    println!("\ttest.toml\t\tfile that contains functional tests\n");

    println!("OPTIONS:");
    println!("\t--help\tDisplay this help menu");
    println!("\t--version\tDisplay the current version");
    println!("\t--changelog\tDisplay the changelog");
    println!("\t--verbose\tDisplay the output difference in the shell");
    println!("\t--diff\t\tDisplay the output difference in Visual Studio Code\n");

    println!("RETURN VALUE:");
    println!("\t0\t\tif all tests succeed");
    println!("\t1\t\tif one or more tests failed or crashed");
    println!("\t84\t\tif IO_Tester panic during execution");

    std::process::exit(return_value);
}

pub fn version() {
    println!("IO_Tester (2.0.0)");
    println!("Repository link: https://github.com/martin-olivier/IO-TESTER");

    std::process::exit(0);
}

pub fn changelog() {
    println!("Changelog:");

    std::process::exit(0);
}