/** @file
  Copyright (C) 2005 - 2015, Apple Inc.  All rights reserved.<BR>

  This program and the accompanying materials have not been licensed.
  Neither is its usage, its redistribution, in source or binary form,
  licensed, nor implicitely or explicitely permitted, except when
  required by applicable law.

  Unless required by applicable law or agreed to in writing, software
  distributed is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
  OR CONDITIONS OF ANY KIND, either express or implied.
**/

#include <AppleEfi.h>

#include <Library/AppleDriverLib.h>
#include <Library/AppleKeyMapLib.h>

#include "AppleKeyMapImplInternal.h"

// KeyMapCreateKeyStrokesBufferImpl
/** Creates a new key set with a given number of keys allocated.  The index
    within the database is returned.

  @param[in]  This             A pointer to the protocol instance.
  @param[in]  KeyBufferLength  The amount of keys to allocate for the key set.
  @param[out] Index            The assigned index of the created key set.

  @return                       Returned is the status of the operation.
  @retval EFI_SUCCESS           A key set with the given number of keys
                                allocated has been created.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval other                 An error returned by a sub-operation.
**/
EFI_STATUS
EFIAPI
KeyMapCreateKeyStrokesBufferImpl (
  IN  APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN  UINTN                            KeyBufferSize,
  OUT UINTN                            *Index
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  UINTN                    BufferSize;
  APPLE_KEY                *Memory;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  ASSERT (This != NULL);
  ASSERT (KeyBufferSize > 0);
  ASSERT (Index != NULL);

  Aggregator = APPLE_KEY_MAP_AGGREGATOR_FROM_DATABASE_PROTOCOL (This);

  if (Aggregator->KeyBuffer != NULL) {
    gBS->FreePool ((VOID *)Aggregator->KeyBuffer);
  }

  BufferSize                 = (Aggregator->KeyBuffersSize + KeyBufferSize);
  Aggregator->KeyBuffersSize = BufferSize;
  Memory                     = EfiLibAllocateZeroPool (BufferSize);
  Aggregator->KeyBuffer      = Memory;
  Status                     = EFI_OUT_OF_RESOURCES;

  if (Memory != NULL) {
    KeyStrokesInfo = EfiLibAllocateZeroPool (
                       sizeof (*KeyStrokesInfo)
                         + (KeyBufferSize * sizeof (APPLE_KEY))
                       );
    Status         = EFI_OUT_OF_RESOURCES;

    if (KeyStrokesInfo != NULL) {
      KeyStrokesInfo->Hdr.Signature     = APPLE_KEY_STROKES_INFO_SIGNATURE;
      KeyStrokesInfo->Hdr.KeyBufferSize = KeyBufferSize;
      KeyStrokesInfo->Hdr.Index         = Aggregator->NextKeyStrokeIndex;
      ++Aggregator->NextKeyStrokeIndex;
      
      InsertTailList (
        &Aggregator->KeyStrokesInfoList,
        &KeyStrokesInfo->Hdr.This
        );

      Status = EFI_SUCCESS;
      *Index = KeyStrokesInfo->Hdr.Index;
    }
  }

  ASSERT_EFI_ERROR (Status);

  return Status;
}

// KeyMapRemoveKeyStrokesBufferImpl
/** Removes a key set specified by its index from the database.

  @param[in] This   A pointer to the protocol instance.
  @param[in] Index  The index of the key set to remove.

  @return                Returned is the status of the operation.
  @retval EFI_SUCCESS    The specified key set has been removed.
  @retval EFI_NOT_FOUND  No key set could be found for the given index.
  @retval other          An error returned by a sub-operation.
**/
EFI_STATUS
EFIAPI
KeyMapRemoveKeyStrokesBufferImpl (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  ASSERT (This != NULL);

  Aggregator     = APPLE_KEY_MAP_AGGREGATOR_FROM_DATABASE_PROTOCOL (This);
  KeyStrokesInfo = KeyMapGetKeyStrokesByIndex (
                     &Aggregator->KeyStrokesInfoList,
                     Index
                     );

  Status         = EFI_NOT_FOUND;

  if (KeyStrokesInfo != NULL) {
    Aggregator->KeyBuffersSize -= KeyStrokesInfo->Hdr.KeyBufferSize;

    RemoveEntryList (&KeyStrokesInfo->Hdr.This);
    gBS->FreePool ((VOID *)KeyStrokesInfo);

    Status = EFI_SUCCESS;
  }

  ASSERT_EFI_ERROR (Status);

  return Status;
}

// KeyMapSetKeyStrokeBufferKeysImpl
/** Sets the keys of a key set specified by its index to the given Keys Buffer.

  @param[in] This          A pointer to the protocol instance.
  @param[in] Index         The index of the key set to edit.
  @param[in] Modifiers     The key modifiers manipulating the given keys.
  @param[in] NumberOfKeys  The number of keys contained in Keys.
  @param[in] Keys          An array of keys to add to the specified key set.

  @return                       Returned is the status of the operation.
  @retval EFI_SUCCESS           The given keys were set for the specified key
                                set.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval EFI_NOT_FOUND         No key set could be found for the given index.
  @retval other                 An error returned by a sub-operation.
**/
EFI_STATUS
EFIAPI
KeyMapSetKeyStrokeBufferKeysImpl (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index,
  IN APPLE_MODIFIER_MAP               Modifiers,
  IN UINTN                            NumberOfKeys,
  IN APPLE_KEY                        *Keys
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  ASSERT (This != NULL);
  ASSERT (NumberOfKeys > 0);
  ASSERT (Keys != NULL);

  Aggregator     = APPLE_KEY_MAP_AGGREGATOR_FROM_DATABASE_PROTOCOL (This);
  KeyStrokesInfo = KeyMapGetKeyStrokesByIndex (&Aggregator->KeyStrokesInfoList, Index);
  Status         = EFI_NOT_FOUND;

  if (KeyStrokesInfo != NULL) {
    Status = EFI_OUT_OF_RESOURCES;

    if (KeyStrokesInfo->Hdr.KeyBufferSize >= NumberOfKeys) {
      KeyStrokesInfo->Hdr.NumberOfKeys = NumberOfKeys;
      KeyStrokesInfo->Hdr.Modifiers    = Modifiers;

      EfiCopyMem (
        (VOID *)&KeyStrokesInfo->Keys,
        (VOID *)Keys, (NumberOfKeys * sizeof (*Keys))
        );

      Status = EFI_SUCCESS;
    }
  }

  ASSERT_EFI_ERROR (Status);

  return Status;
}
