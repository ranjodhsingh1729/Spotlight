# See: https://ai.google.dev/edge/litert/build/arm
# Release: https://github.com/tensorflow/tensorflow/releases/tag/v2.20.0

cd tensorflow/
python ./configure.py
bazel build -c opt //tensorflow/lite:libtensorflowlite.so
cd ../
cp tensorflow/bazel-bin/tensorflow/lite/libtensorflowlite.so .
