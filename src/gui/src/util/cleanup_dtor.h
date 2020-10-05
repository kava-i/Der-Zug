/**
 * @author ShadowItaly
 */
#include <functional>

/**
 * @brief A small cleanup class working with destructors and object livetime in
 * C++
 */
class CleanupDtor
{
  public:
    /**
     * @brief Registers the cleanup function to get executed at the end of
     * livetime
     *
     * @param func The function to execute at the end of object livetime
     */
    CleanupDtor(std::function<void()> func) {
      cleanup_ = func;
    }

    /**
     * @brief Executes the registered cleanup function
     */
    ~CleanupDtor() {
      cleanup_();
    }
  private:
    std::function<void()> cleanup_; ///< The function to execute on the end of object livetime
};

