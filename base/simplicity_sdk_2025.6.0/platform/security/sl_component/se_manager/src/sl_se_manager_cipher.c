/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_se_manager.h"
#if defined(SLI_MAILBOX_COMMAND_SUPPORTED)

#include "sli_se_manager_internal.h"
#include "sli_se_manager_mailbox.h"
#include "sl_assert.h"
#include <string.h>

/// @addtogroup sl_se_manager
/// @{

uint32_t memcmp_time_cst(uint8_t *in1, uint8_t *in2, uint32_t size)
{
  //Don't try to optimise this function for performance, it's time constant for security reasons
  uint32_t diff = 0;
  uint32_t i = 0;
  for (i = 0; i < size; i++) {
    diff |= (*(in1 + i) ^ (*(in2 + i)));
  }

  return (diff > 0);
}

// -----------------------------------------------------------------------------
// Global Functions

/***************************************************************************//**
 * AES-ECB block encryption/decryption.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_ecb(sl_se_command_context_t *cmd_ctx,
                                const sl_se_key_descriptor_t *key,
                                sl_se_cipher_operation_t mode,
                                size_t length,
                                const unsigned char *input,
                                unsigned char *output)
{
  if (cmd_ctx == NULL || key == NULL || input == NULL || output == NULL
      || (length & 0xFU) != 0U) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  sli_se_command_init(cmd_ctx,
                      (mode == SL_SE_ENCRYPT
                       ? SLI_SE_COMMAND_AES_ENCRYPT : SLI_SE_COMMAND_AES_DECRYPT)
                      | SLI_SE_COMMAND_OPTION_MODE_ECB
                      | SLI_SE_COMMAND_OPTION_CONTEXT_WHOLE);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, status);
  // Message size (number of bytes)
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_datatransfer_t in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &in);

  sli_se_datatransfer_t out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  sli_se_mailbox_command_add_output(se_cmd, &out);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * AES-CBC buffer encryption/decryption.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_cbc(sl_se_command_context_t *cmd_ctx,
                                const sl_se_key_descriptor_t *key,
                                sl_se_cipher_operation_t mode,
                                size_t length,
                                unsigned char iv[16],
                                const unsigned char *input,
                                unsigned char *output)
{
  if (cmd_ctx == NULL || key == NULL || input == NULL || output == NULL
      || iv == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  // Input length must be a multiple of 16 bytes which is the AES block length
  if (length & 0xf) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx,
                      (mode == SL_SE_ENCRYPT
                       ? SLI_SE_COMMAND_AES_ENCRYPT : SLI_SE_COMMAND_AES_DECRYPT)
                      | SLI_SE_COMMAND_OPTION_MODE_CBC
                      | SLI_SE_COMMAND_OPTION_CONTEXT_ADD);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, status);
  // Message size (number of bytes)
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, 16);
  sli_se_datatransfer_t in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &iv_in);
  sli_se_mailbox_command_add_input(se_cmd, &in);

  sli_se_datatransfer_t out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  sli_se_datatransfer_t iv_out = SLI_SE_DATATRANSFER_DEFAULT(iv, 16);
  sli_se_mailbox_command_add_output(se_cmd, &out);
  sli_se_mailbox_command_add_output(se_cmd, &iv_out);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * AES-CFB128 buffer encryption/decryption.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_cfb128(sl_se_command_context_t *cmd_ctx,
                                   const sl_se_key_descriptor_t *key,
                                   sl_se_cipher_operation_t mode,
                                   size_t length,
                                   uint32_t *iv_off,
                                   unsigned char iv[16],
                                   const unsigned char *input,
                                   unsigned char *output)
{
  if (cmd_ctx == NULL || key == NULL || input == NULL || output == NULL
      || iv == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  uint32_t n = iv_off ? *iv_off : 0;
  uint32_t processed = 0;
  sl_status_t command_status = SL_STATUS_OK;

  while (processed < length) {
    if (n > 0) {
      // start by filling up the IV
      if (mode == SL_SE_ENCRYPT) {
        iv[n] = output[processed] = (unsigned char)(iv[n] ^ input[processed]);
      } else {
        int c = input[processed];
        output[processed] = (unsigned char)(c ^ iv[n]);
        iv[n] = (unsigned char) c;
      }
      n = (n + 1) & 0x0F;
      processed++;
    } else {
      // process one ore more blocks of data
      uint32_t iterations = (length - processed) / 16;

      if (iterations > 0) {
        sli_se_command_init(cmd_ctx,
                            (mode == SL_SE_ENCRYPT
                             ? SLI_SE_COMMAND_AES_ENCRYPT : SLI_SE_COMMAND_AES_DECRYPT)
                            | SLI_SE_COMMAND_OPTION_MODE_CFB
                            | SLI_SE_COMMAND_OPTION_CONTEXT_ADD);

        // Add key parameters to command
        sli_add_key_parameters(cmd_ctx, key, command_status);
        // Message size (number of bytes)
        sli_se_mailbox_command_add_parameter(se_cmd, iterations * 16);

        // Add key metadata block to command
        sli_add_key_metadata(cmd_ctx, key, command_status);
        // Add key input block to command
        sli_add_key_input(cmd_ctx, key, command_status);

        sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, 16);
        sli_se_datatransfer_t in = SLI_SE_DATATRANSFER_DEFAULT(&input[processed], iterations * 16);
        sli_se_mailbox_command_add_input(se_cmd, &iv_in);
        sli_se_mailbox_command_add_input(se_cmd, &in);

        sli_se_datatransfer_t out = SLI_SE_DATATRANSFER_DEFAULT(&output[processed], iterations * 16);
        sli_se_datatransfer_t iv_out = SLI_SE_DATATRANSFER_DEFAULT(iv, 16);
        sli_se_mailbox_command_add_output(se_cmd, &out);
        sli_se_mailbox_command_add_output(se_cmd, &iv_out);

        command_status = sli_se_execute_and_wait(cmd_ctx);
        processed += iterations * 16;
        if (command_status != SL_STATUS_OK) {
          return command_status;
        }
      }

      while ((length - processed) > 0) {
        if (n == 0) {
          // Need to update the IV but don't have a full block of input to pass
          // to the SE.
          command_status = sl_se_aes_crypt_ecb(cmd_ctx, key, SL_SE_ENCRYPT, 16U, iv, iv);
          if (command_status != SL_STATUS_OK) {
            return command_status;
          }
        }
        // Save remainder to IV
        if (mode == SL_SE_ENCRYPT) {
          iv[n] = output[processed] = (unsigned char)(iv[n] ^ input[processed]);
        } else {
          int c = input[processed];
          output[processed] = (unsigned char)(c ^ iv[n]);
          iv[n] = (unsigned char) c;
        }
        n = (n + 1) & 0x0F;
        processed++;
      }
    }
  }

  if ( iv_off ) {
    *iv_off = n;
  }

  return command_status;
}

/***************************************************************************//**
 * AES-CFB8 buffer encryption/decryption.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_cfb8(sl_se_command_context_t *cmd_ctx,
                                 const sl_se_key_descriptor_t *key,
                                 sl_se_cipher_operation_t mode,
                                 size_t length,
                                 unsigned char iv[16],
                                 const unsigned char *input,
                                 unsigned char *output)
{
  unsigned char c;
  unsigned char ov[17];
  sl_status_t ret = SL_STATUS_OK;

  if (cmd_ctx == NULL || key == NULL || input == NULL || output == NULL
      || iv == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  while (length--) {
    memcpy(ov, iv, 16U);
    if ((ret = sl_se_aes_crypt_ecb(cmd_ctx, key, SL_SE_ENCRYPT, 16U, iv, iv))
        != SL_STATUS_OK) {
      return ret;
    }

    if (mode == SL_SE_DECRYPT) {
      ov[16] = *input;
    }

    c = *output++ = (unsigned char)(iv[0] ^ *input++);

    if (mode == SL_SE_ENCRYPT) {
      ov[16] = c;
    }

    memcpy(iv, ov + 1, 16U);
  }

  return ret;
}

/***************************************************************************//**
 * Increment the input nonce counter by one
 ******************************************************************************/
static void increment_nonce_counter(uint8_t block_end, unsigned char nonce_counter[])
{
  for (size_t i = 0u; i < SL_SE_AES_BLOCK_SIZE; i++) {
    nonce_counter[block_end - i] = nonce_counter[block_end - i] + 1u;
    if (nonce_counter[block_end - i] != 0u) {
      // did not overflow so no need to increment the value at next index
      break;
    }
  }
}

#if (SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED > 1)
/***************************************************************************//**
 * Prepare the SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED * SL_SE_AES_BLOCK_SIZE byte
 * wide stream block buffer that will be used as nonce counter for
 * encryption/decryption.
 ******************************************************************************/
static void prepare_nonce_counter(unsigned char nonce_counter[],
                                  unsigned char stream_block[])
{
  uint8_t no_of_blocks = ((SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED * SL_SE_AES_BLOCK_SIZE) / SL_SE_AES_BLOCK_SIZE);
  // place the most recent counter in the first stream block
  memcpy(stream_block,
         nonce_counter,
         SL_SE_AES_BLOCK_SIZE);

  for (size_t i = 0; i < no_of_blocks - 1u; i++) {
    // Use the first block's reference counter to update the other
    // blocks since it holds the most recent counter information.
    memcpy(&stream_block[i * SL_SE_AES_BLOCK_SIZE + SL_SE_AES_BLOCK_SIZE],
           &stream_block[i * SL_SE_AES_BLOCK_SIZE],
           SL_SE_AES_BLOCK_SIZE);
    increment_nonce_counter(((i + 2u) * SL_SE_AES_BLOCK_SIZE) - 1u, stream_block);
  }

  // Store the largest counter back in the nonce counter buffer
  memcpy(nonce_counter,
         &stream_block[(no_of_blocks - 1u) * SL_SE_AES_BLOCK_SIZE],
         SL_SE_AES_BLOCK_SIZE);
}
#endif // SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED > 1

/***************************************************************************//**
 * AES-CTR buffer encryption/decryption.
 ******************************************************************************/
