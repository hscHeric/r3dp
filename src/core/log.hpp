#pragma once

namespace r3dp::core {

#ifdef DEBUG
  #define LOG_MESSAGE( msg ) \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl;

  #define LOG_VAR( var )                                                                    \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << #var << " = " << var \
              << std::endl;

  #define LOG_VECTOR( vec )                                                            \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << #vec << " = ["; \
    for ( const auto &elem : vec ) {                                                   \
      std::cout << elem << " ";                                                        \
    }                                                                                  \
    std::cout << "]" << std::endl;

  #define LOG_ERR( msg ) \
    std::cerr << "[ERRO] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl;

#else
  #define LOG_MESSAGE( msg ) \
    do {                     \
    } while ( 0 )
  #define LOG_VAR( var ) \
    do {                 \
    } while ( 0 )
  #define LOG_VECTOR( vec ) \
    do {                    \
    } while ( 0 )
  #define LOG_ERR( msg ) \
    do {                 \
    } while ( 0 )
#endif

}  // namespace r3dp::core
