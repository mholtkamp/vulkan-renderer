#pragma once

#include <vector>

#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
        MessageBox(NULL, err_msg, err_class, MB_OK);                       \
        exit(1);                                                               \
    } while (0)

std::vector<char> ReadFile(const std::string& filename);