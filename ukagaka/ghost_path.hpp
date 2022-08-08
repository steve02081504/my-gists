#include <string>

[[nodiscard]] bool is_ghost_path(std::wstring path);
[[nodiscard]] bool path_in_ghost_dir(std::wstring path);
[[nodiscard]] std::wstring get_ghost_path(std::wstring path);
[[nodiscard]] std::wstring get_ghost_dir_name(std::wstring path);
