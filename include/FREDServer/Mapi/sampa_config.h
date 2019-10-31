#ifndef SAMPA_CONFIG_H
#define SAMPA_CONFIG_H

#include <list>
#include <utility>
#include <mutex>
#include <atomic>

#include <iomanip>
//#include "Fred/mapiinterface.h"
#include "Fred/Mapi/mapi.h"
#include "Fred/Mapi/mapigroup.h"


using sampa_config_cmd_t = std::tuple<int, int, int, int, int>;

class SampaCmdQueue
{
public:
  SampaCmdQueue() {}
    ~SampaCmdQueue() {lock.unlock();}


    void addCommand(sampa_config_cmd_t& cmd);
    bool getNextCommand(sampa_config_cmd_t& cmd);

private:
    mutex lock;
    list<sampa_config_cmd_t> stack;
};



//------------------------------------------------------------------------------
/** Common definitions related to HDLC EC communication
 */
class HdlcEcFramePayload
{
  public:    /** EC frame payload struct
     */
    uint8_t trid;
    uint8_t channel;
    uint8_t length;
    union {
      uint8_t command;
      uint8_t error;
    };
    uint32_t data;

    /** HDLC error field definitions, from GBT SCA manual v8.2, p18
     */
    enum error_field_t : uint8_t {
      ERR_GENERIC          = 0x01,
      ERR_INV_CHANNEL      = 0x02,
      ERR_INV_COMMAND      = 0x04,
      ERR_INV_TRID         = 0x08,
      ERR_INV_LENGTH       = 0x10,
      ERR_CHANNEL_DISABLED = 0x20,
      ERR_CHANNEL_BUSY     = 0x40,
      ERR_COMMAND_BUSYD    = 0x80
    };

    /** Print HdlcEcFramePayload objects with details
     */
    friend std::ostream& operator<< (std::ostream& out, const HdlcEcFramePayload& r) {
        out << std::setfill(' ') << std::right
            << "trid = "         << std::setw(3) << static_cast<uint32_t>(r.trid)    << ", "
            << "chan = "         << std::setw(3) << static_cast<uint32_t>(r.channel) << ", "
            << "len = "          << std::setw(3) << static_cast<uint32_t>(r.length)  << ", "
            << std::setfill('0')
            << "cmd/error = 0x"  << std::setw(2) << std::hex << static_cast<uint32_t>(r.command) << ", "
            << "data = 0x"       << std::setw(8) << std::hex << r.data
            << std::dec << std::left;
        return out;
      }

};


class SampaConfig: public Mapi
{
  int tid, group, chip, chan, reg, value;
  int request_trid_;

  /** Type to specify the byte order definition for SCA transceives
   *    - BO_NORMAL [31..0]
   *    - BO_REVERSED [7..0, 15..8, 23..16, 31..24]
   *    - BO_REVERSED_HW [15..8, 7..0, 31..24, 23..16]
   */
  enum sca_byteorder_t : uint8_t { BO_NORMAL, BO_REVERSED, BO_REVERSED_HW };

  /** Tuple for the SCA register file information\n
   *    - <0> TX command
   *    - <1> TX length
   *    - <2> RX length
   *    - <3> Byte order: BO_NORMAL, BO_REVERSED, BO_REVERSED_HW
   */
  using sca_rgf_t = std::tuple<uint8_t, uint8_t, uint8_t, sca_byteorder_t>;
  const sca_rgf_t I2C_S_10B_W {0x8a, 3, 2, BO_NORMAL};
  const sca_rgf_t I2C_S_10B_R {0x8e, 2, 2, BO_NORMAL};


  SampaCmdQueue queue;


  /** Reverse byte order
   *  @param a[31..0] input value
   *  @return a[7..0, ..., 31..24]
   */
  inline uint32_t rbo(uint32_t a) const
    {
      return ((a & 0xff) << 24) | ((a & 0xff00) << 8) |
             ((a & 0xff0000) >> 8) | ((a & 0xff000000) >> 24);
    }

  /** Reverse halfword order
   *  @param a[31..0] input value
   *  @return a[15..8, 7..0, 31..24, 23..16]
   */
  inline uint32_t rhwo(uint32_t a) const
    {
      return ((a & 0xffff) << 16) | ((a & 0xffff0000) >> 16);
    }

  /** Adjust data byte order according to register file definition
   *  @param rgf SCA register file definition
   *  @param data Input data
   *  @return Data with adjusted byte order
   */
  inline uint32_t adjustByteOrder(const sca_rgf_t& rgf, const uint32_t data) const
    {
      uint32_t adjusted_data;
      switch(std::get<3>(rgf)) {
        case BO_REVERSED:    adjusted_data = rbo(data); break;
        case BO_REVERSED_HW: adjusted_data = rhwo(data); break;
        case BO_NORMAL:
        default: adjusted_data = data; break;
      }
      return adjusted_data;
    }

  /** Convert device addresses to D0/D1 data field contents
   *  @param daddr Slave device address
   *  @param raddr Slave register address
   *  @return Value of the 'D1[7..0] & D0[7..0]' field as '[15..8] & [7..0]'
   */
  inline uint16_t addressToD1D0(uint32_t daddr, uint32_t raddr) const
    {
      return static_cast<uint16_t>(
             (((daddr & 0x3) << 14) | ((raddr & 0x3f) << 8)) | // D1
              (0x78 | ((daddr >> 2) & 0x3))                    // D0
             );
    }

  /** Transfer payload struct content to write register
   *    and adjust to correct format for this core
   */
  inline void
  assembleRegisters(const HdlcEcFramePayload& r, uint32_t& w2, uint32_t& w3) const {
      w2 = ((r.channel << 24) |
            (r.trid    << 16) |
            (r.length  << 8)  |
            (r.command << 0));
      w3 = ((r.data & 0xff)       << 24) |
           ((r.data & 0xff00)     << 8) |
           ((r.data & 0xff0000)   >> 8) |
           ((r.data & 0xff000000) >> 24);
    }

  /** Pack register read values into payload struct
   *    and adjust to correct format for this core
   */
  inline void
  disassembleRegisters(HdlcEcFramePayload& r, uint32_t w2, uint32_t w3) const {
      r.channel = (w2 >> 24) & 0xff;
      r.trid    = (w2 >> 16) & 0xff;
      r.length  = (w2 >> 8) & 0xff;
      r.error   = (w2 >> 0) & 0xff;
      r.data = ((w3 & 0xff)       << 24) |
               ((w3 & 0xff00)     << 8) |
               ((w3 & 0xff0000)   >> 8) |
               ((w3 & 0xff000000) >> 24);
    }


  std::string readByte(uint32_t daddr, uint32_t raddr);
  std::string writeByte(uint32_t daddr, uint32_t raddr, uint8_t value);

  string writeGlobalRegister();
  string checkGlobalRegister(string output, const sampa_config_cmd_t& cmd);

  string writeChannelRegister();
  string checkChannelRegister(string output, const sampa_config_cmd_t& cmd);

public:
    SampaConfig();

    string processInputMessage(string input);
    string processOutputMessage(string output);
};

#endif // MAPIEXAMPLE_H
