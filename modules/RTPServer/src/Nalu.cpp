// Copyright @ 2017 Caoyang Jiang

#include "Jcy/RTPServer/Nalu.h"

JCY_WINDOWS_DISABLE_ALL_WARNING
#include <iostream>
#include <vector>
JCY_WINDOWS_ENABLE_ALL_WARNING

namespace jcy
{
Nalu::Nalu()
{
}

Nalu::Nalu(Nalu&& nalu)
{
  nri_    = nalu.nri_;
  type_   = nalu.type_;
  data_   = nalu.data_;
  isfrag_ = nalu.isfrag_;
  fragid_ = nalu.fragid_;
  data_   = std::move(nalu.data_);
}

Nalu::Nalu(const Nalu& nalu)
{
  nri_    = nalu.nri_;
  type_   = nalu.type_;
  data_   = nalu.data_;
  isfrag_ = nalu.isfrag_;
  fragid_ = nalu.fragid_;
  data_   = nalu.data_;
}

Nalu& Nalu::operator=(const Nalu& nalu)
{
  nri_    = nalu.nri_;
  type_   = nalu.type_;
  data_   = nalu.data_;
  isfrag_ = nalu.isfrag_;
  fragid_ = nalu.fragid_;
  data_   = nalu.data_;
  return *this;
}

Nalu::Nalu(const std::vector<uint8_t>& data) : data_(data)
{
  uint8_t nalbyte = data[0];

  if ((nalbyte & 0x80) != 0)
  {
    throw std::runtime_error("Invalid Nalu.");
  }
  else
  {
    nri_  = (nalbyte >> 5) & 0x03;
    type_ = nalbyte & 0x1F;
  }
}

Nalu::Nalu(const uint8_t* data, size_t size) : data_(data, data + size)
{
  uint8_t nalbyte = data[0];

  if ((nalbyte & 0x80) != 0)
  {
    throw std::runtime_error("Invalid Nalu.");
  }
  else
  {
    nri_  = (nalbyte >> 5) & 0x03;
    type_ = nalbyte & 0x1F;
  }
}

Nalu::Nalu(const uint8_t* data,
           size_t size,
           uint32_t nri,
           uint32_t type,
           bool isfragment     = false,
           uint32_t fragmentid = 0)
    : data_(data, data + size)
{
  nri_    = nri;
  type_   = type;
  isfrag_ = isfragment;
  fragid_ = fragmentid;
}

Nalu::Nalu(const std::vector<uint8_t>& data,
           uint32_t nri,
           uint32_t type,
           bool isfragment     = false,
           uint32_t fragmentid = 0)
    : data_(data)
{
  nri_    = nri;
  type_   = type;
  isfrag_ = isfragment;
  fragid_ = fragmentid;
}

Nalu::~Nalu()
{
}

bool Nalu::Fragment(int fragmentsize, std::vector<Nalu>& fragnalus)
{
  size_t totalfrag = (data_.size() - 1 + fragmentsize - 1) / fragmentsize;
  size_t beg       = 1;  // Remove the first byte containing f,nri,type.
                   // FU-A,B type has its own header to signal these parameters.
  size_t end      = 0;
  uint32_t fragid = 0;

  while (beg < data_.size())
  {
    if ((beg + fragmentsize) > data_.size())
    {
      end = data_.size();
    }
    else
    {
      end = beg + fragmentsize;
    }

    fragnalus.push_back(std::move(
        Nalu(data_.data() + beg, end - beg, nri_, type_, true, fragid)));

    fragid++;
    beg = end;
  }

  if (totalfrag != fragid)
  {
    std::cout << "[ERROR]: Fragmentation failed." << std::endl;
  }
  return true;
}

void NaluTools::SplitNaluStream(const uint8_t* stream,
                                size_t size,
                                std::vector<Nalu>& nalus)
{
  size_t readpos = 0;
  std::vector<size_t> nalupos;
  std::vector<size_t> nalusizes;

  while (readpos < size - 2)
  {
    if ((stream[readpos] == 0x00) && (stream[readpos + 1] == 0x00) &&
        (stream[readpos + 2] == 0x01))
    {
      nalupos.push_back(readpos + 3);
      readpos += 4;
    }
    else
    {
      readpos++;
    }
  }

  for (size_t i = 1; i < nalupos.size(); i++)
  {
    nalusizes.push_back(nalupos[i] - 3 - nalupos[i - 1]);
  }

  if (nalupos.size() != 0) nalusizes.push_back(size - nalupos.back());

  for (size_t i = 0; i < nalupos.size(); i++)
  {
    nalus.push_back(Nalu(stream + nalupos[i], nalusizes[i]));
  }
}

}  // namespace jcy
