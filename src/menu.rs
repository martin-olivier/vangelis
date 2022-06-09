pub fn help(return_value: i32) {
    println!("USAGE:");
    println!("\t{} [TESTFILES] [OPTIONS]\n", std::env::args().nth(0).unwrap_or("vangelis".to_string()));

    println!("TESTFILES:");
    println!("\tpath to one or multiple test files\n");

    println!("OPTIONS:");
    println!("\t--help\t\tDisplay this help menu");
    println!("\t--version\tDisplay the current version");
    println!("\t--changelog\tDisplay the changelog");
    println!("\t--verbose\tDisplay the output difference in the shell");
    println!("\t--diff\t\tDisplay the output difference in Visual Studio Code");
    println!("\t--stop_on_failure\tStop the execution if a test didn't succeed\n");

    println!("RETURN VALUE:");
    println!("\t0\t\tif all tests succeed");
    println!("\t1\t\tif one or more tests failed or crashed");
    println!("\t84\t\tif a critical error occured");

    std::process::exit(return_value);
}

pub fn version() {
    println!("Vangelis v2.0.0");
    println!("Copyright (c) 2022 Martin Olivier");
    println!("Repository link: https://github.com/martin-olivier/Vangelis");

    std::process::exit(0);
}

pub fn changelog() {
    println!("Changelog: https://github.com/martin-olivier/Vangelis/releases");

    std::process::exit(0);
}

pub fn tears() {
    println!("I've seen things you people wouldn't believe");
    println!("Attack ships on fire off the shoulder of Orion");
    println!("I watched C-beams glitter in the dark near the Tannhäuser Gate");
    println!("All those moments will be lost in time, like tears in rain");
    println!("\nRest in peace Vangelis (1943 - 2022)");

    std::process::exit(0);
}