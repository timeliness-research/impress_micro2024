#include "logging.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

namespace
{
void make_dir(const std::string& path) { mkdir(path.c_str(), 0755); }
} // namespace

JsonWriter::JsonWriter(const std::string& path) : out_(path) { begin_object(); }

JsonWriter::~JsonWriter() { close(); }

void JsonWriter::close()
{
  while (!need_comma_.empty())
    end_object();
  if (out_.is_open())
    out_.close();
}

void JsonWriter::write_key(const std::string& key)
{
  if (!need_comma_.empty()) {
    if (need_comma_.back())
      out_ << ",\n";
    need_comma_.back() = true;
  }
  if (!key.empty())
    out_ << '"' << key << "\": ";
}

void JsonWriter::begin_object(const std::string& key)
{
  write_key(key);
  out_ << "{\n";
  need_comma_.push_back(false);
}

void JsonWriter::end_object()
{
  out_ << "\n}";
  need_comma_.pop_back();
}

void JsonWriter::field(const std::string& key, const std::string& value)
{
  write_key(key);
  out_ << '"' << value << '"';
}

void JsonWriter::field(const std::string& key, const char* value) { field(key, std::string(value)); }

void JsonWriter::field(const std::string& key, bool value)
{
  write_key(key);
  out_ << (value ? "true" : "false");
}

void JsonWriter::array_field(const std::string& key, const uint64_t* values, std::size_t count)
{
  write_key(key);
  out_ << '[';
  for (std::size_t i = 0; i < count; i++) {
    if (i)
      out_ << ',';
    out_ << values[i];
  }
  out_ << ']';
}

std::string create_run_log_dir(const std::string& executable_path)
{
  std::string exe_dir = ".";
  std::string exe_name = executable_path;
  auto slash = executable_path.find_last_of('/');
  if (slash != std::string::npos) {
    exe_dir = executable_path.substr(0, slash);
    exe_name = executable_path.substr(slash + 1);
  }

  std::string logs_root = exe_dir + "/logs";
  make_dir(logs_root);

  std::time_t now = std::time(nullptr);
  char timestamp[32];
  std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));

  std::ostringstream run_dir;
  run_dir << logs_root << "/" << exe_name << "_" << timestamp << "_" << getpid();
  make_dir(run_dir.str());

  return run_dir.str();
}

std::string create_heartbeat_log_dir(const std::string& run_log_dir, uint32_t cpu, uint64_t heartbeat_index)
{
  std::ostringstream dir;
  dir << run_log_dir << "/cpu" << cpu << "_heartbeat_" << std::setw(6) << std::setfill('0') << heartbeat_index;
  make_dir(dir.str());
  return dir.str();
}
