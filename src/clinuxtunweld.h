#ifndef CLINYXTUNWELD_H
#define CLINYXTUNWELD_H

#include "linuxtun.h"
#include <vector>

class cLinuxTunWeld : public iTun {
    public:
        cLinuxTunWeld(std::unique_ptr<boost::asio::posix::stream_descriptor> && stream);
        void set_ip(const boost::asio::ip::address & addr, uint32_t mtu) override;
        size_t read_from_tun(unsigned char * data, size_t data_size) override;
        size_t send_to_tun(const unsigned char * data, size_t data_size) override;
    private:
        linuxTun<> m_linux_tun;
        std::vector<unsigned char> m_weld;
        size_t m_read_ready_bytes_in_weld = 0;
        unsigned char * m_current_data_read_ready_ptr = nullptr;
        uint32_t m_mtu = 0;

        void clear_weld();
};

#endif // CLINYXTUNWELD_H
