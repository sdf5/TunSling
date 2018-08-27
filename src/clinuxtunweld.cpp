#include "clinuxtunweld.h"
#include <algorithm>

cLinuxTunWeld::cLinuxTunWeld(std::unique_ptr<boost::asio::posix::stream_descriptor> && stream)
    :
      m_linux_tun(std::move(stream))
{
}

void cLinuxTunWeld::set_ip(const boost::asio::ip::address &addr, uint32_t mtu) {
    m_linux_tun.set_ip(addr, mtu);
    m_weld.resize(mtu * 10); // we can contain up to 10 packets
    clear_weld();
    m_mtu = mtu;
}

size_t cLinuxTunWeld::read_from_tun(unsigned char *data, size_t data_size) {
    if (data_size < m_weld.size())
        throw std::invalid_argument("data_size < " + std::to_string(m_weld.size()));
    size_t readed_bytes = m_linux_tun.read_from_tun(m_current_data_read_ready_ptr, m_read_ready_bytes_in_weld);
    // update weld status
    m_read_ready_bytes_in_weld -= readed_bytes;
    m_current_data_read_ready_ptr += readed_bytes;
    if (m_read_ready_bytes_in_weld < m_mtu) {
        const size_t readed_bytes_in_weld = m_weld.size() - m_read_ready_bytes_in_weld;
        std::copy_n(m_weld.begin(), readed_bytes_in_weld, data);
        clear_weld();
        return readed_bytes_in_weld;
    }
    return 0; // data readed to weld
}

size_t cLinuxTunWeld::send_to_tun(const unsigned char *data, size_t data_size) {
    return m_linux_tun.send_to_tun(data, data_size);
}

void cLinuxTunWeld::clear_weld() {
    m_read_ready_bytes_in_weld = m_weld.size();
    m_current_data_read_ready_ptr = &(m_weld[0]);
}
