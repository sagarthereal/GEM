#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <drm/drm.h>
#include <drm/i915_drm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

int main() {
    int fd;
    struct drm_i915_gem_create gem_create = {};
    uint32_t size;
    void *map;

    fd = open("/dev/dri/card0", O_RDWR);
    if (fd < 0) {
        perror("open /dev/dri/card0");
        return EXIT_FAILURE;
    }

    size = 4096; // Example size: 4KB, adjust as needed

    // Align size to the page size
    size = (size + getpagesize() - 1) & ~(getpagesize() - 1);

    gem_create.size = size;

    // Create GEM buffer
    if (ioctl(fd, DRM_IOCTL_I915_GEM_CREATE, &gem_create) < 0) {
        perror("DRM_IOCTL_I915_GEM_CREATE");
        close(fd);
        return EXIT_FAILURE;
    }

    // Map the buffer to user space
    map = mmap(0, gem_create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, gem_create.handle);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    // Use the buffer (for demonstration, fill it with zeros)
    memset(map, 0, size);

    // Unmap the buffer and close the DRM device
    munmap(map, gem_create.size);
    close(fd);

    return EXIT_SUCCESS;
}