sl_status_t sl_se_aes_crypt_ctr(sl_se_command_context_t *cmd_ctx,
                                const sl_se_key_descriptor_t *key,
                                size_t length,
                                uint32_t *nc_off,
                                unsigned char nonce_counter[16],
                                unsigned char stream_block[SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED * SL_SE_AES_BLOCK_SIZE],
                                const unsigned char *input,
                                unsigned char *output)
{
  if (cmd_ctx == NULL || key == NULL
      || (length != 0 && (input == NULL || output == NULL))
      || nonce_counter == NULL || stream_block == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  uint32_t n = nc_off ? *nc_off : 0;
  uint32_t processed = 0;
  sl_status_t command_status = SL_STATUS_OK;

  while (processed < length) {
    if (n > 0) {
      // start by filling up the IV
      output[processed] = (unsigned char)(input[processed] ^ stream_block[n]);
      n = (n + 1) & ((SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED * SL_SE_AES_BLOCK_SIZE) - 1u);
      processed++;
    } else {
      // process one or more blocks of data
      uint32_t iterations = (length - processed) / SL_SE_AES_BLOCK_SIZE;

      if (iterations > 0) {
        sli_se_command_init(cmd_ctx,
                            SLI_SE_COMMAND_AES_ENCRYPT
                            | SLI_SE_COMMAND_OPTION_MODE_CTR
                            | SLI_SE_COMMAND_OPTION_CONTEXT_ADD);

        // Add key parameters to command
        sli_add_key_parameters(cmd_ctx, key, command_status);
        // Message size (number of bytes)
        sli_se_mailbox_command_add_parameter(se_cmd, iterations * SL_SE_AES_BLOCK_SIZE);

        // Add key metadata block to command
        sli_add_key_metadata(cmd_ctx, key, command_status);
        // Add key input block to command
        sli_add_key_input(cmd_ctx, key, command_status);

        sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(nonce_counter, SL_SE_AES_BLOCK_SIZE);
        sli_se_datatransfer_t in = SLI_SE_DATATRANSFER_DEFAULT(&input[processed], iterations * SL_SE_AES_BLOCK_SIZE);
        sli_se_mailbox_command_add_input(se_cmd, &iv_in);
        sli_se_mailbox_command_add_input(se_cmd, &in);

        sli_se_datatransfer_t out = SLI_SE_DATATRANSFER_DEFAULT(&output[processed], iterations * SL_SE_AES_BLOCK_SIZE);
        sli_se_datatransfer_t iv_out = SLI_SE_DATATRANSFER_DEFAULT(nonce_counter, SL_SE_AES_BLOCK_SIZE);
        sli_se_mailbox_command_add_output(se_cmd, &out);
        sli_se_mailbox_command_add_output(se_cmd, &iv_out);

        command_status = sli_se_execute_and_wait(cmd_ctx);
        processed += iterations * SL_SE_AES_BLOCK_SIZE;
        if (command_status != SL_STATUS_OK) {
          return command_status;
        }
      }

      while ((length - processed) > 0) {
        if (n == 0) {
          // Get a new stream block
          unsigned char *counter_ptr = NULL;
          #if (SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED > 1)
          // Use the nonce counter buffer as the reference to create nonce counter blocks
          // needed to compute the key stream blocks. Also, update the nonce counter buffer
          // to store the latest block.
          prepare_nonce_counter(nonce_counter, stream_block);
          // The key stream buffer now holds the nonce counter
          counter_ptr = stream_block;
          #else
          counter_ptr = nonce_counter;
          #endif // SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED > 1

          command_status = sl_se_aes_crypt_ecb(cmd_ctx,
                                               key,
                                               SL_SE_ENCRYPT,
                                               SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED * SL_SE_AES_BLOCK_SIZE,
                                               counter_ptr,
                                               stream_block);
          if (command_status != SL_STATUS_OK) {
            return command_status;
          }
          increment_nonce_counter(SL_SE_AES_BLOCK_SIZE - 1u, nonce_counter);
        }
        // Save remainder to IV
        output[processed] = (unsigned char)(input[processed] ^ stream_block[n]);
        n = (n + 1) & ((SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED * SL_SE_AES_BLOCK_SIZE) - 1u);
        processed++;
      }
    }
  }

  if ( nc_off ) {
    *nc_off = n;
  }

  return command_status;
}

/***************************************************************************//**
 * AES-CCM buffer encryption.
 ******************************************************************************/
sl_status_t sl_se_ccm_encrypt_and_tag(sl_se_command_context_t *cmd_ctx,
                                      const sl_se_key_descriptor_t *key,
                                      size_t length,
                                      const unsigned char *iv, size_t iv_len,
                                      const unsigned char *add, size_t add_len,
                                      const unsigned char *input,
                                      unsigned char *output,
                                      unsigned char *tag, size_t tag_len)
{
  if (cmd_ctx == NULL || key == NULL || (tag_len > 0 && tag == NULL) || iv == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (add_len > 0 && add == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (length > 0 && (input == NULL || output == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  unsigned char q;
  sl_status_t command_status = SL_STATUS_OK;

  // Test for invalid (too long) message length. This test is included here because
  // the SE does not implement the test. When the SE ultimately implements the test
  // the following test can be removed.
  q = 16 - 1 - (unsigned char) iv_len;
  if ((q < sizeof(length)) && (length >= (1UL << (q * 8)))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (tag_len == 2 || tag_len > 16 || tag_len % 2 != 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Also implies q is within bounds
  if (iv_len < 7 || iv_len > 13) {
    return SL_STATUS_INVALID_PARAMETER;
  }

#if !defined(SLI_SE_MANAGER_HOST_SYSTEM)
  if ((uint32_t)output + length > (SRAM_BASE + SRAM_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif // SLI_SE_MANAGER_HOST_SYSTEM

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_AES_CCM_ENCRYPT);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, command_status);
  // Message size (number of bytes)
  sli_se_mailbox_command_add_parameter(se_cmd, ((iv_len & 0xFFFF) << 16) | (tag_len & 0xFFFF));
  sli_se_mailbox_command_add_parameter(se_cmd, add_len);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, command_status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, command_status);

  sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_datatransfer_t in_add = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
  sli_se_datatransfer_t in_nonce = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
  sli_se_mailbox_command_add_input(se_cmd, &in_nonce);
  sli_se_mailbox_command_add_input(se_cmd, &in_add);
  sli_se_mailbox_command_add_input(se_cmd, &in_data);

  sli_se_datatransfer_t out_data = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  sli_se_datatransfer_t out_tag = SLI_SE_DATATRANSFER_DEFAULT(tag, tag_len);
  sli_se_mailbox_command_add_output(se_cmd, &out_data);
  sli_se_mailbox_command_add_output(se_cmd, &out_tag);

  command_status = sli_se_execute_and_wait(cmd_ctx);
  return command_status;
}

/***************************************************************************//**
 * AES-CCM buffer decryption.
 ******************************************************************************/
sl_status_t sl_se_ccm_auth_decrypt(sl_se_command_context_t *cmd_ctx,
                                   const sl_se_key_descriptor_t *key,
                                   size_t length,
                                   const unsigned char *iv, size_t iv_len,
                                   const unsigned char *add, size_t add_len,
                                   const unsigned char *input,
                                   unsigned char *output,
                                   const unsigned char *tag, size_t tag_len)
{
  if (cmd_ctx == NULL || key == NULL || tag == NULL || iv == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (add_len > 0 && add == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (length > 0 && (input == NULL || output == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  unsigned char q;
  sl_status_t command_status = SL_STATUS_OK;

  // Test for invalid (too long) message length. This test is included here because
  // the SE does not implement the test. When the SE ultimately implements the test
  // the following test can be removed.
  q = 16 - 1 - (unsigned char) iv_len;
  if ((q < sizeof(length)) && (length >= (1UL << (q * 8)))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (tag_len == 2 || tag_len == 0 || tag_len > 16 || tag_len % 2 != 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Also implies q is within bounds */
  if (iv_len < 7 || iv_len > 13) {
    return SL_STATUS_INVALID_PARAMETER;
  }

#if !defined(SLI_SE_MANAGER_HOST_SYSTEM)
  if ((uint32_t)output + length > (SRAM_BASE + SRAM_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif // SLI_SE_MANAGER_HOST_SYSTEM

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_AES_CCM_DECRYPT);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, command_status);
  // Message size (number of bytes)
  sli_se_mailbox_command_add_parameter(se_cmd, ((iv_len & 0xFFFF) << 16) | (tag_len & 0xFFFF));
  sli_se_mailbox_command_add_parameter(se_cmd, add_len);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, command_status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, command_status);

  sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_datatransfer_t in_add = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
  sli_se_datatransfer_t in_nonce = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
  sli_se_mailbox_command_add_input(se_cmd, &in_nonce);
  sli_se_mailbox_command_add_input(se_cmd, &in_add);
  sli_se_mailbox_command_add_input(se_cmd, &in_data);

  sli_se_datatransfer_t out_data = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  sli_se_mailbox_command_add_output(se_cmd, &out_data);
  sli_se_datatransfer_t in_tag = SLI_SE_DATATRANSFER_DEFAULT(tag, tag_len);
  sli_se_mailbox_command_add_input(se_cmd, &in_tag);

  command_status = sli_se_execute_and_wait(cmd_ctx);
  if (command_status == SL_STATUS_OK) {
    return SL_STATUS_OK;
  } else {
    memset(output, 0, length);
    return command_status;
  }
}

#if defined(SLI_SE_MAJOR_VERSION_ONE)
sl_status_t sl_se_ccm_multipart_starts(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       sl_se_cipher_operation_t mode,
                                       uint32_t total_message_length,
                                       const uint8_t *iv,
                                       size_t iv_len,
                                       const uint8_t *aad,
                                       size_t aad_len,
                                       size_t tag_len)
{
  sl_status_t status = SL_STATUS_OK;
  uint8_t q;
  uint8_t b[SL_SE_AES_BLOCK_SIZE] = { 0 };
  uint8_t tag_out[SL_SE_AES_BLOCK_SIZE] = { 0 };
  uint8_t cbc_mac_state[SL_SE_AES_BLOCK_SIZE] = { 0 };
  uint8_t nonce_counter[SL_SE_AES_BLOCK_SIZE] = { 0 };
  uint32_t len_left;

  //Check input parameters
  if (ccm_ctx == NULL || cmd_ctx == NULL || key == NULL || iv == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (aad_len > 0 && aad == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (tag_len == 2 || tag_len > 16 || tag_len % 2 != 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (iv_len < 7 || iv_len > 13) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // q is the the octet length of Q which again is a bit string representation of
  // the octet length of the payload.
  q = 16 - 1 - (uint8_t) iv_len;

  // The parameter q determines the maximum length of the payload: by definition, p<2^(8*q),
  // where p is payload.
  if ((q < sizeof(total_message_length)) && (total_message_length >= (1UL << (q * 8)))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  memset(ccm_ctx, 0, sizeof(sl_se_ccm_multipart_context_t));

  // Format first input block B_O according to the formatting function:

  // 0        .. 0        flags
  // 1        .. iv_len   nonce (aka iv)
  // iv_len+1 .. 15       length
  //
  // With flags as (bits):
  // 7        0
  // 6        add present?
  // 5 .. 3   (t - 2) / 2
  // 2 .. 0   q - 1

  b[0] = 0;
  b[0] |= (aad_len > 0) << 6;
  b[0] |= ((tag_len - 2) / 2) << 3;
  b[0] |= q - 1;

  memcpy(b + 1, iv, iv_len);

  len_left = total_message_length;
  for (uint32_t i = 0; i < q; i++, len_left >>= 8) {
    b[15 - i] = (unsigned char)(len_left & 0xFF);
  }

  ccm_ctx->mode = mode;
  ccm_ctx->processed_message_length = 0;
  ccm_ctx->total_message_length = total_message_length;
  ccm_ctx->tag_len = tag_len;
  ccm_ctx->mode = mode;
  ccm_ctx->iv_len = iv_len;
  memcpy(ccm_ctx->iv, iv, iv_len);

  status = sl_se_aes_crypt_cbc(cmd_ctx,
                               key,
                               SL_SE_ENCRYPT,
                               SL_SE_AES_BLOCK_SIZE,
                               cbc_mac_state,
                               b,
                               tag_out);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // If there is additional data, update using CBC. Must be done
  // blockwise to achieve the same behaviour as CBC-MAC.
  if (aad_len > 0) {
    uint8_t use_len;
    len_left = aad_len;
    memset(b, 0, sizeof(b));
    // First block.
    b[0] = (unsigned char)((aad_len >> 8) & 0xFF);
    b[1] = (unsigned char)((aad_len) & 0xFF);
    use_len = len_left < SL_SE_AES_BLOCK_SIZE - 2 ? len_left : 16 - 2;
    memcpy(b + 2, aad, use_len);
    len_left -= use_len;
    aad += use_len;

    status = sl_se_aes_crypt_cbc(cmd_ctx,
                                 key,
                                 SL_SE_ENCRYPT,
                                 SL_SE_AES_BLOCK_SIZE,
                                 cbc_mac_state,
                                 b,
                                 tag_out);
    if (status != SL_STATUS_OK) {
      return status;
    }

    while (len_left) {
      use_len = len_left > 16 ? 16 : len_left;

      memset(b, 0, sizeof(b));
      memcpy(b, aad, use_len);
      status = sl_se_aes_crypt_cbc(cmd_ctx,
                                   key,
                                   SL_SE_ENCRYPT,
                                   SL_SE_AES_BLOCK_SIZE,
                                   cbc_mac_state,
                                   b,
                                   tag_out);

      if (status != SL_STATUS_OK) {
        return status;
      }
      len_left -= use_len;
      aad += use_len;
    }
  }

  memcpy(ccm_ctx->cbc_mac_state, cbc_mac_state, sizeof(cbc_mac_state));

  // Prepare nonce counter for encryption/decryption operation.
  nonce_counter[0] = q - 1;
  memcpy(nonce_counter + 1, iv, iv_len);
  memset(nonce_counter + 1 + iv_len, 0, q);
  nonce_counter[15] = 1;

  memcpy(ccm_ctx->nonce_counter, nonce_counter, sizeof(ccm_ctx->nonce_counter));

  return SL_STATUS_OK;
}

sl_status_t sl_se_ccm_multipart_update(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       size_t length,
                                       const uint8_t *input,
                                       uint8_t *output,
                                       size_t *output_length)
{
  sl_status_t status = SL_STATUS_OK;
  *output_length = 0;

  uint8_t out_buf[SL_SE_AES_BLOCK_SIZE] = { 0 };
  uint8_t empty[SL_SE_AES_BLOCK_SIZE * SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED] = { 0 };
  uint8_t b[SL_SE_AES_BLOCK_SIZE] = { 0 };

  size_t len_left;

  // Check input parameters.
  if (ccm_ctx == NULL || cmd_ctx == NULL || key == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (length == 0) {
    return SL_STATUS_OK;
  }

  // Check variable overflow
  if (ccm_ctx->processed_message_length > 0xFFFFFFFF - length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (ccm_ctx->processed_message_length + length > ccm_ctx->total_message_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (length > 0 && (input == NULL || output == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((uint32_t)output + length > (SRAM_BASE + SRAM_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Support partial overlap.
  if ((output > input) && (output < (input + length))) {
    memmove(output, input, length);
    input = output;
  }

  if (length + ccm_ctx->final_data_length < SL_SE_AES_BLOCK_SIZE && length < SL_SE_AES_BLOCK_SIZE && ccm_ctx->processed_message_length + length != ccm_ctx->total_message_length ) {
    if (ccm_ctx->final_data_length > SL_SE_AES_BLOCK_SIZE) {
      // Context is not valid.
      return SL_STATUS_INVALID_PARAMETER;
    }
    memcpy(ccm_ctx->final_data + ccm_ctx->final_data_length, input, length);
    ccm_ctx->final_data_length += length;
    *output_length = 0;
    return SL_STATUS_OK;
  }

  len_left = length + ccm_ctx->final_data_length;

  // Authenticate and {en,de}crypt the message.

  // The only difference between encryption and decryption is
  // the respective order of authentication and {en,de}cryption.
  while (len_left > 0 ) {
    uint8_t use_len = len_left > SL_SE_AES_BLOCK_SIZE ? SL_SE_AES_BLOCK_SIZE : len_left;

    memset(b, 0, sizeof(b));

    // Process data stored in context first.
    if (ccm_ctx->final_data_length > 0) {
      if (ccm_ctx->final_data_length > SL_SE_AES_BLOCK_SIZE) {
        // Context is not valid.
        return SL_STATUS_INVALID_PARAMETER;
      }
      memcpy(b, ccm_ctx->final_data, ccm_ctx->final_data_length);
      memcpy(b + ccm_ctx->final_data_length, input, SL_SE_AES_BLOCK_SIZE - ccm_ctx->final_data_length);
      input += SL_SE_AES_BLOCK_SIZE - ccm_ctx->final_data_length;
      ccm_ctx->final_data_length = 0;
    } else {
      memcpy(b, input, use_len);
      input += use_len;
    }
    if (ccm_ctx->mode == SL_SE_ENCRYPT) {
      // Authenticate input.
      status = sl_se_aes_crypt_cbc(cmd_ctx,
                                   key,
                                   SL_SE_ENCRYPT,
                                   SL_SE_AES_BLOCK_SIZE,
                                   ccm_ctx->cbc_mac_state,
                                   b,
                                   out_buf);

      if (status != SL_STATUS_OK) {
        return status;
      }
    }
    // Encrypt/decrypt data with CTR.
    status = sl_se_aes_crypt_ctr(cmd_ctx,
                                 key,
                                 use_len,
                                 NULL,
                                 ccm_ctx->nonce_counter,
                                 empty,
                                 b,
                                 output);

    if (ccm_ctx->mode == SL_SE_DECRYPT) {
      // Authenticate output.
      memset(b, 0, sizeof(b));
      memcpy(b, output, use_len);
      status = sl_se_aes_crypt_cbc(cmd_ctx,
                                   key,
                                   SL_SE_ENCRYPT,
                                   SL_SE_AES_BLOCK_SIZE,
                                   ccm_ctx->cbc_mac_state,
                                   b,
                                   out_buf);

      if (status != SL_STATUS_OK) {
        return status;
      }
    }
    ccm_ctx->processed_message_length += use_len;
    *output_length += use_len;
    len_left -= use_len;
    output += use_len;

    if (len_left < SL_SE_AES_BLOCK_SIZE && ((ccm_ctx->processed_message_length + len_left) != ccm_ctx->total_message_length)) {
      memcpy(ccm_ctx->final_data, input, len_left);
      ccm_ctx->final_data_length = len_left;
      break;
    }
  }

  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_se_ccm_multipart_finish(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       uint8_t *tag,
                                       uint8_t tag_size,
                                       uint8_t *output,
                                       uint8_t output_size,
                                       uint8_t *output_length)
{
  (void)output;
  uint8_t q;
  uint8_t ctr[SL_SE_AES_BLOCK_SIZE] = { 0 };
  uint8_t out_tag[SL_SE_AES_BLOCK_SIZE] = { 0 };
  //Check input parameters
  if (ccm_ctx == NULL || cmd_ctx == NULL || key == NULL || tag == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (tag_size < ccm_ctx->tag_len || output_size < ccm_ctx->final_data_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status = SL_STATUS_OK;

  // Reset CTR counter.
  q = 16 - 1 - (unsigned char) ccm_ctx->iv_len;

  ctr[0] = q - 1;
  memcpy(ctr + 1, ccm_ctx->iv, ccm_ctx->iv_len);

  // Encrypt the tag with CTR.
  uint8_t empty[SL_SE_AES_BLOCK_SIZE * SLI_SE_AES_CTR_NUM_BLOCKS_BUFFERED] = { 0 };
  status =  sl_se_aes_crypt_ctr(cmd_ctx,
                                key,
                                ccm_ctx->tag_len,
                                NULL,
                                ctr,
                                empty,
                                ccm_ctx->cbc_mac_state,
                                out_tag);

  if (status != SL_STATUS_OK) {
    memset(out_tag, 0, sizeof(out_tag));
    return status;
  }

  if (ccm_ctx->mode == SL_SE_DECRYPT) {
    if (memcmp_time_cst(tag, out_tag, ccm_ctx->tag_len) != 0) {
      memset(tag, 0, ccm_ctx->tag_len);
      return SL_STATUS_INVALID_SIGNATURE;
    }
  } else {
    memcpy(tag, out_tag, ccm_ctx->tag_len);
  }

  *output_length = 0;
  return SL_STATUS_OK;
}
#endif // SLI_SE_MAJOR_VERSION_ONE

#if defined(SLI_SE_MAJOR_VERSION_TWO)
/***************************************************************************//**
 *   Prepare a CCM streaming command context object to be used in subsequent
 *   CCM streaming function calls.
 ******************************************************************************/
sl_status_t sl_se_ccm_multipart_starts(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       sl_se_cipher_operation_t mode,
                                       uint32_t total_message_length,
                                       const uint8_t *iv,
                                       size_t iv_len,
                                       const uint8_t *add,
                                       size_t add_len,
                                       size_t tag_len)

{
  sl_status_t status = SL_STATUS_OK;
  uint8_t q;

  //Check input parameters
  if (ccm_ctx == NULL || cmd_ctx == NULL || key == NULL || iv == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (add_len > 0 && add == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (tag_len == 2 || tag_len > 16 || tag_len % 2 != 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (iv_len < 7 || iv_len > 13) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  q = 16 - 1 - (unsigned char) iv_len;
  if ((q < sizeof(ccm_ctx->total_message_length)) && (ccm_ctx->total_message_length >= (1UL << (q * 8)))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  memset(ccm_ctx, 0, sizeof(sl_se_ccm_multipart_context_t));

  ccm_ctx->mode = mode;
  ccm_ctx->processed_message_length = 0;
  ccm_ctx->total_message_length = total_message_length;
  ccm_ctx->tag_len = tag_len;
  memcpy(ccm_ctx->iv, iv, iv_len);

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;

  if (total_message_length == 0) {
    // The first encryption precomputes the tag in the event there is no more data.
    // For decryption, the pre-computed is compared to the input tag in
    // sl_se_ccm_multipart_finish.
    sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
    sli_se_datatransfer_t add_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
    sli_se_datatransfer_t tag_out = SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->mode_specific_buffer.tagbuf,
                                                                tag_len);

    sli_se_command_init(cmd_ctx,
                        SLI_SE_COMMAND_AES_CCM_ENCRYPT
                        | SLI_SE_COMMAND_OPTION_CONTEXT_WHOLE);

    sli_add_key_parameters(cmd_ctx, key, status);
    sli_se_mailbox_command_add_parameter(se_cmd, ((iv_len & 0xFFFF) << 16) | (tag_len & 0xFFFF));
    sli_se_mailbox_command_add_parameter(se_cmd, add_len);
    sli_se_mailbox_command_add_parameter(se_cmd, 0);

    // Add key metadata block to command
    sli_add_key_metadata(cmd_ctx, key, status);
    // Add key input block to command
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_mailbox_command_add_input(se_cmd, &iv_in);
    sli_se_mailbox_command_add_input(se_cmd, &add_in);

    sli_se_mailbox_command_add_output(se_cmd, &tag_out);

    status = sli_se_execute_and_wait(cmd_ctx);
    if (status != SL_STATUS_OK) {
      memset(ccm_ctx->mode_specific_buffer.tagbuf, 0, sizeof(ccm_ctx->mode_specific_buffer.tagbuf));
    }
    return status;
  }
  sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
  sli_se_datatransfer_t add_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
  sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->se_ctx, sizeof(ccm_ctx->se_ctx));

  sli_se_datatransfer_t message_length_in = SLI_SE_DATATRANSFER_DEFAULT(&total_message_length, sizeof(uint32_t));

  sli_se_command_init(cmd_ctx,
                      ((ccm_ctx->mode == SL_SE_DECRYPT)
                       ? SLI_SE_COMMAND_AES_CCM_DECRYPT : SLI_SE_COMMAND_AES_CCM_ENCRYPT)
                      | SLI_SE_COMMAND_OPTION_CONTEXT_START);

  sli_add_key_parameters(cmd_ctx, key, status);

  sli_se_mailbox_command_add_parameter(se_cmd, ((iv_len & 0xFFFF) << 16) | (tag_len & 0xFFFF));
  sli_se_mailbox_command_add_parameter(se_cmd, add_len);
  sli_se_mailbox_command_add_parameter(se_cmd, 0);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_mailbox_command_add_input(se_cmd, &message_length_in);

  sli_se_mailbox_command_add_input(se_cmd, &iv_in);
  sli_se_mailbox_command_add_input(se_cmd, &add_in);
  sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

  status = sli_se_execute_and_wait(cmd_ctx);
  if (status != SL_STATUS_OK) {
    memset(ccm_ctx->se_ctx, 0, sizeof(ccm_ctx->se_ctx));
    return status;
  }

  return status;
}
#endif

/***************************************************************************//**
 *   This function feeds an input buffer into an ongoing CCM computation.
 *   It is called between sl_se_ccm_multipart_starts() and sl_se_ccm_multipart_finish().
 *   Can be called repeatedly.
 ******************************************************************************/
#if defined(SLI_SE_MAJOR_VERSION_TWO)
sl_status_t sl_se_ccm_multipart_update(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       size_t length,
                                       const uint8_t *input,
                                       uint8_t *output,
                                       size_t *output_length)
{
  sl_status_t status = SL_STATUS_OK;

  // Check input parameters.
  if (ccm_ctx == NULL || cmd_ctx == NULL || key == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (length == 0) {
    return SL_STATUS_OK;
  }

  if (ccm_ctx->processed_message_length + length > ccm_ctx->total_message_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check variable overflow
  if (ccm_ctx->processed_message_length > 0xFFFFFFFF - length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (length > 0 && (input == NULL || output == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

#if !defined(SLI_SE_MANAGER_HOST_SYSTEM)
  if ((uint32_t)output + length > (SRAM_BASE + SRAM_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
#endif // SLI_SE_MANAGER_HOST_SYSTEM

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  *output_length = 0;

  // Approach:
  // Encrypt or decrypt regularly with context store. The crypto DMA must have input data in the 'END' operation, thus,
  // some data must be saved in the context.

  if ((ccm_ctx->final_data_length + length) < 16 && length < 16) {
    if (ccm_ctx->final_data_length > 16) {
      // Context is not valid.
      return SL_STATUS_INVALID_PARAMETER;
    }

    memcpy(ccm_ctx->mode_specific_buffer.final_data + ccm_ctx->final_data_length, input, length);
    ccm_ctx->final_data_length += length;
    return SL_STATUS_OK;
  }

  // If there is data in final_data, this must be processed first
  if (ccm_ctx->final_data_length) {
    if (ccm_ctx->final_data_length > 16) {
      // Context is not valid.
      return SL_STATUS_INVALID_PARAMETER;
    }

    // Fill up the remainder of the buffer.
    memcpy(ccm_ctx->mode_specific_buffer.final_data + ccm_ctx->final_data_length, input, 16 - ccm_ctx->final_data_length);

    if (ccm_ctx->processed_message_length + 16 == ccm_ctx->total_message_length ) {
      // The finish operation must have some data or the SE fails.
      ccm_ctx->final_data_length = 16;
      return SL_STATUS_OK;
    }

    sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->se_ctx, sizeof(ccm_ctx->se_ctx));

    sli_se_datatransfer_t data_in =
      SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->mode_specific_buffer.final_data, 16);
    sli_se_datatransfer_t data_out =
      SLI_SE_DATATRANSFER_DEFAULT(output, 16);

    sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->se_ctx, sizeof(ccm_ctx->se_ctx));

    sli_se_command_init(cmd_ctx,
                        ((ccm_ctx->mode == SL_SE_DECRYPT)
                         ? SLI_SE_COMMAND_AES_CCM_DECRYPT : SLI_SE_COMMAND_AES_CCM_ENCRYPT)
                        | SLI_SE_COMMAND_OPTION_CONTEXT_ADD);

    sli_add_key_parameters(cmd_ctx, key, status);

    sli_se_mailbox_command_add_parameter(se_cmd, 16);

    sli_add_key_metadata(cmd_ctx, key, status);
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
    sli_se_mailbox_command_add_input(se_cmd, &data_in);

    sli_se_mailbox_command_add_output(se_cmd, &data_out);
    sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

    status = sli_se_execute_and_wait(cmd_ctx);
    if (status != SL_STATUS_OK) {
      memset(output, 0, length);
      memset(ccm_ctx->se_ctx, 0, sizeof(ccm_ctx->se_ctx));
      *output_length = 0;
      return status;
    }
    ccm_ctx->processed_message_length += 16;
    output += 16;
    length -= (16 - ccm_ctx->final_data_length);
    input += (16 - ccm_ctx->final_data_length);
    ccm_ctx->final_data_length = 0;
    *output_length += 16;
  }

  if (length < 16) {
    memcpy(ccm_ctx->mode_specific_buffer.final_data, input, length);
    ccm_ctx->final_data_length += length;
    return SL_STATUS_OK;
  }

  // Run only multiples of 16 and store residue data in context
  if (length % 16 != 0) {
    uint8_t residue_data_length = length % 16;
    memcpy(ccm_ctx->mode_specific_buffer.final_data, input + (length - residue_data_length), residue_data_length);
    length -= residue_data_length;
    ccm_ctx->final_data_length = residue_data_length;
  }

  if ((ccm_ctx->total_message_length == ccm_ctx->processed_message_length + length) && !ccm_ctx->final_data_length) {
    // The finish operation must have some data or the SE fails.
    memcpy(ccm_ctx->mode_specific_buffer.final_data, input + (length - 16), 16);
    ccm_ctx->final_data_length = 16;
    length -= 16;
    if (!length) {
      return SL_STATUS_OK;
    }
  }

  sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->se_ctx, sizeof(ccm_ctx->se_ctx));

  sli_se_datatransfer_t data_in =
    SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_datatransfer_t data_out =
    SLI_SE_DATATRANSFER_DEFAULT(output, length);

  sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->se_ctx, sizeof(ccm_ctx->se_ctx));

  sli_se_command_init(cmd_ctx,
                      ((ccm_ctx->mode == SL_SE_DECRYPT)
                       ? SLI_SE_COMMAND_AES_CCM_DECRYPT : SLI_SE_COMMAND_AES_CCM_ENCRYPT)
                      | SLI_SE_COMMAND_OPTION_CONTEXT_ADD);

  sli_add_key_parameters(cmd_ctx, key, status);

  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_mailbox_command_add_output(se_cmd, &data_out);
  sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

  status = sli_se_execute_and_wait(cmd_ctx);
  if (status != SL_STATUS_OK) {
    memset(output, 0, length);
    memset(ccm_ctx->se_ctx, 0, sizeof(ccm_ctx->se_ctx));
    return status;
  }

  *output_length += length;
  ccm_ctx->processed_message_length += length;

  return status;
}
#endif

/***************************************************************************//**
 *   Finish a CCM streaming operation and return the resulting CCM tag.
 *   It is called after sl_se_ccm_multipart_update().
 ******************************************************************************/
#if defined(SLI_SE_MAJOR_VERSION_TWO)
sl_status_t sl_se_ccm_multipart_finish(sl_se_ccm_multipart_context_t *ccm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       uint8_t *tag,
                                       uint8_t tag_size,
                                       uint8_t *output,
                                       uint8_t output_size,
                                       uint8_t *output_length)
{
  //Check input parameters
  if (ccm_ctx == NULL || cmd_ctx == NULL || key == NULL || tag == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (tag_size < ccm_ctx->tag_len || output_size < ccm_ctx->final_data_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status = SL_STATUS_OK;

  if (ccm_ctx->total_message_length == 0) {
    if (ccm_ctx->mode == SL_SE_DECRYPT) {
      if (memcmp_time_cst(tag, ccm_ctx->mode_specific_buffer.tagbuf, ccm_ctx->tag_len) != 0) {
        memset(tag, 0, ccm_ctx->tag_len);
        return SL_STATUS_INVALID_SIGNATURE;
      }
    } else {
      memcpy(tag, ccm_ctx->mode_specific_buffer.tagbuf, ccm_ctx->tag_len);
    }
    return SL_STATUS_OK;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;

  sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->se_ctx, sizeof(ccm_ctx->se_ctx));

  sli_se_datatransfer_t data_in =
    SLI_SE_DATATRANSFER_DEFAULT(ccm_ctx->mode_specific_buffer.final_data, ccm_ctx->final_data_length);

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, ccm_ctx->final_data_length);
  sli_se_datatransfer_t tag_buf = SLI_SE_DATATRANSFER_DEFAULT(tag,
                                                              ccm_ctx->tag_len);

  sli_se_command_init(cmd_ctx,
                      ((ccm_ctx->mode == SL_SE_DECRYPT)
                       ? SLI_SE_COMMAND_AES_CCM_DECRYPT : SLI_SE_COMMAND_AES_CCM_ENCRYPT)
                      | SLI_SE_COMMAND_OPTION_CONTEXT_END);

  sli_add_key_parameters(cmd_ctx, key, status);

  sli_se_mailbox_command_add_parameter(se_cmd, (ccm_ctx->tag_len & 0xFFFF));

  sli_se_mailbox_command_add_parameter(se_cmd, ccm_ctx->final_data_length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_mailbox_command_add_output(se_cmd, &data_out);
  if (ccm_ctx->mode == SL_SE_DECRYPT) {
    sli_se_mailbox_command_add_input(se_cmd, &tag_buf);
  } else {
    sli_se_mailbox_command_add_output(se_cmd, &tag_buf);
  }
  status = sli_se_execute_and_wait(cmd_ctx);

  if (status != SL_STATUS_OK) {
    memset(tag, 0, ccm_ctx->tag_len);
    *output_length = 0;
    return status;
  }

  *output_length = ccm_ctx->final_data_length;

  return SL_STATUS_OK;
}
#endif

/***************************************************************************//**
 * This function calculates the full generic CMAC on the input buffer with
 * the provided key.
 ******************************************************************************/
sl_status_t sl_se_cmac(sl_se_command_context_t *cmd_ctx,
                       const sl_se_key_descriptor_t *key,
                       const unsigned char *input,
                       size_t input_len,
                       unsigned char *output)
{
  if (cmd_ctx == NULL || key == NULL || input == NULL || output == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status = SL_STATUS_OK;

  switch (key->type) {
    case SL_SE_KEY_TYPE_AES_128:
    case SL_SE_KEY_TYPE_AES_192:
    case SL_SE_KEY_TYPE_AES_256:
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_AES_CMAC);

  // Add key parameter to command.
  sli_add_key_parameters(cmd_ctx, key, status);

  // Message size parameter.
  sli_se_mailbox_command_add_parameter(se_cmd, input_len);

  // Key metadata.
  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  // Data input.
  sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(input, input_len);
  sli_se_mailbox_command_add_input(se_cmd, &in_data);

  // Data output.
  sli_se_datatransfer_t out_tag = SLI_SE_DATATRANSFER_DEFAULT(output, 16);
  sli_se_mailbox_command_add_output(se_cmd, &out_tag);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 *   Finish a CMAC streaming operation and return the resulting CMAC tag.
 *   It is called after sl_se_cmac_update().
 ******************************************************************************/
sl_status_t sl_se_cmac_multipart_finish(sl_se_cmac_multipart_context_t *cmac_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key,
                                        uint8_t *output)
{
  sl_status_t status = SL_STATUS_OK;

  if (cmac_ctx == NULL || cmd_ctx == NULL || key == NULL || output == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (cmac_ctx->length >= 16U) {
    // Compute previous input block by decryption of current data.
    status = sl_se_aes_crypt_ecb(cmd_ctx,
                                 key,
                                 SL_SE_DECRYPT,
                                 16U,
                                 cmac_ctx->data_out,
                                 cmac_ctx->state);  // Keep decrypted data in 'state'
  }

  if (status == SL_STATUS_OK) {
    // Feed previous block and unprocessed data (if any) to CMAC,
    // or on data < 16 just use accumulated data in context.
    sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
    sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_AES_CMAC);

    // Add key parameter to command.
    sli_add_key_parameters(cmd_ctx, key, status);

    // Message size parameter.
    if (cmac_ctx->length >= 16U) {
      sli_se_mailbox_command_add_parameter(se_cmd, 16U + (cmac_ctx->length & 0xFU));
    } else {
      sli_se_mailbox_command_add_parameter(se_cmd, cmac_ctx->length);
    }

    // Key metadata.
    sli_add_key_metadata(cmd_ctx, key, status);
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_datatransfer_t in_data1 = SLI_SE_DATATRANSFER_DEFAULT(cmac_ctx->state, 16U);
    sli_se_datatransfer_t in_data2 = SLI_SE_DATATRANSFER_DEFAULT(cmac_ctx->data_in,
                                                                 cmac_ctx->length & 0xFU);
    sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(cmac_ctx->data_in,
                                                                cmac_ctx->length);
    // Data input.
    if (cmac_ctx->length >= 16U) {
      // Collect data from two sources.
      in_data1.length &= ~SLI_SE_DATATRANSFER_REALIGN;
      sli_se_mailbox_command_add_input(se_cmd, &in_data1);
      sli_se_mailbox_command_add_input(se_cmd, &in_data2);
    } else {
      sli_se_mailbox_command_add_input(se_cmd, &in_data);
    }

    // Data output.
    sli_se_datatransfer_t out_tag = SLI_SE_DATATRANSFER_DEFAULT(output, 16U);
    sli_se_mailbox_command_add_output(se_cmd, &out_tag);

    status = sli_se_execute_and_wait(cmd_ctx);
  }
  return status;
}

/***************************************************************************//**
 *   Prepare a CMAC streaming command context object to be used in subsequent
 *   CMAC streaming function calls.
 ******************************************************************************/
sl_status_t sl_se_cmac_multipart_starts(sl_se_cmac_multipart_context_t *cmac_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key)
{
  if (cmac_ctx == NULL || cmd_ctx == NULL || key == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  switch (key->type) {
    case SL_SE_KEY_TYPE_AES_128:
    case SL_SE_KEY_TYPE_AES_192:
    case SL_SE_KEY_TYPE_AES_256:
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  memset(cmac_ctx, 0, sizeof(sl_se_cmac_multipart_context_t));
  return SL_STATUS_OK;
}

/***************************************************************************//**
 *   This function feeds an input buffer into an ongoing CMAC computation.
 *   It is called between sl_se_cmac_starts() and sl_se_cmac_finish().
 *   Can be called repeatedly.
 ******************************************************************************/
sl_status_t sl_se_cmac_multipart_update(sl_se_cmac_multipart_context_t *cmac_ctx,
                                        sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key,
                                        const uint8_t *input,
                                        size_t input_len)
{
  sl_status_t status = SL_STATUS_OK;

  if (cmac_ctx == NULL || cmd_ctx == NULL || key == NULL || input == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  size_t pending = cmac_ctx->length & 0xFU;

  if ((cmac_ctx->length / 16U) != ((cmac_ctx->length + input_len) / 16U)) {
    // Process one or more 16 byte blocks.
    size_t bytes_to_process = (pending + input_len) & 0xFFFFFFF0U;

    sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
    sli_se_command_init(cmd_ctx,
                        SLI_SE_COMMAND_AES_ENCRYPT
                        | SLI_SE_COMMAND_OPTION_MODE_CBC
                        | SLI_SE_COMMAND_OPTION_CONTEXT_ADD);

    // Add key parameters to command.
    sli_add_key_parameters(cmd_ctx, key, status);
    // Message size (number of bytes)
    sli_se_mailbox_command_add_parameter(se_cmd, bytes_to_process);

    // Add key metadata block to command.
    sli_add_key_metadata(cmd_ctx, key, status);
    // Add key input block to command.
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(cmac_ctx->state, 16U);
    sli_se_mailbox_command_add_input(se_cmd, &iv_in);

    // Data input, collect data from two sources.
    sli_se_datatransfer_t in1 = SLI_SE_DATATRANSFER_DEFAULT(cmac_ctx->data_in, pending);
    sli_se_datatransfer_t in2 = SLI_SE_DATATRANSFER_DEFAULT(input, bytes_to_process - pending);
    in1.length &= ~SLI_SE_DATATRANSFER_REALIGN;
    sli_se_mailbox_command_add_input(se_cmd, &in1);
    sli_se_mailbox_command_add_input(se_cmd, &in2);

    // Data output, discard everything except the last 16 bytes.
    sli_se_datatransfer_t out1 = SLI_SE_DATATRANSFER_DEFAULT(NULL, bytes_to_process - 16U);
    sli_se_datatransfer_t out2 = SLI_SE_DATATRANSFER_DEFAULT(cmac_ctx->data_out, 16U);
    out1.length |= SLI_SE_DATATRANSFER_DISCARD;
    out1.length &= ~SLI_SE_DATATRANSFER_REALIGN;
    sli_se_mailbox_command_add_output(se_cmd, &out1);
    sli_se_mailbox_command_add_output(se_cmd, &out2);

    sli_se_datatransfer_t iv_out = SLI_SE_DATATRANSFER_DEFAULT(cmac_ctx->state, 16U);
    sli_se_mailbox_command_add_output(se_cmd, &iv_out);

    status = sli_se_execute_and_wait(cmd_ctx);

    // Store leftover data.
    size_t leftover = pending + input_len - bytes_to_process;
    memcpy(cmac_ctx->data_in, input + input_len - leftover, leftover);
  } else {
    // Not a complete 16 byte block yet, save input data for later.
    memcpy(cmac_ctx->data_in + pending, input, input_len);
  }
  cmac_ctx->length += input_len;
  return status;
}

/***************************************************************************//**
 *   Compute a HMAC on a full message.
 ******************************************************************************/
sl_status_t sl_se_hmac(sl_se_command_context_t *cmd_ctx,
                       const sl_se_key_descriptor_t *key,
                       sl_se_hash_type_t hash_type,
                       const uint8_t *message,
                       size_t message_len,
                       uint8_t *output,
                       size_t output_len)
{
  if (cmd_ctx == NULL || key == NULL || output == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ( message == NULL && message_len != 0 ) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status = SL_STATUS_OK;
  uint32_t command_word;
  size_t hmac_len;

  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      command_word = SLI_SE_COMMAND_HMAC | SLI_SE_COMMAND_OPTION_HASH_SHA1;
      // SHA1 digest size is 20 bytes
      hmac_len = 20;
      break;

    case SL_SE_HASH_SHA224:
      command_word = SLI_SE_COMMAND_HMAC | SLI_SE_COMMAND_OPTION_HASH_SHA224;
      // SHA224 digest size is 28 bytes
      hmac_len = 28;
      break;

    case SL_SE_HASH_SHA256:
      command_word = SLI_SE_COMMAND_HMAC | SLI_SE_COMMAND_OPTION_HASH_SHA256;
      // SHA256 digest size is 32 bytes
      hmac_len = 32;
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_HASH_SHA384:
      command_word = SLI_SE_COMMAND_HMAC | SLI_SE_COMMAND_OPTION_HASH_SHA384;
      // SHA384 digest size is 48 bytes
      hmac_len = 48;
      break;

    case SL_SE_HASH_SHA512:
      command_word = SLI_SE_COMMAND_HMAC | SLI_SE_COMMAND_OPTION_HASH_SHA512;
      // SHA512 digest size is 64 bytes
      hmac_len = 64;
      break;

#endif
#if defined(_SILICON_LABS_32B_SERIES_3)
    case SL_SE_HASH_AES_MMO:
      command_word = SLI_SE_COMMAND_HMAC | SLI_SE_COMMAND_OPTION_HMAC_HASH_AES_MMO;
      // AES-MMO digest size is 16 bytes
      hmac_len = 16;
      break;
#endif
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  if (output_len < hmac_len) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, command_word);

  // Add key parameter to command.
  sli_add_key_parameters(cmd_ctx, key, status);

  // Message size parameter.
  sli_se_mailbox_command_add_parameter(se_cmd, message_len);

  // Key metadata.
  sli_add_key_metadata(cmd_ctx, key, status);

  sli_add_key_input(cmd_ctx, key, status);

  // Data input.
  sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(message, message_len);
  sli_se_mailbox_command_add_input(se_cmd, &in_data);

  // Data output.
  sli_se_datatransfer_t out_hmac = SLI_SE_DATATRANSFER_DEFAULT(output, hmac_len);
  sli_se_mailbox_command_add_output(se_cmd, &out_hmac);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * GCM buffer encryption or decryption.
 ******************************************************************************/
sl_status_t sl_se_gcm_crypt_and_tag(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_key_descriptor_t *key,
                                    sl_se_cipher_operation_t mode,
                                    size_t length,
                                    const unsigned char *iv,
                                    size_t iv_len,
                                    const unsigned char *add,
                                    size_t add_len,
                                    const unsigned char *input,
                                    unsigned char *output,
                                    size_t tag_len,
                                    unsigned char *tag)
{
  // Check input parameters.
  if (cmd_ctx == NULL || key == NULL || iv == NULL || tag == NULL
      || ((add_len > 0) && (add == NULL))
      || ((length > 0) && (input == NULL || output == NULL))
      || ((tag_len < 4) || (tag_len > 16))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  uint8_t tagbuf[16];
  sl_status_t status = SL_STATUS_OK;

  if (// IV length is required to be 96 bits for SE.
    (iv_len != 96 / 8)
    // AD is limited to 2^64 bits, so 2^61 bytes.
    // However, on 32 bit platforms, that amount of continous data cannot be
    // available.
    // || (((uint64_t)add_len) >> 61 != 0)
    ) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  switch (key->type) {
    case SL_SE_KEY_TYPE_AES_128:
    case SL_SE_KEY_TYPE_AES_192:
    case SL_SE_KEY_TYPE_AES_256:
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  if (mode == SL_SE_DECRYPT) {
    // Extract plaintext first.
    sli_se_command_init(cmd_ctx,
                        SLI_SE_COMMAND_AES_GCM_DECRYPT | ((tag_len & 0xFF) << 8));

    sli_add_key_parameters(cmd_ctx, key, status);
    sli_se_mailbox_command_add_parameter(se_cmd, add_len);
    sli_se_mailbox_command_add_parameter(se_cmd, length);

    sli_add_key_metadata(cmd_ctx, key, status);
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
    sli_se_mailbox_command_add_input(se_cmd, &iv_in);

    sli_se_datatransfer_t aad_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
    sli_se_mailbox_command_add_input(se_cmd, &aad_in);

    sli_se_datatransfer_t data_in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
    sli_se_mailbox_command_add_input(se_cmd, &data_in);

    sli_se_datatransfer_t tag_in = SLI_SE_DATATRANSFER_DEFAULT(tag, tag_len);
    sli_se_mailbox_command_add_input(se_cmd, &tag_in);

    sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
    if (output == NULL) {
      data_out.length |= SLI_SE_DATATRANSFER_DISCARD;
    }
    sli_se_mailbox_command_add_output(se_cmd, &data_out);

    // Execute GCM operation.
    status = sli_se_execute_and_wait(cmd_ctx);
    if ((status != SL_STATUS_OK) && (status != SL_STATUS_INVALID_SIGNATURE)) {
      memset(output, 0, length);
      return status;
    }

    // Re-encrypt the extracted plaintext to generate the tag to match.
    input = output;
    output = NULL;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_AES_GCM_ENCRYPT);

  sli_add_key_parameters(cmd_ctx, key, status);
  sli_se_mailbox_command_add_parameter(se_cmd, add_len);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
  sli_se_mailbox_command_add_input(se_cmd, &iv_in);

  sli_se_datatransfer_t aad_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
  sli_se_mailbox_command_add_input(se_cmd, &aad_in);

  sli_se_datatransfer_t data_in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  if (output == NULL) {
    data_out.length |= SLI_SE_DATATRANSFER_DISCARD;
  }
  sli_se_mailbox_command_add_output(se_cmd, &data_out);

  sli_se_datatransfer_t mac_out = SLI_SE_DATATRANSFER_DEFAULT(tagbuf, sizeof(tagbuf));
  sli_se_mailbox_command_add_output(se_cmd, &mac_out);

  // Execute GCM operation.
  status = sli_se_execute_and_wait(cmd_ctx);
  if (status == SL_STATUS_OK) {
    // For encryption, copy requested tag size to output tag buffer.
    memcpy(tag, tagbuf, tag_len);
  } else {
    memset(output, 0, length);
  }

  return status;
}

/***************************************************************************//**
 * GCM buffer decryption and authentication.
 ******************************************************************************/
sl_status_t sl_se_gcm_auth_decrypt(sl_se_command_context_t *cmd_ctx,
                                   const sl_se_key_descriptor_t *key,
                                   size_t length,
                                   const unsigned char *iv,
                                   size_t iv_len,
                                   const unsigned char *add,
                                   size_t add_len,
                                   const unsigned char *input,
                                   unsigned char *output,
                                   size_t tag_len,
                                   const unsigned char *tag)
{
  // Check input parameters.
  if (cmd_ctx == NULL || key == NULL || iv == NULL || tag == NULL
      || ((add_len > 0) && (add == NULL))
      || ((length > 0) && (input == NULL || output == NULL))
      || ((tag_len < 4) || (tag_len > 16))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status = SL_STATUS_OK;

  if (// IV length is required to be 96 bits for SE.
    (iv_len != 96 / 8)
    // AD is limited to 2^64 bits, so 2^61 bytes.
    // However, on 32 bit platforms, that amount of continous data cannot be
    // available.
    // || (((uint64_t)add_len) >> 61 != 0)
    ) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  switch (key->type) {
    case SL_SE_KEY_TYPE_AES_128: // Fallthrough
    case SL_SE_KEY_TYPE_AES_192: // Fallthrough
    case SL_SE_KEY_TYPE_AES_256:
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx,
                      SLI_SE_COMMAND_AES_GCM_DECRYPT | ((tag_len & 0xFF) << 8));

  sli_add_key_parameters(cmd_ctx, key, status);
  sli_se_mailbox_command_add_parameter(se_cmd, add_len);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
  sli_se_mailbox_command_add_input(se_cmd, &iv_in);

  sli_se_datatransfer_t aad_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
  sli_se_mailbox_command_add_input(se_cmd, &aad_in);

  sli_se_datatransfer_t data_in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_datatransfer_t tag_in = SLI_SE_DATATRANSFER_DEFAULT(tag, tag_len);
  sli_se_mailbox_command_add_input(se_cmd, &tag_in);

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  if (output == NULL) {
    data_out.length |= SLI_SE_DATATRANSFER_DISCARD;
  }
  sli_se_mailbox_command_add_output(se_cmd, &data_out);

  // Execute GCM operation.
  status = sli_se_execute_and_wait(cmd_ctx);

  if (status != SL_STATUS_OK) {
    memset(output, 0, length);
  }

  return status;
}

#if defined(SLI_SE_MAJOR_VERSION_TWO)
/***************************************************************************//**
 * GCM  multipart encryption/decryption, initial stage.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_starts(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       sl_se_cipher_operation_t mode,
                                       const uint8_t *iv,
                                       size_t iv_len,
                                       const uint8_t *add,
                                       size_t add_len)
{
  sl_status_t status = SL_STATUS_OK;

  // Check input parameters.
  if (gcm_ctx == NULL || cmd_ctx == NULL || key == NULL || iv == NULL
      || (add_len > 0 && add == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((iv_len != 12)
      // AD are limited to 2^64 bits, so 2^61 bytes.
      // However, on 32 bit platforms, that amount of continous data cannot be
      // available.
      // || (((uint64_t)add_len) >> 61 != 0)
      ) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  memset(gcm_ctx, 0, sizeof(sl_se_gcm_multipart_context_t));

  gcm_ctx->mode = mode;
  gcm_ctx->len = 0;
  gcm_ctx->add_len = add_len;

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;

  // The start context requires some data, either additional data or input data.
  // Case add_len > 0: Run start command with additonal data to create ctx_out.
  // Case add_len = 0: Store iv in gcm_ctx and run start function with input data
  // in sl_se_gcm_multipart_update. In the case of zero input data or
  // input data < 16 run sl_se_gcm_auth_decrypt()/sl_se_gcm_crypt_and_tag() in
  // sl_se_gcm_multipart_finish.
  if ( add_len > 0 ) {
    sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
    sli_se_datatransfer_t add_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
    sli_se_datatransfer_t ctx_out =
      SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx, sizeof(gcm_ctx->se_ctx));

    sli_se_command_init(cmd_ctx,
                        (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
                         : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
                        | SLI_SE_COMMAND_OPTION_CONTEXT_START);

    sli_add_key_parameters(cmd_ctx, key, status);
    sli_se_mailbox_command_add_parameter(se_cmd, add_len);
    sli_se_mailbox_command_add_parameter(se_cmd, 0);

    sli_add_key_metadata(cmd_ctx, key, status);
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_mailbox_command_add_input(se_cmd, &iv_in);
    sli_se_mailbox_command_add_input(se_cmd, &add_in);
    sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

    status = sli_se_execute_and_wait(cmd_ctx);
    if (status != SL_STATUS_OK) {
      memset(gcm_ctx->se_ctx, 0, sizeof(gcm_ctx->se_ctx));
      return status;
    }
    gcm_ctx->first_operation = false;
  } else {
    memcpy(gcm_ctx->se_ctx, iv, iv_len);
    gcm_ctx->first_operation = true;
  }
  return SL_STATUS_OK;
}

#else
/***************************************************************************//**
 * GCM  multipart encryption/decryption, initial stage.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_starts(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       sl_se_cipher_operation_t mode,
                                       const uint8_t *iv,
                                       size_t iv_len,
                                       const uint8_t *add,
                                       size_t add_len)
{
  sl_status_t status = SL_STATUS_OK;

  // Check input parameters.
  if (gcm_ctx == NULL || cmd_ctx == NULL || key == NULL || iv == NULL
      || (add_len > 0 && add == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((iv_len != 12)
      // AD are limited to 2^64 bits, so 2^61 bytes.
      // However, on 32 bit platforms, that amount of continous data cannot be
      // available.
      // || (((uint64_t)add_len) >> 61 != 0)
      ) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  memset(gcm_ctx, 0, sizeof(sl_se_gcm_multipart_context_t));

  gcm_ctx->mode = mode;
  gcm_ctx->len = 0;
  gcm_ctx->add_len = add_len;

  // The start context requires some data, either additional data or input data.
  // Case add_len > 0: Run start command with additonal data to create ctx_out.
  // Case add_len = 0: Store iv in gcm_ctx and run start function with input data
  // in sl_se_gcm_multipart_update. In the case of zero input data or
  // input data < 16 run sl_se_gcm_auth_decrypt()/sl_se_gcm_crypt_and_tag() in
  // sl_se_gcm_multipart_finish.
  if ( add_len > 0 ) {
    // Encrypt: Compute tag and store it in context and output tag in finish.
    // Decrypt: Compute tag and store it in context and compare it to the
    // input tag in finish to verify it.

    // Explanation:The end-context in finish is currently not supporting 0 input data
    // for this config. For add_len = 0 and input_length = 0 we can run
    // sl_se_gcm_auth_decrypt()/sl_se_gcm_crypt_and_tag() in finish, so this is only
    // an issue for 0 input data and add_len != 0.
    sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
    sli_se_datatransfer_t iv_in = SLI_SE_DATATRANSFER_DEFAULT(iv, iv_len);
    sli_se_datatransfer_t add_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
    sli_se_datatransfer_t tag_out = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->tagbuf,
                                                                sizeof(gcm_ctx->tagbuf));
    sli_se_command_init(cmd_ctx,
                        SLI_SE_COMMAND_AES_GCM_ENCRYPT
                        | SLI_SE_COMMAND_OPTION_CONTEXT_WHOLE);

    sli_add_key_parameters(cmd_ctx, key, status);
    sli_se_mailbox_command_add_parameter(se_cmd, add_len);
    sli_se_mailbox_command_add_parameter(se_cmd, 0);

    sli_add_key_metadata(cmd_ctx, key, status);
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_mailbox_command_add_input(se_cmd, &iv_in);
    sli_se_mailbox_command_add_input(se_cmd, &add_in);
    sli_se_mailbox_command_add_output(se_cmd, &tag_out);

    status = sli_se_execute_and_wait(cmd_ctx);
    if (status != SL_STATUS_OK) {
      memset(gcm_ctx->tagbuf, 0, sizeof(gcm_ctx->tagbuf));
      return status;
    }

    sli_se_datatransfer_t ctx_out =
      SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx, sizeof(gcm_ctx->se_ctx));

    // Reuse the values of the command context object from the previous
    // operation, and only update the command word and the output data pointer.
    cmd_ctx->command.command =
      (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
       : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
      | SLI_SE_COMMAND_OPTION_CONTEXT_START;

    cmd_ctx->command.data_out = &ctx_out;

    status = sli_se_execute_and_wait(cmd_ctx);
    if (status != SL_STATUS_OK) {
      memset(gcm_ctx->se_ctx, 0, sizeof(gcm_ctx->se_ctx));
      return status;
    }
    gcm_ctx->first_operation = false;
  } else {
    memcpy(gcm_ctx->se_ctx, iv, iv_len);
    gcm_ctx->first_operation = true;
  }
  return SL_STATUS_OK;
}
#endif

#if defined(SLI_SE_MAJOR_VERSION_TWO)
/***************************************************************************//**
 * GCM multipart encryption/decryption, update stage.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_update(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       size_t length,
                                       const uint8_t *input,
                                       uint8_t *output,
                                       size_t *output_length)
{
  sl_status_t status = SL_STATUS_OK;
  uint8_t stored_res_length = 0;

  // Check input parameters.
  if (cmd_ctx == NULL || key == NULL || gcm_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (length == 0) {
    return SL_STATUS_OK;
  }
  if (length > 0 && (input == NULL || output == NULL || output_length == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  *output_length = 0;

  // Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes.
  // Also check for possible overflow.
  if (gcm_ctx->len + length < gcm_ctx->len
      || (uint64_t)gcm_ctx->len + length > 0xFFFFFFFE0ULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;

  // The update context only support an input length as a multiple of 16. Hence, there
  // there is a few cases that can happen.

  // Always:
  //  Case length = 0: Return SL_STATUS_OK

  // If there is no data in gcm_ctx->final_data:
  //  Case length < 16: Store data in gcm_ctx->final_data and return SL_STATUS_OK
  //  Case length == 16: Run update as normal
  //  Case length > 16 and length is a multiple of 16: Run update as normal
  //  Case length > 16 and length is not a multiple of 16: Run update as normal on the largest multiple
  //  and save the residue bytes in gcm_ctx->final_data.

  // If there is data in gcm_ctx->final_data:
  //  Case final_data_length + length < 16: Store input data in gcm_ctx and return SL_STATUS_OKAY
  //  Case final_data_length + length > 16: Add data to fill up the gcm_ctx->final_data-buffer, run update
  //  on the gcm_ctx->final_data-buffer and finally run update as explained above on the rest of the data.

  if (gcm_ctx->final_data_length) {
    if ((gcm_ctx->final_data_length + length) < 16) {
      memcpy(gcm_ctx->final_data + gcm_ctx->final_data_length, input, length);
      gcm_ctx->final_data_length += length;
      *output_length = 0;
      return SL_STATUS_OK;
    }
    stored_res_length = 16 - gcm_ctx->final_data_length;
    memcpy(gcm_ctx->final_data + gcm_ctx->final_data_length, input, stored_res_length);

    //The gcm_ctx->se_ctx buffer contain iv data with length 12 if gcm_ctx->first_operation = true
    sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                  gcm_ctx->first_operation ? 12 : sizeof(gcm_ctx->se_ctx));

    sli_se_datatransfer_t data_in =
      SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->final_data, 16);

    sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, 16);
    sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                sizeof(gcm_ctx->se_ctx));

    sli_se_command_init(cmd_ctx,
                        (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
                         : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
                        | (gcm_ctx->first_operation ? SLI_SE_COMMAND_OPTION_CONTEXT_START : SLI_SE_COMMAND_OPTION_CONTEXT_ADD));

    sli_add_key_parameters(cmd_ctx, key, status);
    sli_se_mailbox_command_add_parameter(se_cmd, 0);
    sli_se_mailbox_command_add_parameter(se_cmd, 16);

    sli_add_key_metadata(cmd_ctx, key, status);
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
    sli_se_mailbox_command_add_input(se_cmd, &data_in);

    sli_se_mailbox_command_add_output(se_cmd, &data_out);

    sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

    status = sli_se_execute_and_wait(cmd_ctx);

    if (status != SL_STATUS_OK) {
      memset(gcm_ctx->se_ctx, 0, sizeof(gcm_ctx->se_ctx));
      return status;
    }
    gcm_ctx->first_operation = false;
    gcm_ctx->len += 16;
    output += 16;

    if ((length - stored_res_length) < 16) {
      memcpy(gcm_ctx->final_data, input + stored_res_length, length - stored_res_length);
      gcm_ctx->final_data_length = length - stored_res_length;
      *output_length = 16;
      return SL_STATUS_OK;
    }

    length -= stored_res_length;
    gcm_ctx->final_data_length = 0;
    *output_length += 16;
  }
  if (length % 16 != 0) {
    if (length > 16) {
      // Input length is larger than, and a non multiple of, 16
      memcpy(gcm_ctx->final_data, input + stored_res_length + (length - (length % 16)), length % 16);
      gcm_ctx->final_data_length = length % 16;
      length -= length % 16;
    } else {
      // Input length is not a multiple of 16
      memcpy(gcm_ctx->final_data, input + stored_res_length, length);
      gcm_ctx->final_data_length = length;
      *output_length = 0;
      return SL_STATUS_OK;
    }
  }
  gcm_ctx->len += length;

  sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                gcm_ctx->first_operation ? 12 : sizeof(gcm_ctx->se_ctx));

  sli_se_datatransfer_t data_in =
    SLI_SE_DATATRANSFER_DEFAULT(input + stored_res_length, length);

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                              sizeof(gcm_ctx->se_ctx));

  sli_se_command_init(cmd_ctx,
                      (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
                       : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
                      | (gcm_ctx->first_operation ? SLI_SE_COMMAND_OPTION_CONTEXT_START : SLI_SE_COMMAND_OPTION_CONTEXT_ADD));

  sli_add_key_parameters(cmd_ctx, key, status);
  sli_se_mailbox_command_add_parameter(se_cmd, 0);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_mailbox_command_add_output(se_cmd, &data_out);

  sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

  status = sli_se_execute_and_wait(cmd_ctx);

  if (status != SL_STATUS_OK) {
    memset(gcm_ctx->se_ctx, 0, sizeof(gcm_ctx->se_ctx));
    return status;
  }
  *output_length += length;
  gcm_ctx->first_operation = false;
  return SL_STATUS_OK;
}

#else // SLI_SE_MAJOR_VERSION_ONE
/***************************************************************************//**
 * GCM multipart encryption/decryption, update stage.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_update(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       size_t length,
                                       const uint8_t *input,
                                       uint8_t *output,
                                       size_t *output_length)
{
  sl_status_t status = SL_STATUS_OK;
  uint8_t stored_res_length = 0;

  // Check input parameters.
  if (cmd_ctx == NULL || key == NULL || gcm_ctx == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (length == 0) {
    return SL_STATUS_OK;
  }
  if (length > 0 && (input == NULL || output == NULL || output_length == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  *output_length = 0;

  // Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes.
  // Also check for possible overflow.
  if (gcm_ctx->len + length < gcm_ctx->len
      || (uint64_t)gcm_ctx->len + length > 0xFFFFFFFE0ULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;

  // The finish command will return the wrong tag or INVALID SIGNATURE if there is no
  // input data. There is no way to know when sl_se_gcm_multipart_update is called for the last time, so there must
  // always be data stored.
  // The update context only support an input length as a multiple of 16. Hence, there
  // there is a few cases that can happen.

  // Always:
  //  Case length = 0: Return SL_STATUS_OK

  // If there is no data in gcm_ctx->final_data
  //  Case length < 16: Store data in gcm_ctx->final_data and return SL_STATUS_OK
  //  Case length > 16 and length is not a multiple of 16: Run update as normal on the largest multiple
  //  and save the residue bytes in gcm_ctx->final_data.
  //  Case length == 16: store the current se_ctx in gcm_ctx->previous_se_ctx and store input data in final_data,
  //  then run update as normal on the input data.
  //  Case length > 16 and length is a multible of 16: run update twice, one with all input data and one with
  //  all input data except the last 16 bytes. The out_ctx from each run are stored in gcm_ctx->se_ctx and
  //  gcm_ctx->previous_se_ctx respectively. The last 16 bytes are then stored in final_data.
  //  NOTE:output_length will include all encrypted/decrypted data.

  // If there is data in gcm_ctx->final_data
  //  Case final_data_length + length < 16: Store input data in gcm_ctx and return SL_STATUS_OKAY
  //  Case final_data_length + length > 16: Add data to fill up the gcm_ctx->final_data-buffer, run update
  //  on the gcm_ctx->final_data-buffer and finally run update as explained above on the rest of the data.

  // Our drivers only support full or no overlap between input and output
  // buffers. So in the case of partial overlap, copy the input buffer into
  // the output buffer and process it in place as if the buffers fully
  // overlapped.
  if ((output > input) && (output < (input + length))) {
    memmove(output, input, length);
    input = output;
  }

  // Check for data in final_data_length.
  if (gcm_ctx->final_data_length && gcm_ctx->final_data_length != 16) {
    if ((gcm_ctx->final_data_length + length) < 16) {
      memcpy(gcm_ctx->final_data + gcm_ctx->final_data_length, input, length);
      gcm_ctx->final_data_length += length;
      *output_length = 0;
      return SL_STATUS_OK;
    }

    if ((gcm_ctx->final_data_length + length) == 16) {
      memcpy(gcm_ctx->previous_se_ctx, gcm_ctx->se_ctx, sizeof(gcm_ctx->se_ctx));
    }
    stored_res_length = 16 - gcm_ctx->final_data_length;
    memcpy(gcm_ctx->final_data + gcm_ctx->final_data_length, input, stored_res_length);

    sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                  gcm_ctx->first_operation ? 12 : sizeof(gcm_ctx->se_ctx));

    sli_se_datatransfer_t data_in =
      SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->final_data, 16);

    sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, 16);
    sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                sizeof(gcm_ctx->se_ctx));

    sli_se_command_init(cmd_ctx,
                        (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
                         : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
                        | (gcm_ctx->first_operation ? SLI_SE_COMMAND_OPTION_CONTEXT_START : SLI_SE_COMMAND_OPTION_CONTEXT_ADD));

    sli_add_key_parameters(cmd_ctx, key, status);
    sli_se_mailbox_command_add_parameter(se_cmd, 0);
    sli_se_mailbox_command_add_parameter(se_cmd, 16);

    sli_add_key_metadata(cmd_ctx, key, status);
    sli_add_key_input(cmd_ctx, key, status);

    sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
    sli_se_mailbox_command_add_input(se_cmd, &data_in);

    sli_se_mailbox_command_add_output(se_cmd, &data_out);

    sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

    status = sli_se_execute_and_wait(cmd_ctx);

    if (status != SL_STATUS_OK) {
      memset(gcm_ctx->se_ctx, 0, sizeof(gcm_ctx->se_ctx));
      return status;
    }
    gcm_ctx->first_operation = false;
    gcm_ctx->len += 16;
    output += 16;

    if ((gcm_ctx->final_data_length + length) == 16) {
      gcm_ctx->final_data_length = 16;
      *output_length = 16;
      return SL_STATUS_OK;
    }

    if ((length - stored_res_length) < 16) {
      memcpy(gcm_ctx->final_data, input + stored_res_length, length - stored_res_length);
      gcm_ctx->final_data_length = length - stored_res_length;
      *output_length = 16;
      return SL_STATUS_OK;
    }

    length -= stored_res_length;
    gcm_ctx->final_data_length = 0;
    *output_length += 16;
  }
  if (length % 16 != 0) {
    if (length > 16) {
      //Input length is larger than, and a non multiple of, 16
      memcpy(gcm_ctx->final_data, input + stored_res_length + (length - (length % 16)), length % 16);
      gcm_ctx->final_data_length = length % 16;
      length -= length % 16;
    } else {
      //Input length is not a multiple of 16
      memcpy(gcm_ctx->final_data, input + stored_res_length, length);
      gcm_ctx->final_data_length = length;
      *output_length = 0;
      return SL_STATUS_OK;
    }
  } else {
    if (length > 16) {
      //If length is larger than, and a multiple of, 16, we must compute a context without the last 16 bytes
      //and store it as the previous context in case there is no more data.
      memcpy(gcm_ctx->final_data, input + stored_res_length + (length - 16), 16);
      gcm_ctx->final_data_length = 16;

      //The gcm_ctx->se_ctx buffer contain iv data with length 12 if gcm_ctx->first_operation = true
      sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                    gcm_ctx->first_operation ? 12 : sizeof(gcm_ctx->se_ctx));

      sli_se_datatransfer_t data_in =
        SLI_SE_DATATRANSFER_DEFAULT(input + stored_res_length, length - 16);

      sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length - 16);
      sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                  sizeof(gcm_ctx->se_ctx));

      sli_se_command_init(cmd_ctx,
                          (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
                           : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
                          | (gcm_ctx->first_operation ? SLI_SE_COMMAND_OPTION_CONTEXT_START : SLI_SE_COMMAND_OPTION_CONTEXT_ADD));

      sli_add_key_parameters(cmd_ctx, key, status);
      sli_se_mailbox_command_add_parameter(se_cmd, 0);
      sli_se_mailbox_command_add_parameter(se_cmd, length - 16);

      sli_add_key_metadata(cmd_ctx, key, status);
      sli_add_key_input(cmd_ctx, key, status);

      sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
      sli_se_mailbox_command_add_input(se_cmd, &data_in);

      sli_se_mailbox_command_add_output(se_cmd, &data_out);

      sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

      status = sli_se_execute_and_wait(cmd_ctx);

      if (status != SL_STATUS_OK) {
        memset(gcm_ctx->se_ctx, 0, sizeof(gcm_ctx->se_ctx));
        return status;
      }

      // Only process the last 16 bytes in the last operation.
      output += (length - 16);
      input += (length - 16);
      gcm_ctx->first_operation = false;
      gcm_ctx->len += (length - 16);
      *output_length += (length - 16);

      length = 16;

      memcpy(gcm_ctx->previous_se_ctx, gcm_ctx->se_ctx, sizeof(gcm_ctx->se_ctx));
    } else {
      memcpy(gcm_ctx->previous_se_ctx, gcm_ctx->se_ctx, sizeof(gcm_ctx->se_ctx));
      memcpy(gcm_ctx->final_data, input + stored_res_length, length);
      gcm_ctx->final_data_length = length;
    }
  }
  gcm_ctx->len += length;

  // The gcm_ctx->se_ctx buffer contain iv data with length 12 if gcm_ctx->first_operation = true
  sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                                gcm_ctx->first_operation ? 12 : sizeof(gcm_ctx->se_ctx));

  sli_se_datatransfer_t data_in =
    SLI_SE_DATATRANSFER_DEFAULT(input + stored_res_length, length);

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  sli_se_datatransfer_t ctx_out = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx,
                                                              sizeof(gcm_ctx->se_ctx));

  sli_se_command_init(cmd_ctx,
                      (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
                       : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
                      | (gcm_ctx->first_operation ? SLI_SE_COMMAND_OPTION_CONTEXT_START : SLI_SE_COMMAND_OPTION_CONTEXT_ADD));

  sli_add_key_parameters(cmd_ctx, key, status);
  sli_se_mailbox_command_add_parameter(se_cmd, 0);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_mailbox_command_add_output(se_cmd, &data_out);

  sli_se_mailbox_command_add_output(se_cmd, &ctx_out);

  status = sli_se_execute_and_wait(cmd_ctx);

  if (status != SL_STATUS_OK) {
    memset(gcm_ctx->se_ctx, 0, sizeof(gcm_ctx->se_ctx));
    return status;
  }
  *output_length += length;
  gcm_ctx->first_operation = false;
  return SL_STATUS_OK;
}
#endif

/***************************************************************************//**
 * GCM multipart encryption/decryption, finish stage.
 ******************************************************************************/
sl_status_t sl_se_gcm_multipart_finish(sl_se_gcm_multipart_context_t *gcm_ctx,
                                       sl_se_command_context_t *cmd_ctx,
                                       const sl_se_key_descriptor_t *key,
                                       uint8_t *tag,
                                       uint8_t tag_length,
                                       uint8_t *output,
                                       uint8_t output_size,
                                       uint8_t *output_length)
{
  sl_status_t status = SL_STATUS_OK;
  uint32_t tmpbuf[4];
  uint8_t length;
  if (cmd_ctx == NULL || key == NULL || gcm_ctx == NULL || tag == NULL || tag_length < 4 || tag_length > 16) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((gcm_ctx->final_data_length != 16)
      && (output_size < gcm_ctx->final_data_length)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  length = gcm_ctx->final_data_length;
  gcm_ctx->len += ((length % 16 != 0) ? length : 0);

  #if defined(SLI_SE_MAJOR_VERSION_ONE)
  if ((gcm_ctx->add_len > 0) && (gcm_ctx->len == 0)) {
    if (gcm_ctx->mode == SL_SE_DECRYPT) {
      if (memcmp_time_cst(tag, gcm_ctx->tagbuf, tag_length)) {
        memset(tag, 0, tag_length);
        return SL_STATUS_INVALID_SIGNATURE;
      }
    } else {
      memcpy(tag, gcm_ctx->tagbuf, tag_length);
    }
    return SL_STATUS_OK;
  }
  #endif

#if defined(SLI_SE_MAJOR_VERSION_ONE)
  // For xG21 devices, since the multipart finish command cannot handle cases without
  // more data being passed as part of the finish call, there are two cases for which
  // a finish call can condense into a one-shot operation:
  // 1. The 'first operation' flag is set, meaning no multipart context has been started
  // 2. There was no AAD input and the total input length equals 16 bytes. In such a case,
  //    all information needed for a one-shot operation is still present in the context,
  //    being the 16 bytes of input in the lookback buffer. In such a case, be careful to
  //    not return ciphertext/plaintext to the user a second time, since it has already
  //    been returned as part of the initial call to `_update`.
  if (gcm_ctx->first_operation || (gcm_ctx->add_len == 0 && gcm_ctx->len == 16)) {
#else
  // Devices xG23 or newer support a finish call without data, so the only case for
  // condensing a multipart finish operation into a one-shot operation is when the
  // 'first operation' flag is set, meaning no multipart context has been started
  if (gcm_ctx->first_operation) {
#endif
    if (gcm_ctx->mode == SL_SE_ENCRYPT) {
      status = sl_se_gcm_crypt_and_tag(cmd_ctx,
                                       key,
                                       gcm_ctx->mode,
                                       length,
                                       gcm_ctx->se_ctx, //iv
                                       12,  //iv_len
                                       NULL,
                                       0,
                                       gcm_ctx->final_data,
                                       (length < 16 ? output : (unsigned char*)tmpbuf),
                                       tag_length,
                                       tag);
    } else {
      status = sl_se_gcm_auth_decrypt(cmd_ctx,
                                      key,
                                      length,
                                      gcm_ctx->se_ctx, //iv
                                      12,   //iv_len
                                      NULL,
                                      0,
                                      gcm_ctx->final_data,
                                      (length < 16 ? output : (unsigned char*)tmpbuf),
                                      tag_length,
                                      tag);
    }
    if (status != SL_STATUS_OK) {
      *output_length = 0;
      memset(tag, 0, tag_length);
      return status;
    }
    if (length < 16) {
      *output_length = length;
    } else {
      *output_length = 0;
    }
    return SL_STATUS_OK;
  }

  // Construct GCM LenA || LenC block into temporary buffer
  tmpbuf[0] = __REV(gcm_ctx->add_len >> 29);
  tmpbuf[1] = __REV((gcm_ctx->add_len << 3) & 0xFFFFFFFFUL);
  tmpbuf[2] = __REV(gcm_ctx->len >> 29);
  tmpbuf[3] = __REV((gcm_ctx->len << 3) & 0xFFFFFFFFUL);

  sli_se_datatransfer_t data_in =
    SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->final_data, length);

  #if defined(SLI_SE_MAJOR_VERSION_ONE)
  sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT((length % 16 != 0 || length == 0) ? gcm_ctx->se_ctx : gcm_ctx->previous_se_ctx, sizeof(gcm_ctx->previous_se_ctx));
  #else
  sli_se_datatransfer_t iv_ctx_in = SLI_SE_DATATRANSFER_DEFAULT(gcm_ctx->se_ctx, sizeof(gcm_ctx->se_ctx));
  #endif

  sli_se_datatransfer_t lenalenc_in = SLI_SE_DATATRANSFER_DEFAULT(&tmpbuf[0],
                                                                  sizeof(tmpbuf));

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  if (length == 16) {
    data_out.data = NULL;
    data_out.length |= SLI_SE_DATATRANSFER_DISCARD;
  }

  sli_se_datatransfer_t gcm_tag = SLI_SE_DATATRANSFER_DEFAULT(tag, tag_length);

  sli_se_command_init(cmd_ctx,
                      (gcm_ctx->mode == SL_SE_DECRYPT ? SLI_SE_COMMAND_AES_GCM_DECRYPT
                       : SLI_SE_COMMAND_AES_GCM_ENCRYPT)
                      | tag_length << 8 | SLI_SE_COMMAND_OPTION_CONTEXT_END);

  sli_add_key_parameters(cmd_ctx, key, status);
  sli_se_mailbox_command_add_parameter(se_cmd, 0);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_mailbox_command_add_input(se_cmd, &iv_ctx_in);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);
  sli_se_mailbox_command_add_input(se_cmd, &lenalenc_in);

  if (gcm_ctx->mode == SL_SE_DECRYPT) {
    sli_se_mailbox_command_add_input(se_cmd, &gcm_tag);
  }

  sli_se_mailbox_command_add_output(se_cmd, &data_out);

  if (gcm_ctx->mode == SL_SE_ENCRYPT) {
    sli_se_mailbox_command_add_output(se_cmd, &gcm_tag);
  }

  status = sli_se_execute_and_wait(cmd_ctx);

  if (status != SL_STATUS_OK) {
    if (gcm_ctx->mode == SL_SE_ENCRYPT) {
      memset(tag, 0, tag_length);
    }
    *output_length = 0;
    return status;
  }

  if (length < 16) {
    *output_length = length;
  } else {
    *output_length = 0;
  }

  return SL_STATUS_OK;
}

#if (defined(_SILICON_LABS_SECURITY_FEATURE) \
  && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT))
/***************************************************************************//**
 * ChaCha20 buffer encryption/decryption, as defined by RFC8439 section 2.4.
 ******************************************************************************/
sl_status_t sl_se_chacha20_crypt(sl_se_command_context_t *cmd_ctx,
                                 sl_se_cipher_operation_t mode,
                                 const sl_se_key_descriptor_t *key,
                                 size_t length,
                                 const unsigned char initial_counter[4],
                                 const unsigned char nonce[12],
                                 const unsigned char *input,
                                 unsigned char *output)
{
  if (cmd_ctx == NULL || key == NULL || initial_counter == NULL || nonce == NULL
      || input == NULL || output == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (key->type != SL_SE_KEY_TYPE_CHACHA20) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  sli_se_command_init(cmd_ctx,
                      (mode == SL_SE_ENCRYPT
                       ? SLI_SE_COMMAND_CHACHA20_ENCRYPT
                       : SLI_SE_COMMAND_CHACHA20_DECRYPT)
                      | SLI_SE_COMMAND_OPTION_CONTEXT_WHOLE);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, status);
  // Message size (number of bytes)
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, status);

  // Add initial counter to command
  sli_se_datatransfer_t counter = SLI_SE_DATATRANSFER_DEFAULT(initial_counter, 4);
  sli_se_mailbox_command_add_input(se_cmd, &counter);

  // Add nonce/IV to command
  sli_se_datatransfer_t iv = SLI_SE_DATATRANSFER_DEFAULT(nonce, 12);
  sli_se_mailbox_command_add_input(se_cmd, &iv);

  // Add input data to command
  sli_se_datatransfer_t in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &in);

  // Request output data from command
  sli_se_datatransfer_t out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  sli_se_mailbox_command_add_output(se_cmd, &out);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * ChaCha20-Poly1305 authenticated encryption with additional data, as defined
 * by RFC8439 section 2.8.
 ******************************************************************************/
sl_status_t sl_se_chacha20_poly1305_encrypt_and_tag(sl_se_command_context_t *cmd_ctx,
                                                    const sl_se_key_descriptor_t *key,
                                                    size_t length,
                                                    const unsigned char nonce[12],
                                                    const unsigned char *add, size_t add_len,
                                                    const unsigned char *input,
                                                    unsigned char *output,
                                                    unsigned char *tag)
{
  // Check input parameters.
  if (cmd_ctx == NULL || key == NULL || nonce == NULL
      || ((add_len > 0) && (add == NULL))
      || ((length > 0) && (input == NULL))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  if (key->type != SL_SE_KEY_TYPE_CHACHA20) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_CHACHAPOLY_ENCRYPT);

  sli_add_key_parameters(cmd_ctx, key, status);
  sli_se_mailbox_command_add_parameter(se_cmd, add_len);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_datatransfer_t nonce_in = SLI_SE_DATATRANSFER_DEFAULT(nonce, 12);
  sli_se_mailbox_command_add_input(se_cmd, &nonce_in);

  sli_se_datatransfer_t aad_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
  sli_se_mailbox_command_add_input(se_cmd, &aad_in);

  sli_se_datatransfer_t data_in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  if (output == NULL) {
    data_out.length |= SLI_SE_DATATRANSFER_DISCARD;
  }
  sli_se_mailbox_command_add_output(se_cmd, &data_out);

  sli_se_datatransfer_t mac_out = SLI_SE_DATATRANSFER_DEFAULT(tag, 16);
  if (tag == NULL) {
    mac_out.length |= SLI_SE_DATATRANSFER_DISCARD;
  }
  sli_se_mailbox_command_add_output(se_cmd, &mac_out);

  // Execute AEAD operation.
  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * ChaCha20-Poly1305 authenticated decryption with additional data, as defined
 * by RFC8439 section 2.8.
 ******************************************************************************/
sl_status_t sl_se_chacha20_poly1305_auth_decrypt(sl_se_command_context_t *cmd_ctx,
                                                 const sl_se_key_descriptor_t *key,
                                                 size_t length,
                                                 const unsigned char nonce[12],
                                                 const unsigned char *add, size_t add_len,
                                                 const unsigned char *input,
                                                 unsigned char *output,
                                                 const unsigned char tag[16])
{
  // Check input parameters.
  if (cmd_ctx == NULL || key == NULL || nonce == NULL || tag == NULL
      || ((add_len > 0) && (add == NULL))
      || ((length > 0) && (input == NULL))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  if (key->type != SL_SE_KEY_TYPE_CHACHA20) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_CHACHAPOLY_DECRYPT);

  sli_add_key_parameters(cmd_ctx, key, status);
  sli_se_mailbox_command_add_parameter(se_cmd, add_len);
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  sli_add_key_metadata(cmd_ctx, key, status);
  sli_add_key_input(cmd_ctx, key, status);

  sli_se_datatransfer_t nonce_in = SLI_SE_DATATRANSFER_DEFAULT(nonce, 12);
  sli_se_mailbox_command_add_input(se_cmd, &nonce_in);

  sli_se_datatransfer_t aad_in = SLI_SE_DATATRANSFER_DEFAULT(add, add_len);
  sli_se_mailbox_command_add_input(se_cmd, &aad_in);

  sli_se_datatransfer_t data_in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &data_in);

  sli_se_datatransfer_t mac_in = SLI_SE_DATATRANSFER_DEFAULT(tag, 16);
  sli_se_mailbox_command_add_input(se_cmd, &mac_in);

  sli_se_datatransfer_t data_out = SLI_SE_DATATRANSFER_DEFAULT(output, length);
  if (output == NULL) {
    data_out.length |= SLI_SE_DATATRANSFER_DISCARD;
  }
  sli_se_mailbox_command_add_output(se_cmd, &data_out);

  // Execute AEAD operation.
  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * Generate a MAC (message authentication code) for a given message, according
 * to RFC8439 section 2.8 but bypassing the encryption step.
 ******************************************************************************/
sl_status_t sl_se_poly1305_genkey_tag(sl_se_command_context_t *cmd_ctx,
                                      const sl_se_key_descriptor_t *key,
                                      size_t length,
                                      const unsigned char nonce[12],
                                      const unsigned char *input,
                                      unsigned char *tag)
{
  if (cmd_ctx == NULL || key == NULL || nonce == NULL || tag == NULL
      || ((length > 0) && (input == NULL))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (key->type != SL_SE_KEY_TYPE_CHACHA20) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;

  sli_se_command_init(cmd_ctx, SLI_SE_COMMAND_POLY1305_KEY_MAC);

  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, status);
  // Message size (number of bytes)
  sli_se_mailbox_command_add_parameter(se_cmd, length);

  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, status);

  // Add nonce/IV to command
  sli_se_datatransfer_t iv = SLI_SE_DATATRANSFER_DEFAULT(nonce, 12);
  sli_se_mailbox_command_add_input(se_cmd, &iv);

  // Add input data to command
  sli_se_datatransfer_t in = SLI_SE_DATATRANSFER_DEFAULT(input, length);
  sli_se_mailbox_command_add_input(se_cmd, &in);

  // Request tag from command
  sli_se_datatransfer_t out = SLI_SE_DATATRANSFER_DEFAULT(tag, 16);
  sli_se_mailbox_command_add_output(se_cmd, &out);

  return sli_se_execute_and_wait(cmd_ctx);
}
#endif

/** @} (end addtogroup sl_se) */

#if defined(_SILICON_LABS_32B_SERIES_3)

/***************************************************************************//**
 *   Prepare a HMAC streaming command context object to be used in subsequent
 *   HMAC streaming function calls.
 ******************************************************************************/
sl_status_t sl_se_hmac_multipart_starts(sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key,
                                        sl_se_hash_type_t hash_type,
                                        const uint8_t *message,
                                        size_t message_len,
                                        uint8_t *state_out,
                                        size_t state_out_len)
{
  if (cmd_ctx == NULL || key == NULL || message == NULL || state_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status = SL_STATUS_OK;
  uint32_t command_word;
  size_t hmac_state_len;

  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_START | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA1;
      hmac_state_len = 20;
      break;

    case SL_SE_HASH_SHA224:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_START | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA224;
      hmac_state_len = 32;
      break;

    case SL_SE_HASH_SHA256:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_START | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA256;
      hmac_state_len = 32;
      break;

    case SL_SE_HASH_SHA384:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_START | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA384;
      hmac_state_len = 64;
      break;

    case SL_SE_HASH_SHA512:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_START | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA512;
      hmac_state_len = 64;
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }
  hmac_state_len += 8u; // adding 8 bytes for storing the HMAC multipart internal states
  if (state_out_len < hmac_state_len) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, command_word);

  // Add key parameter to command.
  sli_add_key_parameters(cmd_ctx, key, status);

  // Message size parameter.
  sli_se_mailbox_command_add_parameter(se_cmd, message_len);

  // Key metadata.
  sli_add_key_metadata(cmd_ctx, key, status);

  sli_add_key_input(cmd_ctx, key, status);

  // Data input.
  sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(message, message_len);
  sli_se_mailbox_command_add_input(se_cmd, &in_data);

  // Data output.
  sli_se_datatransfer_t out_hmac_state = SLI_SE_DATATRANSFER_DEFAULT(state_out, hmac_state_len);
  sli_se_mailbox_command_add_output(se_cmd, &out_hmac_state);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 *   This function feeds an input buffer into an ongoing HMAC computation.
 ******************************************************************************/
sl_status_t sl_se_hmac_multipart_update(sl_se_command_context_t *cmd_ctx,
                                        sl_se_hash_type_t hash_type,
                                        const uint8_t *message,
                                        size_t message_len,
                                        uint8_t *state_in_out,
                                        size_t state_in_out_len)
{
  if (cmd_ctx == NULL || message == NULL || state_in_out == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  uint32_t command_word;
  size_t hmac_state_len;

  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_UPDATE | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA1;
      hmac_state_len = 20;
      break;

    case SL_SE_HASH_SHA224:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_UPDATE | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA224;
      hmac_state_len = 32;
      break;

    case SL_SE_HASH_SHA256:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_UPDATE | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA256;
      hmac_state_len = 32;
      break;

    case SL_SE_HASH_SHA384:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_UPDATE | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA384;
      hmac_state_len = 64;
      break;

    case SL_SE_HASH_SHA512:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_UPDATE | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA512;
      hmac_state_len = 64;
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }
  hmac_state_len += 8u; // adding 8 bytes for storing the HMAC multipart internal states
  if (state_in_out_len != hmac_state_len) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, command_word);

  // Message size parameter.
  sli_se_mailbox_command_add_parameter(se_cmd, message_len);

  // Data input.
  sli_se_datatransfer_t in_hmac_state = SLI_SE_DATATRANSFER_DEFAULT(state_in_out, hmac_state_len);
  sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(message, message_len);
  sli_se_mailbox_command_add_input(se_cmd, &in_hmac_state);
  sli_se_mailbox_command_add_input(se_cmd, &in_data);

  sli_se_datatransfer_t out_hmac_state = SLI_SE_DATATRANSFER_DEFAULT(state_in_out, hmac_state_len);
  sli_se_mailbox_command_add_output(se_cmd, &out_hmac_state);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 *   Finish a HMAC streaming operation and return the resulting HMAC.
 ******************************************************************************/
sl_status_t sl_se_hmac_multipart_finish(sl_se_command_context_t *cmd_ctx,
                                        const sl_se_key_descriptor_t *key,
                                        sl_se_hash_type_t hash_type,
                                        const uint8_t *message,
                                        size_t message_len,
                                        uint8_t *state_in,
                                        size_t state_in_len,
                                        uint8_t *output,
                                        size_t output_len)
{
  if (cmd_ctx == NULL || key == NULL || message == NULL || state_in == NULL || output == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_mailbox_command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status = SL_STATUS_OK;
  uint32_t command_word;
  size_t hmac_state_len, hmac_len;

  switch (hash_type) {
    case SL_SE_HASH_SHA1:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_FINISH | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA1;
      hmac_state_len = 20;
      hmac_len = 20;
      break;

    case SL_SE_HASH_SHA224:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_FINISH | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA224;
      hmac_state_len = 32;
      hmac_len = 28;
      break;

    case SL_SE_HASH_SHA256:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_FINISH | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA256;
      hmac_state_len = 32;
      hmac_len = 32;
      break;

    case SL_SE_HASH_SHA384:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_FINISH | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA384;
      hmac_state_len = 64;
      hmac_len = 48;
      break;

    case SL_SE_HASH_SHA512:
      command_word = SLI_SE_COMMAND_HMAC_STREAMING_FINISH | SLI_SE_COMMAND_OPTION_HMAC_HASH_SHA512;
      hmac_state_len = 64;
      hmac_len = 64;
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
      break;
  }
  hmac_state_len += 8u; // adding 8 bytes for storing the HMAC multipart internal states

  if (state_in_len != hmac_state_len) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (output_len < hmac_len) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_se_command_init(cmd_ctx, command_word);

  // Add key parameter to command.
  sli_add_key_parameters(cmd_ctx, key, status);

  // Message size parameter.
  sli_se_mailbox_command_add_parameter(se_cmd, message_len);

  // Key metadata.
  sli_add_key_metadata(cmd_ctx, key, status);

  sli_add_key_input(cmd_ctx, key, status);

  // Data input.
  sli_se_datatransfer_t state_in_data = SLI_SE_DATATRANSFER_DEFAULT(state_in, hmac_state_len);
  sli_se_datatransfer_t in_data = SLI_SE_DATATRANSFER_DEFAULT(message, message_len);
  sli_se_mailbox_command_add_input(se_cmd, &state_in_data);
  sli_se_mailbox_command_add_input(se_cmd, &in_data);

  // Data output.
  sli_se_datatransfer_t out_hmac = SLI_SE_DATATRANSFER_DEFAULT(output, hmac_len);
  sli_se_mailbox_command_add_output(se_cmd, &out_hmac);

  return sli_se_execute_and_wait(cmd_ctx);
}

#endif // defined(_SILICON_LABS_32B_SERIES_3)

#endif // defined(SLI_MAILBOX_COMMAND_SUPPORTED)
