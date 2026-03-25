#include <cstddef>
#include <span>
#include <variant>

using Pixel = int;

template <typename T, typename E> using Result = std::variant<T, E>;

class DrawError
{
};

template <typename Context> class Drawable
{
  public:
	virtual Result<size_t, DrawError> draw(Context const& ctx, std::span<Pixel> buf) = 0;
	virtual ~Drawable()																 = default;
};
