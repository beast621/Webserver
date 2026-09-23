#pragma once

#include<string>
#include<map>
#include<cstddef>

std::string trim(const std::string& message);
bool parse_headers(
    const std::string& message,
    std::map<std::string,std::string>& headers
);

int parse_content_length(
    const std::string& value,
    std::size_t& length
);

std::string make_response(
    const std::string& status,
    const std::string& body,
    const std::string& content_type,
    bool head=false,
    const std::string& allow="GET, HEAD"
);

bool should_keep_alive(
    const std::string& version,
    const std::map<std::string,std::string>& headers
);

