#pragma once
#include <string>

[[nodiscard]] std::wstring GetSSPpath();
[[nodiscard]] bool IsSSPinstalled(std::wstring sspPath=GetSSPpath());
