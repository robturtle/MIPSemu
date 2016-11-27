#ifndef MIPS_YANGLIU_BIN_INSPECT_1126_2107
#define MIPS_YANGLIU_BIN_INSPECT_1126_2107

#include <string>
#include <sstream>

struct BinInspect
{
  std::string bin_format, hex_format;

  BinInspect(size_t value, size_t len)
  {
    std::ostringstream bs, hs;
    for (size_t i = 0, v = value; i < len; i++)
    {
      bs << v % 2;
      v /= 2;
      if (i != len - 1 && i % 4 == 3) bs << '-';
    } 

    size_t hex_len = (len - 1)/4 + 1;
    hs << std::hex;
    for (size_t i = 0, v = value; i < hex_len; i++)
    {
      hs << v % 0x10;
      v /= 0x10;
      if (i != hex_len -1) hs << "    ";
      else if (len % 4 == 0) hs << "   ";
      else for (int k = 0; k < len % 4 - 1; k++) hs << ' ';
    }
    bin_format = bs.str();
    reverse(bin_format.begin(), bin_format.end());
    hex_format = hs.str();
    reverse(hex_format.begin(), hex_format.end());
  }

  BinInspect operator|(BinInspect const &other)
  {
    BinInspect copy(*this);
    copy.bin_format =
        (std::ostringstream() << copy.bin_format << ' ' << other.bin_format).str();
    copy.hex_format =
        (std::ostringstream() << copy.hex_format << ' ' << other.hex_format).str();
    return copy;
  }
};

std::ostream& operator<<(std::ostream &o, BinInspect const &b)
{
  o << b.bin_format << '\n' << b.hex_format;
  return o;
}

#endif
