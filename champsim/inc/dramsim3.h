#ifndef __MEMORY_SYSTEM__H
#define __MEMORY_SYSTEM__H

#include <functional>
#include <string>

namespace dramsim3 {

// This should be the interface class that deals with CPU
class MemorySystem {
   public:
    MemorySystem(const std::string &config_file, const std::string &output_dir,
                 std::function<void(uint64_t)> read_callback,
                 std::function<void(uint64_t)> write_callback);
    ~MemorySystem();
    void ClockTick();
    void RegisterCallbacks(std::function<void(uint64_t)> read_callback,
                           std::function<void(uint64_t)> write_callback);
    void RegisterACTCallback(std::function<void(uint64_t, 
                                                uint64_t, 
                                                uint64_t,
                                                uint64_t, 
                                                uint64_t,
                                                uint64_t,
                                                uint64_t)> act_callback);
    double GetTCK() const;
    int GetBusBits() const;
    int GetBurstLength() const;
    int GetQueueSize() const;
    void PrintStats() const;
    void ResetStats();
    // Writes current cumulative stats into output_dir. Safe to call
    // repeatedly mid-simulation (e.g. once per heartbeat).
    void DumpStats(const std::string &output_dir);

    bool WillAcceptTransaction(uint64_t hex_addr, bool is_write) const;
    bool AddTransaction(uint64_t hex_addr, bool is_write, bool isRHMit);
    void iRFMToBank(int ba, int ra, int ch);
};

MemorySystem* GetMemorySystem(const std::string &config_file, const std::string &output_dir,
                 std::function<void(uint64_t)> read_callback,
                 std::function<void(uint64_t)> write_callback);
}  // namespace dramsim3

#endif
