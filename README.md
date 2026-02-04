# Spotlight - Real-time Webcam Effects for Linux

**Spotlight** is an experimental project designed to bring functionality similar to Microsoft Windows Studio Effects to Linux desktop environments. These effects are typically restricted to AI-capable hardware on Windows systems and rely heavily on dedicated accelerators. Replicating comparable performance and quality without specialized hardware is challenging, but this project explores achieving efficient real-time performance using general-purpose CPU resources on Linux. It is my hope that in time, this project can support multiple hardware acceleration backends too.

## Project Goals

### Functional Features

* [x] Background Blur
* [ ] Background Replacement
* [ ] Automatic Face Framing
* [ ] Automatic Lighting Adjustment

### Performance Targets

* [ ] Complete end-to-end FHD webcam pipeline operating at 30 FPS on a Raspberry Pi 5 using a single CPU core without hardware acceleration (low quality preset).
* [ ] Minimal visual artifacts such as halos or edge distortions, targeting quality comparable to Google Meet (which provided the selfie segmentation model and the acompanying blog).

### Usability Goals

* Provide a simple GNOME extension interface that communicates with a background daemon using DBus.


## Dependencies and Technologies

The project integrates several third-party libraries and Linux multimedia components:

* **v4l2loopback**
  Provides Virtual Camera devices where Spotlight can write its output.

* **TensorFlow Lite**
  Lightweight inference runtime used for face detection and selfie segmentation. The build process is automated through `3rdparty/build_tflite.sh`.

* **TurboJPEG**
  High-performance JPEG encoding and decoding for MJPEG-based streams in order to support `V4L2_PIX_FMT_MJPEG`.

* **LibYUV**
  Handles YUYV to RGB24 and RGB24 to YUYV conversions in order to support `V4L2_PIX_FMT_YUYV`.

* **SPNG**
  Lightweight PNG decoder used for loading background replacement images.

* **getopt_long**
  Command-line argument parsing.



## Build Instructions

Its just a Makefile for now.
Please ensure all required development libraries and headers are installed before building.

### 1. Build TensorFlow Lite

```bash
./3rdparty/build_tflite.sh
```

This script expects the TensorFlow Lite source release to be extracted into a directory named:

```
tensorflow/
```

### 2. Compile Spotlight

```bash
make
```

The compiled executable `spotlight` will be generated in the project root directory.

---

## Usage

### Creating a Virtual Camera Device

The processed output needs to be exposed as a virtual webcam (for conferencing applications), create a virtual device using `v4l2loopback`:

```bash
sudo modprobe v4l2loopback devices=1 max_buffers=4 exclusive_caps=1 card_label="Spotlight" video_nr=10
```

This creates `/dev/video10`. Confirm using:

```bash
dmesg | grep v4l2loopback
```

or

```bash
ls /dev/video*
```

### Running Spotlight

```bash
./spotlight [OPTIONS]
```



## Command-line Options

| Short | Long Option     | Argument   | Description                          | Default               |
| ----- | --------------- | ---------- | ------------------------------------ | --------------------- |
| -j    | --threads       | NUM        | Number of threads used for inference | 1                     |
| -i    | --input-path    | PATH       | Input V4L2 device path               | /dev/video0           |
|       | --input-width   | WIDTH      | Input video width                    | 1280                  |
|       | --input-height  | HEIGHT     | Input video height                   | 720                   |
|       | --input-fps     | FPS        | Input frame rate                     | 30.0                  |
|       | --input-fourcc  | FOURCC     | Input pixel format (MJPG, YUYV)      | MJPG                  |
| -o    | --output-path   | PATH       | Output V4L2 device path              | /dev/video10          |
|       | --output-width  | WIDTH      | Output video width                   | 1280                  |
|       | --output-height | HEIGHT     | Output video height                  | 720                   |
|       | --output-fps    | FPS        | Output frame rate                    | 30.0                  |
|       | --output-fourcc | FOURCC     | Output pixel format (MJPG, YUYV)     | MJPG                  |
| -b    | --background    | IMAGE_PATH | PNG background image for replacement | assets/background.png |


### Example

Run Spotlight using `/dev/video0` as input and `/dev/video10` as output with background blur enabled:

```bash
./spotlight -i /dev/video0 -o /dev/video10
```



## Machine Learning Models

Pre-trained TensorFlow Lite models are stored in the `models/` directory:

* Face Detection
  `models/face/face_smpl_320p.tflite`

* Selfie Segmentation
  `models/segm/segm_lite_v681.tflite`


## License

This project is distributed under the MIT License. Refer to individual file headers for detailed licensing information.
