



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
    IDOS->NameFromLock(fileLock, appPath, sizeof(char) * 255);

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
