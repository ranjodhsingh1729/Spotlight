/**
 * @file model.hpp
 * @author Ranjodh Singh
 *
 * @brief MODEL.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef MODEL_HPP
#define MODEL_HPP

#include <tensorflow/lite/model.h>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>


namespace spotlight {

template <typename ModelType>
class Model
{
 public:

  Model(const std::string& model_path, const int num_threads)
    : model_path(model_path), num_threads(num_threads)
  {
    model = tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
    if (model == nullptr)
      throw std::runtime_error("Failed to load model from " + model_path);

    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
    if (interpreter == nullptr)
      throw std::runtime_error("Failed to build interpreter for " + model_path);

    interpreter->SetNumThreads(num_threads);

    if (interpreter->AllocateTensors() != TfLiteStatus::kTfLiteOk)
      throw std::runtime_error("Failed to allocate tensors for " + model_path);

    modH = interpreter->input_tensor(0)->dims->data[1];
    modW = interpreter->input_tensor(0)->dims->data[2];
    input_tensor = interpreter->typed_input_tensor<ModelType>(0);
  }

  void setInputTensor(const ModelType* input_pointer)
  {
    std::memcpy(
      input_tensor,
      input_pointer,
      3 * ModelHeight() * ModelWidth() * sizeof(ModelType)
    );
  }

  void invoke()
  {
    if (interpreter->Invoke() != TfLiteStatus::kTfLiteOk)
      throw std::runtime_error("Failed to invoke interpreter for " + model_path);
  }

  ModelType* getOutputTensor(const int idx)
  {
    return interpreter->typed_output_tensor<ModelType>(idx);
  }

  int ModelWidth() const { return modW; }
  int ModelHeight() const { return modH; }
  int ModelPixels() const { return modW * modH; }

 private:

  int modH;

  int modW;

  ModelType* input_tensor;

  std::unique_ptr<tflite::FlatBufferModel> model;

  tflite::ops::builtin::BuiltinOpResolver resolver;

  std::unique_ptr<tflite::Interpreter> interpreter;

  const std::string& model_path;

  const int num_threads;
};

} // namespace spotlight

#endif // MODEL_HPP
