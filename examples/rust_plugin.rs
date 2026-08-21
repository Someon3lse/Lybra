use std::os::raw::c_char;

#[repr(C)]
pub enum PluginType {
    NONE = 0,
    SCRAPER = 1,
    AFTER_SEARCH = 2,
    AFTER_DOWNLOAD = 3,
}

#[repr(C)]
struct PluginInfo {
    name: *const c_char,
    author: *const c_char,
    version: [i32; 3],
    description: *const c_char,
    pub id: *const c_char,
    preference: i32,
    pub plugin_type: PluginType,
}

#[repr(C)]
struct HostAPI {
    pub pluginInit:  extern "C" fn(*const PluginInfo) -> i32,
    pub debug:       extern "C" fn(*const c_char, *const c_char),
    pub print:       extern "C" fn(*const c_char, *const c_char),
    pub warn:        extern "C" fn(*const c_char, *const c_char),
    pub error:       extern "C" fn(*const c_char, *const c_char),
    pub fatal:       extern "C" fn(*const c_char, *const c_char),
    pub apocalypse:  extern "C" fn(*const c_char, *const c_char),
    pub debugLevel:  i32,
    pub getPluginFn: extern "C" fn(*const [u8; 32], *const c_char),
}

#[no_mangle]
pub extern "C" fn pluginMain(api: *const HostAPI) -> i32 {
    let name = b"Rust Plugin\0".as_ptr() as *const c_char;
    let author = b"Rustacean\0".as_ptr() as *const c_char;
    let description = b"Plugin escrito en Rust\0".as_ptr() as *const c_char;

    let id = b"rust_in_peace".as_ptr() as *const c_char;

    let info = PluginInfo {
        name,
        author,
        version: [2, 1, 0],
        description,
        id,
        preference: -1,
        plugin_type: PluginType::NONE,
    };

    unsafe {
        let result = ((*api).pluginInit)(&info);
        if result != 0 {
            return result;
        }

        ((*api).debug)(
            b"Rust plugin listo!\0".as_ptr() as *const c_char,
            b"Rust Plugin\0".as_ptr() as *const c_char,
        );
    }

    0
}

//Build with: rustc --crate-type cdylib rust_plugin.rs
