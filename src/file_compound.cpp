/*
 * Copyright (C) 2017 Matthieu Gautier <mgautier@kymeria.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <zim/file_compound.h>
#include <zim/buffer.h>

#include <errno.h>
#include <string.h>
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace zim {

FileCompound::FileCompound(const std::string& filename)
{
  auto part = new FilePart(filename);
  if (part->good())
  {
    emplace(Range(0, part->size()), part);
    _fsize = part->size();
  } else {
    int errnoSave = errno;
    _fsize = 0;
    for (char ch0 = 'a'; ch0 <= 'z'; ++ch0)
    {
      std::string fname0 = filename + ch0;
      for (char ch1 = 'a'; ch1 <= 'z'; ++ch1)
      {
        std::string fname1 = fname0 + ch1;

        auto currentPart = new FilePart(fname1);
        if (currentPart->fail())  {
          break;
        }
        emplace(Range(_fsize, _fsize+currentPart->size()), currentPart);
        _fsize += currentPart->size();
      }
    }

    if (empty())
    {
      std::ostringstream msg;
      msg << "error " << errnoSave << " opening file \"" << filename;
      throw std::runtime_error(msg.str());
    }
  }
}

time_t FileCompound::getMTime() const {
  if (mtime || empty())
    return mtime;

  const char* fname = begin()->second->filename().c_str();

  #if defined(HAVE_STAT64) && ! defined(__APPLE__)
    struct stat64 st;
    int ret = ::stat64(fname, &st);
  #else
    struct stat st;
    int ret = ::stat(fname, &st);
  #endif
  if (ret != 0)
  {
    std::ostringstream msg;
    msg << "stat failed with errno " << errno << " : " << strerror(errno);
    throw std::runtime_error(msg.str());
  }
  mtime = st.st_mtime;

  return mtime;

}

} // zim
