//
// Created by thiago on 20/07/25.
//
#include "Network.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <string>

#define TCP_PORT                  1080
#define BUFFER_SIZE               0x20

static
const char* SSID = WIFI_SSID;

static
const char* PSK = WIFI_PSK;

static
void print_client(AsyncClient& client) {
  Serial.print(client.remoteIP());
  Serial.print(':');
  Serial.print(client.remotePort());
}

void Network::clientConnected(AsyncClient& client) {
  const AcConnectHandler onDisconnect = [this](void*, const AsyncClient* it) {
    clientDisconnected(*it);
  };
  client.onDisconnect(onDisconnect, this);

  const AcDataHandler onData = [this](void*, AsyncClient* it, void* data, const size_t len) {
    const std::string s((char*)data, len);
    it->ack(len);
    clientReceived(*it, s);
  };
  client.onData(onData, this);

  m_Clients.push_back(&client);
}

void Network::clientReceived(AsyncClient& client, const std::string& data) {
  const char last = data[data.length() - 1];
  const auto cmd = last != '\n' ? data : data.substr(0, data.length() - 1);
  print_client(client);
  Serial.print(" command: ");
  Serial.println(cmd.c_str());

  if (!cmd.empty()) {
    m_Commands.insert(m_Commands.begin(), cmd);
    client.write("OK\n");
  }
}

void Network::clientDisconnected(const AsyncClient& client) {
  for (auto it = m_Clients.begin(); it != m_Clients.end(); ++it) {
    if (&client == *it) {
      m_Clients.erase(it);
      break;
    }
  }
}

Network::Network() :
  m_Server(TCP_PORT) {}

IPAddress Network::localIP() {
  return WiFi.localIP();
}

void Network::send(const float temp_c, const float fan_pwm) const {
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "T=%.1f,F=%.1f\n", temp_c, 100.0f * fan_pwm);

  const auto len = strlen(buffer);
  for (AsyncClient* client : m_Clients) {
    if (client->space() > len) {
      client->add(buffer, len);
      if (!client->send()) {
        client->close();
      }
    }
  }
}

void Network::connect() {
  connect(SSID, PSK);
}

void Network::connect(const char* ssid, const char* psk) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, psk);
}

bool Network::connected() {
    return WL_CONNECTED == status();
}

uint8_t Network::status() {
    return WiFi.status();
}

void Network::startServer() {
  connect();
  const AcConnectHandler onClient = [this](void*, AsyncClient* client) {
    clientConnected(*client);
  };
  m_Server.onClient(onClient, this);
  m_Server.begin();
}

std::string Network::pop() {
  if (m_Commands.empty()) {
    return "";
  }
  const auto last = m_Commands.back();
  m_Commands.pop_back();
  return last;
}
