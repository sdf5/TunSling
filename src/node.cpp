#include "node.h"

#include <array>
#include "cSecretbox_wrapper.h"
#include "linuxtun.h"
#include "cAsio_udp.h"
#include "cSendmmsg_udp.h"
#include <thread>

void node::run() {
	std::array<unsigned char, crypto_secretbox_KEYBYTES> crypto_key;
	crypto_key.fill(0b10101010);
	assert(m_tun != nullptr);
	assert(m_io_service != nullptr);
	m_tun->set_ip(boost::asio::ip::address::from_string("fd44:1111:2222:3333:4444:5555:6666:7777"), 1500);
	while (true) {
		std::vector<unsigned char> buffer(9000);
		size_t tun_read_size = m_tun->read_from_tun(buffer.data(), buffer.size());
		if( m_thread_pool ) {
			m_thread_pool->addJob([=,buffer{move(buffer)}]() mutable {
					size_t encypted_message_size =
						m_crypto->encrypt(
							  buffer.data(), tun_read_size,
							  crypto_key.data(), crypto_key.size(),
							  buffer.data(), buffer.size());
					std::lock_guard<std::mutex> lg(m_udp_mutex);
					size_t udp_sended = m_udp->send(buffer.data(), encypted_message_size, m_dst_addr);
				});
		} else {
			size_t encypted_message_size =
				m_crypto->encrypt(
					  buffer.data(), tun_read_size,
					  crypto_key.data(), crypto_key.size(),
					  buffer.data(), buffer.size());
			size_t udp_sended = m_udp->send(buffer.data(), encypted_message_size, m_dst_addr);
		}
	}
}


