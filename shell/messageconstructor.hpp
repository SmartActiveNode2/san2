
#pragma once

#include <iterator>
#include <string>
#include <stdio.h>

#include "utils/cvector.hpp"

bool enc_construct_A_message(const std::string &username, const San2::Utils::bytes &srpA, San2::Utils::bytes &out);
bool enc_parse_A_message(const San2::Utils::bytes &messageIn, San2::Utils::bytes &username, San2::Utils::bytes &srpA);
bool enc_testA();

