#ifndef _SC_FILE_UTILS_H_
#define _SC_FILE_UTILS_H_

#include <filesystem>
#include <string_view>
#include "logger.hpp"

extern logger::SimpleLogger s_logger;

namespace sc {

    namespace files {

    namespace fs = std::filesystem;

        class SFile {
        public:

            class LinesIterator {
            public:
                LinesIterator(const SFile* cu, int offset = 0) : m_cu(cu), m_offset(offset) {
                    auto [line, bytes_read] = m_cu->get_line(m_offset);
                    m_line = line;
                    m_offset += bytes_read + 1;
                }
                LinesIterator operator++() {
                    auto [line, bytes_read] = m_cu->get_line(m_offset);
                    m_line = line;
                    m_offset += bytes_read + 1;
                    return *this;
                }
                bool operator!=(const LinesIterator& other) const {
                    return this->m_offset != other.m_offset;
                }
                const std::string& operator*() const { return m_line; }
            private:
                const SFile* m_cu;
                int                m_offset;
                std::string        m_line;
            };

            SFile(std::string_view& fpath);
            SFile(const SFile& cu) = delete;
            SFile(SFile&& cu) noexcept:
                m_start_addr(cu.m_start_addr),
                m_file_path(std::move(cu.m_file_path)),
                m_length(cu.m_length) {
                    s_logger.log(logger::log_level_t::Info, "Calling move constuctor");
                    cu.m_start_addr = nullptr;
                    cu.m_length = 0;
                    s_logger.log(logger::log_level_t::Info, "Ending move constuctor");
                }

            ~SFile();

            LinesIterator begin() const { return LinesIterator(this); }
            LinesIterator end()   const { return LinesIterator(this, m_length); }

            /* 
             * Read memory starting from m_start_addr to offset and return number number of readed bytes with string.
             * @return tuple of string and read bytes count
             */
            std::tuple<std::string, int> get_line(int offset) const;

            SFile operator=(SFile const&) = delete;
        private:
            char*       m_start_addr;
            fs::path    m_file_path;
            size_t      m_length;
        };

    };
};

#endif /* _SC_FILE_UTILS_H_ */
