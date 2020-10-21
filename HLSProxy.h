#ifndef __HLS_PROXY_H__
#define __HLS_PROXY_H__

#include "HttpProxy.h"

class HLSProxy : public HttpProxy
{
public:

  // Create a new Proxy for HLS video streaming connected to the specified remote host/port
  HLSProxy(const std::string& host, int port);
  virtual ~HLSProxy();

  HLSProxy(const HLSProxy&) = delete;
  HLSProxy& operator=(const HLSProxy&) = delete;

protected:

  enum class HLSType { UNKNOWN, MANIFEST, SEGMENT };

  virtual bool processRequest(HttpStream& req, Socket& server) override;
  virtual bool processResponse(HttpStream& res, Socket& client) override;

  bool processManifest(HttpStream& res, Socket& client) const;

  static HLSType getHLSType(const std::string& filename);
  static const char* hlstypeToString(HLSType type);

};

#endif // __HLS_PROXY_H__
