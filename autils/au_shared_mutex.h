#ifndef __AU_SHARED_MUTEX_H__
#define __AU_SHARED_MUTEX_H__

#include "au_common.h"

#ifdef USE_AU_SHARED_MUTEX

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace au {
	namespace shm {
        class AU_API SharedMutex {
        public:
            SharedMutex();
            ~SharedMutex();

            // flag: O_CREAT | O_RDWR
            AU_INLINE int create(std::string name, int flags);
            AU_INLINE void release();
            
            AU_INLINE void lock();
            AU_INLINE void unlock();

        private:
            int shm_fd;
            int flags;
            void* ptr;
            std::string name;
        };


        SharedMutex::SharedMutex(){
            shm_fd = -1;
            flags = 0;
            ptr = nullptr;
            name = "";
        }

        SharedMutex::~SharedMutex(){
            release();
        }

        AU_INLINE int SharedMutex::create(std::string name, int flags){
            release();
            int mode = (S_IRUSR | S_IWUSR) | (S_IRGRP | S_IWGRP);
            this->flags = flags;
            this->name = name;

            // delete old shared memory file
            if(flags & O_CREAT) {
                shm_unlink(name.c_str());
            }

            shm_fd = shm_open(name.c_str(), flags, mode);
            if (shm_fd == -1) {
                return -1;
            }

            if (ftruncate(shm_fd, sizeof(pthread_mutex_t)) == -1) {
                close(shm_fd);
                return -1;
            }

            ptr = mmap(nullptr, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

            if (ptr == MAP_FAILED) {
                close(shm_fd);
                return -2;
            }

            if(flags & O_CREAT) {
                pthread_mutexattr_t attr;
                if(pthread_mutexattr_init(&attr) != 0) {
                    close(shm_fd);
                    return -3;
                }
                pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
                pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
                pthread_mutex_init(reinterpret_cast<pthread_mutex_t*>(ptr), &attr);
                pthread_mutexattr_destroy(&attr);
            }
            
            return 0;
        }

        AU_INLINE void SharedMutex::release(){
            if(ptr) {
                if(flags & O_CREAT) {
                    pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(ptr));
                }
                munmap(ptr, sizeof(pthread_mutex_t));
                ptr = nullptr;
            }
        
            if (shm_fd != -1) {
                close(shm_fd);
                shm_fd = -1;
            }
        
            if(flags & O_CREAT) {
                shm_unlink(name.c_str());
            }
            name.clear();
            flags = 0;
        }

        AU_INLINE void SharedMutex::lock(){
            int ret = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(ptr));
            if(ret == EOWNERDEAD){
                // if previous process died and holding the lock
                // mark as consistent so others can use it
                pthread_mutex_consistent(reinterpret_cast<pthread_mutex_t*>(ptr));
            }else if (ret != 0){
                perror("SharedMutex pthread lock failed");
            }
        }

        AU_INLINE void SharedMutex::unlock(){
            int ret = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(ptr));
            if(ret != 0){
                perror("SharedMutex pthread unlock failed");
            }
        }
        
    }
}


#endif //USE_AU_SHARED_MEMORY

#endif // __AU_SHARED_MEMORY_H__