#include "compile_unit.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>


using namespace lexer;

logger::SimpleLogger s_logger;

CompileUnit::CompileUnit(std::string_view& fpath) : m_file_path(fpath) {
    s_logger.log(logger::log_level_t::Info, "Calling Simple constructor");

    s_logger.log(logger::Info, m_file_path.string());
    int fd = open(m_file_path.c_str(), O_RDONLY);
    if (fd == -1) {
        s_logger.log(logger::Error, "Cannot get fd");
        throw std::runtime_error("Cannot get fd");
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        s_logger.log(logger::Error, "Error during stat");
        throw std::runtime_error("Error during stat");
    }

    m_length = sb.st_size;
    m_start_addr = static_cast<char *>(mmap(NULL, m_length, PROT_READ, MAP_PRIVATE, fd, 0));
    if (m_start_addr == MAP_FAILED) {
        s_logger.log(logger::Error, "Cannot mmap");
        throw std::runtime_error("Cannot mmap");
    }
    s_logger.log(logger::log_level_t::Info, "Ending Simple constructor");
    
}

std::tuple<std::string, int> CompileUnit::get_line(int offset) const {
    auto start_address = m_start_addr + offset;
    auto last_addr = m_start_addr + m_length;
    int bytes_read = 0;
    char *str_end = nullptr;
    std::string res = "";
    if (m_length >= offset) {
        if ((str_end = static_cast<char*>(memchr(start_address, '\n', m_length - offset)))) {
            bytes_read = str_end - start_address; 
            res = std::string(start_address, str_end);
        }
    }
    else {
        throw std::runtime_error("Offset exceeds file size");
    }
    return std::make_tuple(res, bytes_read);
}

CompileUnit::~CompileUnit() {
    s_logger.log(logger::log_level_t::Info, "Calling destructor");
    if (m_start_addr) {
        if (munmap(m_start_addr, m_length) != 0) {
            s_logger.log(logger::Error, "Cannot munmap");
        }
    }
    s_logger.log(logger::log_level_t::Info, "Exiting destructor");
}
