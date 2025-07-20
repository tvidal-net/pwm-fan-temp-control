//
// Created by thiago on 20/07/25.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <ESPAsyncTCP.h>

class Network {
  std::vector<std::string> m_Commands;
  std::vector<AsyncClient*> m_Clients;
  AsyncServer m_Server;

  void clientConnected(AsyncClient& client);
  void clientReceived(AsyncClient& client, const std::string& data);
  void clientDisconnected(AsyncClient& client);

public:
  explicit Network();

  static
  IPAddress localIP();

  void send(float temp_c, float fan_pwm = 0.0f) const;

  static
  void connect();

  static
  void connect(const char* ssid, const char* psk);

  void startServer();

  std::string pop();
};

#endif //NETWORK_H
