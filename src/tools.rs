pub fn get_padding(name: &str, duration: &str) -> String {
    let termsize: usize = term_size::dimensions().unwrap().0;
    std::iter::repeat(" ").take(termsize - (name.len() + 4 + duration.len())).collect::<String>()
}

pub fn get_vscode_bin() -> Option<String> {
    if cfg!(target_os = "macos") {
        let vscode_path = "/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code";
        if std::path::Path::new(vscode_path).is_file() {
            return Some(vscode_path.to_owned());
        }
    }
    std::env::var_os("PATH").and_then(|paths| {
        std::env::split_paths(&paths).filter_map(|dir| {
            let full_path = dir.join("code");
            if full_path.is_file() {
                Some(full_path.to_str().unwrap().to_owned())
            } else {
                None
            }
        }).next()
    })
}