#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <drm/i915_drm.h>

int main() {
    int fd = open("/dev/dri/card0", O_RDWR);
    if (fd < 0) {
        perror("open /dev/dri/card0");
        return EXIT_FAILURE;
    }

    struct drm_i915_gem_create create = { .size = 4096 }; // 4KB buffer
    if (ioctl(fd, DRM_IOCTL_I915_GEM_CREATE, &create) < 0) {
        perror("DRM_IOCTL_I915_GEM_CREATE");
        close(fd);
        return EXIT_FAILURE;
    }

    struct drm_i915_gem_mmap mmap_arg = { 
        .handle = create.handle, 
        .size = create.size 
    };
    if (ioctl(fd, DRM_IOCTL_I915_GEM_MMAP, &mmap_arg) < 0) {
        perror("DRM_IOCTL_I915_GEM_MMAP");
        close(fd);
        return EXIT_FAILURE;
    }

    void *map = mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mmap_arg.offset);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    // Use the buffer (for demonstration, fill it with zeros)
    memset(map, 0, create.size);

    // Clean up
    munmap(map, create.size);
    struct drm_gem_close close_buf = { .handle = create.handle };
    ioctl(fd, DRM_IOCTL_GEM_CLOSE, &close_buf);
    close(fd);

    return EXIT_SUCCESS;
}
