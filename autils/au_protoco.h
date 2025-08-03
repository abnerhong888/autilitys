#ifndef __AU_PROTOCO_H__
#define __AU_PROTOCO_H__

#include "au_common.h"

#ifdef USE_AU_PROTOCO

#ifndef USE_AU_THREAD
#error Must define USE_AU_THREAD if using protoco define.
#endif

#include "au_thread.h"
#include "au_logger.h"
#include "au_ptr.h"

#include <string>

#if defined(__GNUC__)
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif //__GNUC__

namespace au {
    namespace protoco{
        namespace global{
            static const long udp_timeout_ms = 1;
        } // namespace global

        enum class eBackendType{
            UDP = 0,
        };

        typedef struct _handle{
            eBackendType backend;
            std::string ip;
            int port;
        }handle;

        
    } // namespace protoco
} // namespace au

namespace au {
    namespace protoco{
        namespace backend{
            class IBackend{
            public:
                IBackend() = default;
                virtual ~IBackend() = default;
                virtual int bind() {
                    log::console::Write(log::eLevel::warn, "IBackend::bind() not implemented.");
                    return -1;
                }

                virtual int recvfrom(void *__restrict __buf, size_t __n, int __flags,
	                                    __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len) {
                    log::console::Write(log::eLevel::warn, "IBackend::recvfrom() not implemented.");
                    return -1;
                }
                virtual int sendto(const void *__buf, size_t __n,
		                                int __flags, __CONST_SOCKADDR_ARG __addr, socklen_t __addr_len){
                    log::console::Write(log::eLevel::warn, "IBackend::sendto() not implemented.");
                    return -1;
                }

                AU_INLINE struct sockaddr_in get_sockaddr(protoco::handle handle) { 
                    struct sockaddr_in addr;
                    memset(&addr, 0, sizeof(addr));
                    addr.sin_family = AF_INET;
                    addr.sin_port = htons(handle.port);
                    // addr.sin_addr.s_addr = inet_addr(handle.ip.c_str());
                    inet_pton(addr.sin_family, handle.ip.c_str(), &addr.sin_addr);
                    return addr;
                };
                AU_INLINE struct sockaddr_in get_sockaddr(std::string ip, int port) { 
                    struct sockaddr_in addr;
                    memset(&addr, 0, sizeof(addr));
                    addr.sin_family = AF_INET;
                    addr.sin_port = htons(port);
                    //addr.sin_addr.s_addr = inet_addr(ip.c_str());
                    inet_pton(addr.sin_family, ip.c_str(), &addr.sin_addr);
                    return addr;
                };
            };

            class UDPBackend : public IBackend{
            public:
                UDPBackend(std::string _host, int _port){
                    int ret = 0;
                    AU_UNUSED(ret);
                    ret = init(_host, _port);
                    AU_ASSERT(ret == 0, "UDPBackend init failed.");
                }
                ~UDPBackend(){
                    close(sockfd);
                }

                AU_INLINE int init(std::string _host, int _port){
                    host = _host;
                    port = _port;
                    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
                    if (sockfd < 0){
                        return -1;
                    }
                    return 0;
                }


                AU_INLINE int bind() override{
                    struct sockaddr_in server_addr = get_sockaddr(host, port);
                    if(::bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
                        return -1;
                    }
                    return 0;
                }
                AU_INLINE int recvfrom(void *__restrict __buf, size_t __n, int __flags,
	                            __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len) {
                    struct timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = global::udp_timeout_ms * 1000;
                    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
                    return ::recvfrom(sockfd, __buf, __n, __flags, __addr, __addr_len);
                };
                AU_INLINE int sendto(const void *__buf, size_t __n,
		                        int __flags, __CONST_SOCKADDR_ARG __addr, socklen_t __addr_len){
                    return ::sendto(sockfd, __buf, __n, __flags, __addr, __addr_len);
                };
            private:
                int sockfd;
                std::string host;
                int port;
            };


        } // namespace backend
    } // namespace protoco
} // namespace au


namespace au {
    namespace protoco{
        class AU_API Server: public thread::cppThread{
        public:
            Server(std::string _server_name): cppThread(_server_name){
                server_name = _server_name;
            }
            Server(handle _handle, std::string _server_name): cppThread(_server_name){
                server_name = _server_name;
                Server::Initialize(_handle);
            }
            ~Server(){
                thread::cppThread::Terminate();
            }

            AU_INLINE int Initialize(handle _handle){
                if(is_initialized)
                   return 0; 

                hd = _handle;

                if(hd.backend == eBackendType::UDP){
                    backend = aptr::make_ptr(new backend::UDPBackend(hd.ip, hd.port));
                    int ret = backend->bind();
                    AU_ASSERT(ret == 0, "UDPBackend bind failed.");
                }

                is_initialized = true;

                return 0;
            }
            AU_INLINE aptr::ptr<backend::IBackend> GetBackend(){
                return backend;
            }
        private:
            std::string server_name;
            handle hd;
            aptr::ptr<backend::IBackend> backend;
            bool is_initialized = false;
        };
        

        class AU_API Client : public thread::cppThread{
        public:
            Client(std::string _client_name): cppThread(_client_name){
                client_name = _client_name;
            }
            Client(handle _handle, std::string _client_name): cppThread(_client_name){
                client_name = _client_name;
                Client::Initialize(_handle);
            }
            ~Client(){
                thread::cppThread::Terminate();
            }

            AU_INLINE int Initialize(handle _handle){
                if(is_initialized)
                   return 0; 

                hd = _handle;
                
                if(hd.backend == eBackendType::UDP){
                    backend = aptr::make_ptr(new backend::UDPBackend(hd.ip, hd.port));
                }
                is_initialized = true;
                return 0;
            }
            AU_INLINE aptr::ptr<backend::IBackend> GetBackend(){
                return backend;
            }

            virtual void Parser(char* buffer, size_t size){

            }
        private:
            std::string client_name;
            handle hd;
            aptr::ptr<backend::IBackend> backend;
            bool is_initialized = false;
        };

    } // namespace protoco
} // namespace au

#endif

#endif //__AU_PROTOCO_H__