// Copyright @ 2017 Caoyang Jiang

#ifndef MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_NALU_H_
#define MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_NALU_H_

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
class Nalu
{
 public:
  Nalu();
  Nalu(Nalu&& nalu);
  Nalu(const Nalu& nalu);
  Nalu& operator=(const Nalu& nalu);

  /**
   * @brief      Initialize nalu and auto detect nri and type
   *
   * @param[in]  data  The data
   */
  explicit Nalu(const std::vector<uint8_t>& data);

  /**
   * @brief      Initialize nalu and auto detect nri and type
   *
   * @param[in]  data  The data
   * @param[in]  size  The size
   */
  Nalu(const uint8_t* data, size_t size);

  Nalu(const uint8_t* data,
       size_t size,
       uint32_t nri,
       uint32_t type,
       bool isfragment,
       uint32_t fragmentid);

  Nalu(const std::vector<uint8_t>& data,
       uint32_t nri,
       uint32_t type,
       bool isfragment,
       uint32_t fragmentid);

  ~Nalu();

  bool Fragment(int fragmentsize, std::vector<Nalu>& fragnalus);

  inline uint32_t GetNRI() const
  {
    return nri_;
  }

  inline uint32_t GetType() const
  {
    return type_;
  }

  inline bool IsFragment() const
  {
    return isfrag_;
  }

  inline uint32_t GetFragmentID() const
  {
    return fragid_;
  }

  inline int GetSize() const
  {
    return data_.size();
  }

  inline const std::vector<uint8_t>& GetData() const
  {
    return data_;
  }

 private:
  uint32_t nri_    = 0;
  uint32_t type_   = 0;
  bool isfrag_     = false;
  uint32_t fragid_ = 0;
  std::vector<uint8_t> data_;
};

class NaluTools
{
 public:
  static void SplitNaluStream(const uint8_t* stream,
                              size_t size,
                              std::vector<Nalu>& nalus);
};

}  // namespace hvr
#endif  // MODULES_RTPSERVER_INCLUDE_JCY_RTPSERVER_NALU_H_
