 /**
  * File:    fsfuncs.c
  *
  *   Copyright (c) 2021, Georgios Sokianos
  *
  *   This file is part of MediaVault
  *
  * Author:   Georgios Sokianos (walkero@gmail.com)
  * Date:     May 2021
  *
  * Summary of File:
  *
  *   This file contains code that has to do with
  *   file system paths and files.
  *
  */


#include "globals.h"
#include "httpfuncs.h"

STRPTR getFilePath(STRPTR filename)
{
  STRPTR appPath = IExec->AllocVecTags(sizeof(char) * 255,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  BPTR fileLock = IDOS->Lock(filename, SHARED_LOCK);
  if (fileLock)
  {
    BPTR folderLock = IDOS->ParentDir(fileLock);
    IDOS->NameFromLock(folderLock, appPath, sizeof(char) * 255);

    IDOS->UnLock(folderLock);
    IDOS->UnLock(fileLock);
    //IExec->FreeVec(appPath);
    return appPath;
  }

  return NULL;
}

STRPTR getParentPath(STRPTR filename)
{
  STRPTR appPath = IExec->AllocVecTags(sizeof(char) * 255,
      AVT_Type,            MEMF_SHARED,
      AVT_ClearWithValue,  "\0",
      TAG_DONE);

  BPTR fileLock = IDOS->Lock(filename, SHARED_LOCK);
  if (fileLock)
  {
    BPTR folderLock = IDOS->ParentDir(fileLock);
    BPTR parentLock = IDOS->ParentDir(folderLock);
    IDOS->NameFromLock(parentLock, appPath, sizeof(char) * 255);

    IDOS->UnLock(parentLock);
    IDOS->UnLock(folderLock);
    IDOS->UnLock(fileLock);
    //IExec->FreeVec(appPath);
    return appPath;
  }

  return NULL;
  
}

BOOL downloadLatestUpdate(STRPTR releaseUrl)
{
  return downloadFile(releaseUrl, (STRPTR)"mediavault_update", (STRPTR)"RAM:");
}

int32 unarcFile(STRPTR filename, STRPTR targetFolder)
{
  BPTR fileLock = IDOS->Lock(filename, SHARED_LOCK);
  if (fileLock)
  {
    STRPTR cmd = IUtility->ASPrintf("c:xadUnFile FROM \"%s\" DESTDIR %s OW Q ", filename, targetFolder);

    int32 result = IDOS->SystemTags( cmd,
        SYS_Input,    ZERO,
        SYS_Output,   NULL,
        SYS_Error,    ZERO,
        SYS_Asynch,   FALSE,
        TAG_DONE);

    IDOS->UnLock(fileLock);
    return result;
  }

  return -2;
}

BOOL fileExists(STRPTR file)
{
  BPTR fileLock = IDOS->Lock(file, SHARED_LOCK);
  if (fileLock)
  {
    IDOS->UnLock(fileLock);
    return TRUE;
  }

  return FALSE;
}
