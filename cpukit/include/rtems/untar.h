/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Untar an Image
 *
 * This file defines the interface to methods which can untar an image.
 */

/*
 *  Written by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_UNTAR_H
#define _RTEMS_UNTAR_H

#include <stdbool.h>
#include <stddef.h>
#include <tar.h>
#include <zlib.h>
#include <xz.h>

#include <rtems/print.h>

/**
 *  @defgroup libmisc_untar_img Untar Image
 *
 *  @ingroup LibIO
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

#define UNTAR_SUCCESSFUL         0
#define UNTAR_FAIL               1
#define UNTAR_INVALID_CHECKSUM   2
#define UNTAR_INVALID_HEADER     3

#define UNTAR_GZ_INFLATE_FAILED 4
#define UNTAR_GZ_INFLATE_END_FAILED 5

int Untar_FromMemory(void *tar_buf, size_t size);
int Untar_FromMemory_Print(void *tar_buf, size_t size, const rtems_printer* printer);
int Untar_FromFile(const char *tar_name);
int Untar_FromFile_Print(const char *tar_name, const rtems_printer* printer);

#define UNTAR_FILE_NAME_SIZE 100

typedef struct {
  char *file_path;
  char *file_name;
  char link_name[UNTAR_FILE_NAME_SIZE];
  unsigned long mode;
  unsigned long file_size;
  unsigned long nblocks;
  unsigned char linkflag;
  const rtems_printer *printer;
} Untar_HeaderContext;

typedef struct {
  Untar_HeaderContext base;

  /**
   * @brief File path buffer.
   */
  char buf[UNTAR_FILE_NAME_SIZE];

  /**
   * @brief Current context state.
   */
  enum {
    UNTAR_CHUNK_HEADER,
    UNTAR_CHUNK_SKIP,
    UNTAR_CHUNK_WRITE,
    UNTAR_CHUNK_ERROR
  } state;

  /**
   * @brief Header buffer.
   */
  char header[512];

  /**
   * @brief Number of bytes of overall length are already processed.
   */
  size_t done_bytes;

  /**
   * @brief File descriptor of output file.
   */
  int out_fd;
} Untar_ChunkContext;

typedef struct {
  /**
   * @brief Instance of Chunk Context needed for tar decompression.
   */
  Untar_ChunkContext base;

  /**
   * @brief Current zlib context.
   */
  z_stream strm;

  /**
   * @brief Buffer that contains the inflated data.
   */
  void *inflateBuffer;

  /**
   * @brief Size of buffer that contains the inflated data.
   */
  size_t inflateBufferSize;

} Untar_GzChunkContext;

typedef struct {
  /**
   * @brief Instance of Chunk Context needed for tar decompression.
   */
  Untar_ChunkContext base;

  /**
   * @brief Xz context.
   */
  struct xz_dec* strm;

  /**
   * @brief Xz buffer.
   */
  struct xz_buf buf;

  /**
   * @brief Buffer that contains the inflated data.
   */
  void *inflateBuffer;

  /**
   * @brief Size of buffer that contains the inflated data.
   */
  size_t inflateBufferSize;

} Untar_XzChunkContext;

/**
 * @brief Initializes the Untar_ChunkContext files out of a part of a block of
 * memory.
 *
 * @param Untar_ChunkContext *context [in] Pointer to a context structure.
 */
void Untar_ChunkContext_Init(Untar_ChunkContext *context);

/*
 * @brief Rips links, directories and files out of a part of a block of memory.
 *
 * @param Untar_ChunkContext *context [in] Pointer to a context structure.
 * @param void *chunk [in] Pointer to a chunk of a TAR buffer.
 * @param size_t chunk_size [in] Length of the chunk of a TAR buffer.
 *
 * @retval UNTAR_SUCCESSFUL (0)    on successful completion.
 * @retval UNTAR_FAIL              for a faulty step within the process.
 * @retval UNTAR_INVALID_CHECKSUM  for an invalid header checksum.
 * @retval UNTAR_INVALID_HEADER    for an invalid header.
 */

int Untar_FromChunk_Print(
  Untar_ChunkContext *context,
  void *chunk,
  size_t chunk_size,
  const rtems_printer* printer
);

/**
 * @brief Initializes the Untar_ChunkGzContext.
 *
 * @param Untar_ChunkGzContext *context [in] Pointer to a context structure.
 * @param void *inflateBuffer [in] Pointer to a context structure.
 * @param size_t inflateBufferSize [in] Size of inflateBuffer.
 */
int Untar_GzChunkContext_Init(
  Untar_GzChunkContext *ctx,
  void *inflateBuffer,
  size_t inflateBufferSize
);

/*
 * @brief Untars a GZ compressed POSIX TAR file.
 *
 * This is a subroutine used to rip links, directories, and
 * files out of a tar.gz/tgz file.
 *
 * @param Untar_ChunkContext *context [in] Pointer to a context structure.
 * @param ssize buflen [in] Size of valid bytes in input buffer.
 * @param z_stream *strm [in] Pointer to the current zlib context.
 */
int Untar_FromGzChunk_Print(
  Untar_GzChunkContext *ctx,
  void *chunk,
  size_t chunk_size,
  const rtems_printer* printer
);

/**
 * @brief Initializes the Untar_ChunkXzContext.
 *
 * @param Untar_ChunkXzContext *context [in] Pointer to a context structure.
 * @param enum xz_mode mode [in] Dictionary mode.
 * @param uint32_t dict_max [in] Maximum size of dictionary.
 * @param void *inflateBuffer [in] Pointer to a context structure.
 * @param size_t inflateBufferSize [in] Size of inflateBuffer.
 */
int Untar_XzChunkContext_Init(
  Untar_XzChunkContext *ctx,
  enum xz_mode mode,
  uint32_t dict_max,
  void *inflateBuffer,
  size_t inflateBufferSize
);

/*
 * @brief Untars a XZ compressed POSIX TAR file.
 *
 * This is a subroutine used to rip links, directories, and
 * files out of a tar.gz/tgz file.
 *
 * @param Untar_ChunkContext *context [in] Pointer to a context structure.
 * @param ssize buflen [in] Size of valid bytes in input buffer.
 * @param z_stream *strm [in] Pointer to the current zlib context.
 */
int Untar_FromXzChunk_Print(
  Untar_XzChunkContext *ctx,
  const void *chunk,
  size_t chunk_size,
  const rtems_printer* printer
);

int Untar_ProcessHeader(Untar_HeaderContext *ctx, const char *bufr);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif  /* _RTEMS_UNTAR_H */
