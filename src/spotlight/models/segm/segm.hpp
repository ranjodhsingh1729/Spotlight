/**
 * @file segm.hpp
 * @author Ranjodh Singh
 *
 * @brief SEGM.
 *
 * Copyright (c) 2026 Ranjodh Singh
 * This file is licensed under the MIT License.
 * You may obtain a copy of the License at https://opensource.org/license/MIT.
 */
#ifndef SEGM_HPP
#define SEGM_HPP

#include <spotlight/models/model.hpp>


namespace spotlight {

template <typename ModelType>
class SelfieSegmentation
{
 public:

  SelfieSegmentation(const std::string& model_path, const int num_threads)
    : model(model_path, num_threads)
  {
    mask_tensor = model.getOutputTensor(0);
  }

  // Note: bg - 0, fg - 1
  void invoke(const ModelType* input, ModelType* output)
  {
    model.setInputTensor(input);
    model.invoke();
    postProcess(output);
  }

  void postProcess(ModelType* output)
  {
    // background - 2*i+0, forground - 2*i+1
    for (int i = 0; i < ModelHeight() * ModelWidth(); i++)
    {
      // const float exp_fg = expf(mask_tensor[2*i+1]);
      // const float exp_bg = expf(mask_tensor[2*i+0]);
      // const float prob_fg = exp_fg / (exp_bg + exp_fg);
      // const float prob_bg = exp_bg / (exp_bg + exp_fg);

      output[i] = mask_tensor[2*i] < mask_tensor[2*i+1];

      // output[i] = mask_tensor[2*i] < mask_tensor[2*i+1];
      // output[3*i+0] = mask_tensor[2*i] < mask_tensor[2*i+1];
      // output[3*i+1] = mask_tensor[2*i] < mask_tensor[2*i+1];
      // output[3*i+2] = mask_tensor[2*i] < mask_tensor[2*i+1];
    }
  }

  int ModelWidth() const { return model.ModelWidth(); }
  int ModelHeight() const { return model.ModelHeight(); }
  int ModelPixels() const { return model.ModelWidth() * model.ModelHeight(); }

 private:

  ModelType* mask_tensor;

  Model<ModelType> model;
};

} // namespace spotlight

#endif // SEGM_HPP
