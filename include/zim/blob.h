/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef ZIM_BLOB_H
#define ZIM_BLOB_H

#include <iostream>
#include <string>
#include <zim/cluster.h>
#include <algorithm>

namespace zim
{
  class Blob
  {
      const char* _data;
      unsigned _size;
      std::shared_ptr<const Buffer> _buffer;

    public:
      Blob()
        : _data(0), _size(0)
          { }

      Blob(const char* data, unsigned size)
        : _data(data),
          _size(size)
          { }

      Blob(std::shared_ptr<const Buffer> buffer)
        : _data(buffer->data()),
          _size(buffer->size()),
          _buffer(buffer)
          {}

      operator std::string() const { return std::string(_data, _size); }
      const char* data() const  { return _data; }
      const char* end() const   { return _data + _size; }
      unsigned size() const     { return _size; }
  };

  inline std::ostream& operator<< (std::ostream& out, const Blob& blob)
  {
    if (blob.data())
      out.write(blob.data(), blob.size());
    return out;
  }

  inline bool operator== (const Blob& b1, const Blob& b2)
  {
    return b1.size() == b2.size()
        && std::equal(b1.data(), b1.data() + b1.size(), b2.data());
  }
}

#endif // ZIM_BLOB_H
