// Copyright 2015 Jason Juang

#ifndef MODULES_CUDASAMPLE_INCLUDE_HVR_CUDASAMPLE_CUDASAMPLE_CUH_
#define MODULES_CUDASAMPLE_INCLUDE_HVR_CUDASAMPLE_CUDASAMPLE_CUH_

#include <vector>

namespace hvr
{
/**
  * @brief      Minimal cuda class
  *
  * @ingroup    CUDASample
  * @author     Jason Juang
  * @attention  This is for internal use only, DO NOT distribute the code
  */
class HVR_CUDASAMPLE_DLL CUDASample
{
 public:
  CUDASample();
  ~CUDASample();

  /**
   * @brief         add one to every element in vector
   *
   * @param[in,out] in    input vector
   */
  void AddOneToVector(std::vector<int> &in) const;
};

}  // namespace hvr

#endif  // MODULES_CUDASAMPLE_INCLUDE_HVR_CUDASAMPLE_CUDASAMPLE_CUH_
