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
        
        typedef struct _handle{
            std::string backend;
        }handle;

       
        class AU_API UDPServer{
        public:
            UDPServer(handle _handle){
                handle = _handle;
            }
            ~UDPServer(){
                
            }
        private:
            handle handle;
        };
        

        class AU_API UDPClient{
        public:
            UDPClient(handle _handle){
                handle = _handle;
            }
            ~UDPClient(){
                
            }
        private:
            handle handle;
        };

    } // namespace protoco
} // namespace au

#endif

#endif //__AU_PROTOCO_H__