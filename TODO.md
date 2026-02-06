# TODO

## LIST

- [ ] Improove Throughput. (<30ms even for FHD Resolution)
- [ ] Improove Efficienty. (Single E Core Low Utilization)
- [ ] Reduce Memory Consumption 
  - [ ] Internal buffers are currently float.
  - [ ] Use least amount of buffers requried.

- [ ] Fix Halo in relatively dark backgrounds.
- [ ] Add the face detection and auto framming.
  - [ ] Model is there just needs proper plugging in.

- [ ] Custom Allocator to improove memory layout.
  - [ ] Arena (Bump) allocator should suffice for our purpose.

- [ ] Use DMABUF for true ZERO-COPY.
- [ ] Avoid extra copy in jpeg encoding.
- [ ] Avoid extra copy in yuyv enc/dec.
- [ ] Add support for GPUs (USING OPENCL/SYCL)


## PLAN

- [ ] Fix Halo in relatively dark backgrounds.
- [ ] Polish the CPU version the best you can.
- [ ] Implement CONF file and DBUS Interface.
- [ ] Write the gnome extension and release.
