#include <sstream> 
#include <algorithm> 
#include <string> 
#include <iostream> 
#include "Mapi/sampa_config.h"


void SampaCmdQueue::addCommand(sampa_config_cmd_t& cmd) {
  lock.lock();
  stack.push_back(cmd);
  std::cout<<"pushed command to queue: "
      <<std::get<0>(cmd)<<" "
      <<std::get<1>(cmd)<<" "
      <<std::get<2>(cmd)<<" "
      <<std::get<3>(cmd)<<" "
      <<std::get<4>(cmd)<<" "
      <<std::endl;
  lock.unlock();
}
bool SampaCmdQueue::getNextCommand(sampa_config_cmd_t& cmd) {
  bool result = false;
  lock.lock();
  if( !stack.empty() ) {
    cmd = stack.front();
    stack.pop_front();
    std::cout<<"popped command to queue: "
        <<std::get<0>(cmd)<<" "
        <<std::get<1>(cmd)<<" "
        <<std::get<2>(cmd)<<" "
        <<std::get<3>(cmd)<<" "
        <<std::get<4>(cmd)<<" "
        <<std::endl;
    result = true;
  }
  lock.unlock();
  return result;
}
SampaConfig::SampaConfig(): request_trid_(100) {
    //your constructor
}
std::string SampaConfig::readByte(uint32_t daddr, uint32_t raddr) {
  const sca_rgf_t& rgf = I2C_S_10B_R;
  uint32_t data = addressToD1D0(daddr, raddr);
  HdlcEcFramePayload request{request_trid_, group+3, std::get<1>(rgf), {std::get<0>(rgf)}, adjustByteOrder(rgf, data)};
  request_trid_ = (request_trid_ % 0xfe) + 1;
  uint32_t wdata, wcmd;
  assembleRegisters(request, wcmd, wdata);
  char tstr[500];
  snprintf(tstr,499,"%08X,%08X",wcmd,wdata);
  std::string output = tstr;
  printf("SampaConfig::processInputMessage: output=\"%s\"\n", output.c_str());
  //output.erase(remove(output.begin(), output.end(), '\n'), output.end());
  return output; //send message to the ALF
}
std::string SampaConfig::writeByte(uint32_t daddr, uint32_t raddr, uint8_t value) {
  const sca_rgf_t& rgf = I2C_S_10B_W;
  uint32_t data = (uint32_t(value) << 16) | addressToD1D0(daddr, raddr);
  HdlcEcFramePayload request{request_trid_, group+3, std::get<1>(rgf), {std::get<0>(rgf)}, adjustByteOrder(rgf, data)};
  request_trid_ = (request_trid_ % 0xfe) + 1;
  uint32_t wdata, wcmd;
  assembleRegisters(request, wcmd, wdata);
  char tstr[500];
  snprintf(tstr,499,"%08X,%08X",wcmd,wdata);
  std::string output = tstr;
  printf("SampaConfig::processInputMessage: output=\"%s\"\n", output.c_str());
  //output.erase(remove(output.begin(), output.end(), '\n'), output.end());
  return output; //send message to the ALF
}
string SampaConfig::writeGlobalRegister() {
  string output;
  std::string cmd1 = writeByte(chip, reg, value);
  std::string cmd2 = readByte(chip, reg);
  //output = cmd1 + "\n" + cmd2 + "\n" + cmd3 + "\n" + cmd4;
  output = cmd1 + "\n" + cmd2;
  printf("SampaConfig::writeGlobalRegister: output=\"%s\"\n", output.c_str());
  return output;
}
string SampaConfig::checkGlobalRegister(string output, const sampa_config_cmd_t& cmd) {
  std::istringstream f(output);
  std::string line;
  uint32_t w1, w2;
  HdlcEcFramePayload reply;
  int trid = std::get<0>(cmd);
  int chip = std::get<1>(cmd);
  int chan = std::get<2>(cmd);
  int reg = std::get<3>(cmd);
  int value = std::get<4>(cmd);
  int group = chip / 10;
  chip = chip % 10;
  // 1st line
  if( !std::getline(f, line) ) return "failure";
  if( line != "success" ) return "failure";
  // 2nd line
  if( !std::getline(f, line) ) return "failure";
  sscanf(line.c_str(),"%X,%X", &w1, &w2);
  disassembleRegisters(reply, w1, w2);
  printf("line=\"%s\" w1=%08X w2=%08X channel=%d error=%d data=%08X\n",
      line.c_str(), (int)w1, (int)w2, (int)reply.channel, (int)reply.error, (int)reply.data);
  //std::cout << line << " " << w1 << " " << w2 << " channel="<<r.channel<<" error="<<r.error<<std::endl;
  if( (reply.trid) != trid ) return "failure";
  if( (reply.channel) != group+3 ) return "failure";
  if( (reply.data & 0xFF) != 0x04 ) return "failure";
  // Exception for soft-reset command
  if( (reg != 0x0e) || (value != 0x05) ) {
    // 3rd line
    if( !std::getline(f, line) ) return "failure";
    sscanf(line.c_str(),"%X,%X", &w1, &w2);
    printf("line=\"%s\" w1=%08X w2=%08X channel=%d error=%d data=%08X\n",
        line.c_str(), (int)w1, (int)w2, (int)reply.channel, (int)reply.error, (int)reply.data);
    //std::cout << line << " " << w1 << " " << w2 << std::endl;
    disassembleRegisters(reply, w1, w2);
    if( (reply.channel) != group+3 ) return "failure";
    if( (reply.data & 0xFF) != 0x04 ) return "failure";
    uint32_t retvalue = (reply.data >> 8) & 0xFF;
    std::cout<<"value="<<value<<" retvalue="<<retvalue<<std::endl;
    if( value != retvalue ) return "failure";
  }
  string response = "success";
  return response; //send answer back to WinCC
}
string SampaConfig::writeChannelRegister() {
  string output;
  std::string cmd1 = writeByte(chip, 0x17, reg);
  std::string cmd2 = writeByte(chip, 0x18, value & 0xFF);
  std::string cmd3 = writeByte(chip, 0x19, (value>>8) & 0xFF);
  std::string cmd4 = writeByte(chip, 0x1A, ((chan) & 0x1F) + 0x40);
  std::string cmd5 = writeByte(chip, 0x17, reg);
  std::string cmd6 = writeByte(chip, 0x1A, (chan) & 0x1F);
  std::string cmd7 = readByte(chip, 0x1B);
  std::string cmd8 = readByte(chip, 0x1C);
  //output = cmd1 + "\n" + cmd2 + "\n" + cmd3 + "\n" + cmd4;
  output = cmd1 + "\n" + cmd2 + "\n" + cmd3 + "\n" + cmd4 + "\n" + cmd5 + "\n"
      + cmd6 + "\n" + cmd7 + "\n" + cmd8;
  printf("SampaConfig::writeGlobalRegister: output=\"%s\"\n", output.c_str());
  return output;
}
string SampaConfig::checkChannelRegister(string output, const sampa_config_cmd_t& cmd) {
  std::istringstream f(output);
  std::string line;
  uint32_t w1, w2;
  HdlcEcFramePayload reply;
  int trid = std::get<0>(cmd);
  int chip = std::get<1>(cmd);
  int chan = std::get<2>(cmd);
  int reg = std::get<3>(cmd);
  int value = std::get<4>(cmd);
  int group = chip / 10;
  chip = chip % 10;
  // 1st line
  if( !std::getline(f, line) ) return "failure";
  if( line != "success" ) return "failure";
  for(int i = 0; i < 6; i++) {
    if( !std::getline(f, line) ) return "failure";
    sscanf(line.c_str(),"%X,%X", &w1, &w2);
    disassembleRegisters(reply, w1, w2);
    printf("line=\"%s\" w1=%08X w2=%08X channel=%d error=%d data=%08X\n",
        line.c_str(), (int)w1, (int)w2, (int)reply.channel, (int)reply.error, (int)reply.data);
    //std::cout << line << " " << w1 << " " << w2 << " channel="<<r.channel<<" error="<<r.error<<std::endl;
    if( i == 0 ) {
      if( (reply.trid) != trid ) return "failure";
    }
    if( (reply.channel) != group+3 ) return "failure";
    if( (reply.data & 0xFF) != 0x04 ) return "failure";
  }
  uint32_t retvalue = 0;
  // 1st data word
  if( !std::getline(f, line) ) return "failure";
  sscanf(line.c_str(),"%X,%X", &w1, &w2);
  printf("line=\"%s\" w1=%08X w2=%08X channel=%d error=%d data=%08X\n",
      line.c_str(), (int)w1, (int)w2, (int)reply.channel, (int)reply.error, (int)reply.data);
  //std::cout << line << " " << w1 << " " << w2 << std::endl;
  disassembleRegisters(reply, w1, w2);
  if( (reply.channel) != group+3 ) return "failure";
  if( (reply.data & 0xFF) != 0x04 ) return "failure";
  retvalue += (reply.data >> 8) & 0xFF;
  // 2nd data word
  if( !std::getline(f, line) ) return "failure";
  sscanf(line.c_str(),"%X,%X", &w1, &w2);
  printf("line=\"%s\" w1=%08X w2=%08X channel=%d error=%d data=%08X\n",
      line.c_str(), (int)w1, (int)w2, (int)reply.channel, (int)reply.error, (int)reply.data);
  //std::cout << line << " " << w1 << " " << w2 << std::endl;
  disassembleRegisters(reply, w1, w2);
  if( (reply.channel) != group+3 ) return "failure";
  if( (reply.data & 0xFF) != 0x04 ) return "failure";
  retvalue += (((reply.data >> 8) & 0xFF) << 8) & 0xFF00;
  std::cout<<"value="<<value<<" retvalue="<<retvalue<<std::endl;
  if( value != retvalue ) return "failure";
  string response = "success";
  return response; //send answer back to WinCC
}
string SampaConfig::processInputMessage(string input) {
    //input - message received from DIM command
    // string request = "12345678";
    string output = input;
    //std::cout<<"SampaConfig::processInputMessage: input=\""<<input<<"\"\n";
    printf("SampaConfig::processInputMessage: input=\"%s\"\n", input.c_str());
    //istringstream istr(input);
    //istr >> chip >> chan >> reg >> value;
    //if( !istr ) {
    if( sscanf(input.c_str(), "%i %i %i %i", &chip, &chan, &reg, &value) != 4 ) {
      printf("SampaConfig::processInputMessage: failed parsing input\n");
      group = chip = chan = reg = value = -1;
      return input;
    }
    sampa_config_cmd_t cmd =
        std::make_tuple(request_trid_, (int)chip, (int)chan, (int)reg, (int)value);
    queue.addCommand(cmd);
    group = chip / 10;
    chip = chip % 10;
    printf("SampaConfig::processInputMessage: group=%d, chip=%d, chan=%d, reg=%d, value=%d\n",
        group, chip, chan, reg, value);
    if( chan < 0 ) output = writeGlobalRegister();
    else output = writeChannelRegister();
    return output; //send message to the ALF
}
string SampaConfig::processOutputMessage(string output) {
  //output - message received from the ALF
  printf("SampaConfig::processOutputMessage: output=\"%s\"\n", output.c_str());
  sampa_config_cmd_t cmd;
  if( !queue.getNextCommand(cmd) ) return "failure";
  if( chan < 0 ) return checkGlobalRegister(output, cmd);
  else return checkChannelRegister(output, cmd);
}
