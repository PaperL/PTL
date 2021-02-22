#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {
    class exception {
    protected:
        const std::string variant = "";
        std::string detail = "";
    public:
        exception() {}

        exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}

        virtual std::string what() {
            return variant + " " + detail;
        }
    };

    class index_out_of_bound : public exception {
    public:
        index_out_of_bound() { detail = "index_out_of_bound"; }
        /* __________________________ */
    };

    class runtime_error : public exception {
    public:
        runtime_error() { detail = "runtime_error"; }
        /* __________________________ */
    };

    class invalid_iterator : public exception {
    public:
        invalid_iterator() { detail = "invalid_iterator"; }
        /* __________________________ */
    };

    class container_is_empty : public exception {
    public:
        container_is_empty() { detail = "container_is_empty"; }
        /* __________________________ */
    };

/*
    class exception {
    protected:
        const std::string variant;
        std::string detail;
    public:
        exception() : variant("Exception"), detail("No Error Information") {}

        exception(const std::string &var, const std::string &det) : variant(var), detail(det) {}

        virtual std::string what() {
            return variant + ": " + detail;
        }
    };

    class index_out_of_bound : public exception {
    public:
        explicit index_out_of_bound(const std::string &msg = "No Detailed Information")
                : exception("Index out of Bound", msg) {}
    };

    class runtime_error : public exception {
    public:
        explicit runtime_error(const std::string &msg = "No Detailed Information")
                : exception("Runtime Error", msg) {}
    };

    class invalid_iterator : public exception {
    public:
        explicit invalid_iterator(const std::string &msg = "No Detailed Information")
                : exception("Invalid Iterator", msg) {}
    };

    class container_is_empty : public exception {
    public:
        explicit container_is_empty(const std::string &msg = "No Detailed Information")
                : exception("Container is Empty", msg) {}
    };
*/

}

#endif
