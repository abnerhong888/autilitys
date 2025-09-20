#ifndef __AU_SHARED_MEMORY_H__
#define __AU_SHARED_MEMORY_H__

#include "au_common.h"

#ifdef USE_AU_SHARED_MEMORY

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace au {
	namespace shm {
        class AU_API SharedMemory {
        public:
            SharedMemory();
            ~SharedMemory();

            // flag: O_CREAT | O_RDWR
            AU_INLINE int map(std::string name, size_t size, int flags);
            AU_INLINE int unmap();
            AU_INLINE void release();

            AU_INLINE size_t get_size();
            AU_INLINE void* get_ptr();
            AU_INLINE std::string get_name();

            template<typename T>
            AU_INLINE T* cast_ptr(){
                return reinterpret_cast<T*>(ptr);
            }

        private:
            int shm_fd;
            int flags;
            size_t size;
            void* ptr;
            std::string name;
        };


        SharedMemory::SharedMemory(){
            shm_fd = -1;
            flags = 0;
            size = 0;
            ptr = nullptr;
            name = "";
        }

        SharedMemory::~SharedMemory(){
            release();
        }

        
        AU_INLINE int SharedMemory::map(std::string name, size_t size, int flags){
            release();
            int mode = (S_IRUSR | S_IWUSR) | (S_IRGRP | S_IWGRP);
            this->flags = flags;
            this->name = name;
            
            shm_fd = shm_open(name.c_str(), flags, mode);

            if (shm_fd == -1) {
                return -1;
            }

            this->size = size;

            if (ftruncate(shm_fd, size) == -1) {
                close(shm_fd);
                return -2;
            }

            ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

            if (ptr == MAP_FAILED) {
                close(shm_fd);
                return -3;
            }

            return 0;
        }

        AU_INLINE int SharedMemory::unmap(){
            if(ptr) {
                munmap(ptr, size);
                ptr = nullptr;
                size = 0;
            }
            return 0;
        }

        AU_INLINE void SharedMemory::release(){
            unmap();

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

        AU_INLINE size_t SharedMemory::get_size(){
            return size;
        }

        AU_INLINE void* SharedMemory::get_ptr(){
            return ptr;
        }

        AU_INLINE std::string SharedMemory::get_name(){
            return name;
        }
    }
}


#endif //USE_AU_SHARED_MEMORY

#endif // __AU_SHARED_MEMORY_H__