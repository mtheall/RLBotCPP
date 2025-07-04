#pragma once

#include "Pool.h"

#include <corepacket_generated.h>
#include <interfacepacket_generated.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace rlbot::detail
{
/// @brief message
class Message
{
public:
	static constexpr std::size_t HEADER_SIZE = 2; ///< Size of message header

	~Message () noexcept;

	Message () noexcept;

	/// @brief Parameterized constructor
	/// @param buffer_ Buffer pool reference
	/// @param offset_ Offset into buffer where message header starts
	explicit Message (Pool<Buffer>::Ref buffer_, std::size_t offset_ = 0) noexcept;

	Message (Message const &that_) noexcept;

	Message (Message &&that_) noexcept;

	Message &operator= (Message const &that_) noexcept;

	Message &operator= (Message &&that_) noexcept;

	/// @brief bool cast operator
	/// Determines whether this message points into a valid buffer
	explicit operator bool () const noexcept;

	/// @brief Get message size (excluding header)
	unsigned size () const noexcept;

	/// @brief Get message size (including header)
	unsigned sizeWithHeader () const noexcept;

	/// @brief Get message span (including header)
	std::span<std::uint8_t const> span () const noexcept;

	/// @brief Get flatbuffer which points into this message
	/// @param verify_ Whether to verify contents
	/// @note Returns nullptr for invalid message
	rlbot::flat::InterfacePacket const *interfacePacket (bool verify_ = false) const noexcept;

	/// @brief Get flatbuffer which points into this message
	/// @param verify_ Whether to verify contents
	/// @note Returns nullptr for invalid message
	rlbot::flat::CorePacket const *corePacket (bool verify_ = false) const noexcept;

	/// @brief Get buffer reference
	Pool<Buffer>::Ref buffer () const noexcept;

	/// @brief Reset message
	/// This makes the message invalid and releases the underlying buffer
	void reset () noexcept;

private:
	/// @brief Referenced buffer
	Pool<Buffer>::Ref m_buffer;
	/// @brief Offset into buffer where message header starts
	std::size_t m_offset = 0;
};
}
