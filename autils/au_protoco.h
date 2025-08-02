#ifndef __AU_PROTOCO_H__
#define __AU_PROTOCO_H__

#include "au_common.h"

#ifdef USE_AU_PROTOCO

#ifndef USE_AU_THREAD
#error Must define USE_AU_THREAD if using protoco define.
#endif
#include "au_thread.h"

#include <string>

namespace au {
    namespace protoco{
        namespace global{
            
        } // namespace global
        
        typedef struct _udo_handle{
            std::string backend;
            std::string ip;
            uint32_t port;
        }udo_handle;

       
        class AU_API UDPServer: public thread::cppThread{
        public:
            UDPServer(udo_handle _handle): cppThread("udp_server"){
                handle = _handle;
            }
            ~UDPServer(){
                
            }
        private:
            udo_handle handle;
        };
        

        class AU_API UDPClient : public thread::cppThread{
        public:
            UDPClient(udo_handle _handle): cppThread("udp_client"){
                handle = _handle;
            }
            ~UDPClient(){
                
            }
        private:
            udo_handle handle;
        };

    } // namespace protoco
} // namespace au

#endif

#endif //__AU_PROTOCO_H__