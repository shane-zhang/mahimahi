/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string>

#include "config.h"

#ifndef APACHE_CONFIGURATION_HH
#define APACHE_CONFIGURATION_HH

//const std::string apache_main_config = "LoadModule mpm_event_module " + std::string( MOD_MPM_EVENT ) + "\nLoadModule http2_module " + std::string( MOD_HTTP2 ) + "\nProtocols h2 h2c" + "\nLoadModule authz_core_module " + std::string( MOD_AUTHZ_CORE ) + "\nMutex pthread\nLoadFile " + std::string( MOD_DEEPCGI ) + "\nLoadModule deepcgi_module " + std::string( MOD_DEEPCGI ) + "\nSetHandler deepcgi-handler\n";

const std::string apache_main_config = "LoadModule mpm_event_module " + std::string( MOD_MPM_EVENT ) + "\nThreadsPerChild  5\nServerLimit  2\nAsyncRequestWorkerFactor  2\nMaxRequestWorkers  10"  + "\nLoadModule http2_module " + std::string( MOD_HTTP2 ) + "\nProtocols h2 h2c" + "\nLoadModule authz_core_module " + std::string( MOD_AUTHZ_CORE ) + "\nMutex pthread\nLoadFile " + std::string( MOD_DEEPCGI ) + "\nLoadModule deepcgi_module " + std::string( MOD_DEEPCGI ) + "\nSetHandler deepcgi-handler\n";

const std::string apache_ssl_config = "LoadModule ssl_module " + std::string( MOD_SSL ) + "\nSSLEngine on\nSSLCertificateFile      " + std::string( MOD_SSL_CERTIFICATE_FILE ) + "\nSSLCertificateKeyFile " + std::string( MOD_SSL_KEY ) +"\n";

#endif /* APACHE_CONFIGURATION_HH */
