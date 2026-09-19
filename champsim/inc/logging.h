/*
 * Minimal structured-logging helpers: a tiny hand-rolled JSON writer (no
 * external dependency) plus helpers for laying out the per-run / per-heartbeat
 * log directory tree next to the ChampSim executable.
 */
#ifndef LOGGING_H
#define LOGGING_H

#include <cstdint>
#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

// Streams a well-formed JSON object to disk incrementally. Not thread-safe,
// not reusable across objects; construct one, build it top-to-bottom, and let
// it go out of scope (or call close()) to finish the file.
class JsonWriter
{
public:
  explicit JsonWriter(const std::string& path);
  ~JsonWriter();

  // Starts a nested object. Pass an empty key for the implicit root object.
  void begin_object(const std::string& key = "");
  void end_object();

  void field(const std::string& key, const std::string& value);
  void field(const std::string& key, const char* value);
  void field(const std::string& key, bool value);

  template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
  void field(const std::string& key, T value)
  {
    write_key(key);
    out_ << value;
  }

  // Writes a flat array of integers, e.g. a latency histogram.
  void array_field(const std::string& key, const uint64_t* values, std::size_t count);

  void close();

private:
  void write_key(const std::string& key);

  std::ofstream out_;
  std::vector<bool> need_comma_;
};

// Creates (if necessary) "<dir containing executable>/logs/<run_name>/" and
// returns its path. Safe to call once near the start of main().
std::string create_run_log_dir(const std::string& executable_path);

// Creates "<run_log_dir>/cpu<cpu>_heartbeat_<index>/" and returns its path.
std::string create_heartbeat_log_dir(const std::string& run_log_dir, uint32_t cpu, uint64_t heartbeat_index);

#endif
