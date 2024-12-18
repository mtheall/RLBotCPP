#pragma once

#include <flatbuffers/flatbuffer_builder.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace rlbot::detail
{
/// @brief Object pool
/// @tparam T Object type
template <typename T>
class Pool : public std::enable_shared_from_this<Pool<T>>
{
private:
	struct Private
	{
		explicit Private () noexcept;
	};

public:
	/// @brief Object reference
	class Ref
	{
	public:
		/// @brief Counted object reference
		struct Counted
		{
			/// @brief Reference
			T ref{};
			/// @brief Reference count
			std::atomic_uint count = 0;
#ifndef _WIN32
			/// @brief User-defined tag
			std::uintptr_t tag = 0;
			/// @brief Whether preferred
			bool preferred = false;
#endif
		};

		using CountedRef = std::shared_ptr<Counted>;

		~Ref () noexcept;

		Ref () noexcept;

		Ref (Ref const &that_) noexcept;

		Ref (Ref &&that_) noexcept;

		Ref &operator= (Ref const &that_) noexcept;

		Ref &operator= (Ref &&that_) noexcept;

		/// @brief Parameterized constructor
		/// @param pool_ Pool which will recycle this reference
		/// @param object_ Referenced object
		Ref (std::shared_ptr<Pool> pool_, CountedRef object_) noexcept;

		/// @brief bool cast operator
		/// Determines whether this reference points to an object
		explicit operator bool () const noexcept;

		/// @brief Member dereference operator
		T *operator->() noexcept;

		/// @brief Member dereference operator
		T const *operator->() const noexcept;

		/// @brief Object dereference operator
		T &operator* () noexcept;

		/// @brief Object dereference operator
		T const &operator* () const noexcept;

		/// @brief Resets reference
		/// This makes the reference invalid and releases the object into a pool
		void reset () noexcept;

#ifndef _WIN32
		/// @brief Get user-defined tag
		std::uintptr_t tag () const noexcept;

		/// @brief Set user-defined tag
		/// @param tag_ Tag to set
		void setTag (std::uintptr_t tag_) noexcept;

		/// @brief Whether this is a preferred object
		bool preferred () const noexcept;

		/// @brief Set whether this is a preferred object
		/// @param preferred_ Whether preferred
		void setPreferred (bool preferred_ = true) noexcept;
#endif

	private:
		/// @brief Object pool
		std::shared_ptr<Pool> m_pool;
		/// @brief Object reference
		CountedRef m_object;
	};

	~Pool () noexcept;

	/// @brief Parameterized constructor
	/// @param private_ Overload discriminator
	/// @param name_ Pool name
	/// @param reservations_ Initial capacity
	Pool (Private private_, std::string name_, unsigned reservations_) noexcept;

	/// @brief Create pool
	/// @param name_ Pool name
	/// @param reservations_ Number of preallocated objects
	static std::shared_ptr<Pool> create (std::string name_,
	    unsigned const reservations_ = 0) noexcept;

	/// @brief Get object from pool
	/// @note If pool is empty, a new object is constructed
	Ref getObject () noexcept;

	/// @brief Release object into pool
	/// @param object_ Object to put
	/// @note If this is the last reference, the object is recycled
	void putObject (Ref::CountedRef object_) noexcept;

private:
	/// @brief Mutex
	std::mutex m_mutex;
#ifndef _WIN32
	/// @brief Pool of unreferenced preferred objects
	std::vector<typename Ref::CountedRef> m_preferredPool;
#endif
	/// @brief Pool of unreferenced unpreferred objects
	std::vector<typename Ref::CountedRef> m_pool;
	/// @brief Pool name
	std::string const m_name;
	/// @brief Maximum size of pool
	std::size_t m_watermark = 0;
};

/// @brief Buffer size for buffer pool
/// @note Large enough to hold GamePacket+BallPrediction
constexpr auto BUFFER_SIZE = 2 * (std::numeric_limits<std::uint16_t>::max () + 1u);

using Buffer = std::array<std::uint8_t, BUFFER_SIZE>;

extern template class Pool<Buffer>;
extern template class Pool<flatbuffers::FlatBufferBuilder>;
}
