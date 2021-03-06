/*
 * Copyright (C) 2006 Tommi Maekitalo
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

#ifndef ZIM_FILEIMPL_H
#define ZIM_FILEIMPL_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <zim/refcounted.h>
#include <zim/zim.h>
#include <zim/fileheader.h>
#include <zim/cache.h>
#include <zim/dirent.h>
#include <zim/cluster.h>
#include <zim/buffer.h>

namespace zim
{
  class FileReader;
  class FileCompound;
  class FileImpl : public RefCounted
  {
      std::shared_ptr<FileCompound> zimFile;
      std::shared_ptr<FileReader> zimReader;
      std::vector<char> bufferDirentZone;
      Fileheader header;
      std::string filename;

      std::shared_ptr<const Buffer> titleIndexBuffer;
      std::shared_ptr<const Buffer> urlPtrOffsetBuffer;
      std::shared_ptr<const Buffer> clusterOffsetBuffer;

      Cache<size_type, const Dirent> direntCache;
      Cache<offset_type, std::shared_ptr<Cluster>> clusterCache;
      bool cacheUncompressedCluster;
      typedef std::map<char, size_type> NamespaceCache;
      NamespaceCache namespaceBeginCache;
      NamespaceCache namespaceEndCache;

      std::string namespaces;

      typedef std::vector<std::string> MimeTypes;
      MimeTypes mimeTypes;

      offset_type getOffset(const Buffer* buffer, size_type idx);

    public:
      explicit FileImpl(const char* fname);

      time_t getMTime() const;

      const std::string& getFilename() const   { return filename; }
      const Fileheader& getFileheader() const  { return header; }
      offset_type getFilesize() const;

      const Dirent getDirent(size_type idx);
      const Dirent getDirentByTitle(size_type idx);
      size_type getIndexByTitle(size_type idx);
      size_type getCountArticles() const       { return header.getArticleCount(); }

      std::shared_ptr<Cluster> getCluster(size_type idx);
      size_type getCountClusters() const       { return header.getClusterCount(); }
      offset_type getClusterOffset(size_type idx)   { return getOffset(clusterOffsetBuffer.get(), idx); }

      size_type getNamespaceBeginOffset(char ch);
      size_type getNamespaceEndOffset(char ch);
      size_type getNamespaceCount(char ns)
        { return getNamespaceEndOffset(ns) - getNamespaceBeginOffset(ns); }

      std::string getNamespaces();
      bool hasNamespace(char ch);

      const std::string& getMimeType(uint16_t idx) const;

      std::string getChecksum();
      bool verify();
      bool is_multiPart() const;
  };

}

#endif // ZIM_FILEIMPL_H

