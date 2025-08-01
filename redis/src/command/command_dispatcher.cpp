#include "command_dispatcher.hpp"

CommandDispatcher::CommandDispatcher()
{
  m_handlers["PING"] = [](const RESPValue &)
  {
    return "+PONG\r\n";
  };
  m_handlers["COMMAND"] = [](const RESPValue &)
  {
    return "*0\r\n";
  };
  m_handlers["ECHO"] = [](const RESPValue &value)
  {
    if (value.type != RESPValue::Type::Array ||
        value.array.size() != 2 ||
        value.array[1].type != RESPValue::Type::BulkString)
    {
      return std::string("-ERR wrong number of arguments for 'ECHO' command\r\n");
    }

    const std::string &msg = value.array[1].str;
    return "$" + std::to_string(msg.size()) + "\r\n" + msg + "\r\n";
  };
}

void CommandDispatcher::register_command(const std::string &cmd, Handler handler)
{
  m_handlers[to_upper(cmd)] = std::move(handler);
}

std::string CommandDispatcher::dispatch(const RESPValue &value) const
{
  if (value.type != RESPValue::Type::Array ||
      value.array.empty() ||
      value.array[0].type != RESPValue::Type::BulkString)
  {
    return "-ERR invalid request\r\n";
  }

  std::string cmd = to_upper(value.array[0].str);
  auto it = m_handlers.find(cmd);
  if (it != m_handlers.end())
  {
    return it->second(value);
  }

  return "-ERR unknown command\r\n";
}

std::string CommandDispatcher::to_upper(const std::string &s)
{
  std::string out = s;
  std::transform(out.begin(), out.end(), out.begin(), ::toupper);
  return out;
}