#include <lines/util/logger.hpp>

namespace lines {

Logger& DefaultLogger() {
    static thread_local Logger logger;
    return logger;
}

}  // namespace lines
