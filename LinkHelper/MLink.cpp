/*
	ZY ´úÂëÀ´×Ô
	chromium/tree/master/native_client_sdk/src/build_tools/MkLink/Mklink.c
*/
// MLink.cpp: implementation of the CMLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MLink.h"

#include <TCHAR.H>
#include <WINDOWS.H>
#include <Shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum { kLargeBuf = 1024, kSmallBuf = 256 } ;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMLink::CMLink()
{

}

CMLink::~CMLink()
{

}

#if defined(_MSC_VER)
/* Ensure these are treated as functions and not inlined as intrinsics, or disable /Oi */
#pragma warning(disable:4164)  /* intrinsic function not declared */
#pragma function(memcpy, memset, memcmp)
#endif

HMODULE hNrDll = NULL;
#if _MSC_VER >1800
#define  ZY_TYPEDEF
#else
#define  ZY_TYPEDEF typedef
#endif // _MSC_VER >1800

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif

#if _MSC_VER < 1600
	#include "VcHelper.h"
#else
	#include <winternl.h>
#endif // _MSC_VER < 1500

ZY_TYPEDEF NTSTATUS (NTAPI *fNtCreateFile) (PHANDLE FileHandle,
								 ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes,
								 PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize,
								 ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition,
								 ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
ZY_TYPEDEF NTSTATUS (NTAPI *fNtClose) (HANDLE Handle);

HMODULE hKernel32 = NULL;
ZY_TYPEDEF BOOL (WINAPI *fCreateHardLink) (TCHAR * linkFileName,
								TCHAR * existingFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

ZY_TYPEDEF BOOL (WINAPI *fCreateSymbolicLink) (TCHAR * linkFileName,
									TCHAR * existingFileName, DWORD flags);

#undef CreateHardLink
#undef CreateSymbolicLink
#ifdef UNICODE
#define CreateHardLink "CreateHardLinkW"
#define CreateSymbolicLink "CreateSymbolicLinkW"
#else
#define CreateHardLink "CreateHardLinkA"
#define CreateSymbolicLink "CreateSymbolicLinkA"
#endif


BOOL CMLink::MakeHardLink(TCHAR *linkFileName, TCHAR *existingFileName)
{
	bool bRet = false;
	DWORD dwErr = 0;
	if (!hKernel32)
		hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));
	if (hKernel32) {
#if _MSC_VER >1800
		if (!fCreateHardLink)
			fCreateHardLink = GetProcAddress(hKernel32, CreateHardLink);
		if (fCreateHardLink)
			return fCreateHardLink(linkFileName, existingFileName, NULL);
#else
		fCreateHardLink harkLink;
		harkLink = (fCreateHardLink)GetProcAddress(hKernel32, CreateHardLink);
		if (harkLink)
			bRet = harkLink(linkFileName, existingFileName, NULL);
		dwErr = GetLastError();
#endif _MSC_VER >1800
	}
  return bRet;
}

BOOL CMLink::MakeSymLink(TCHAR *linkFileName, TCHAR *existingFileName, BOOL dirLink)
{
  TCHAR *f1 = (TCHAR *)HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR)*kLargeBuf);
  TCHAR *f2 = (TCHAR *)HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR)*kLargeBuf);
  SECURITY_ATTRIBUTES sec_attr = { sizeof (SECURITY_ATTRIBUTES), NULL, FALSE};
  OBJECT_ATTRIBUTES obj_attr;
  IO_STATUS_BLOCK io_block;
  TCHAR *p, *q;
  HANDLE f;
  BOOL status;
  if (!f1 || !f2)
    return FALSE;
  lstrcpy(f1, linkFileName);
  for (p = f1; p[0]; p++)
    if (p[0] == _T('/'))
      p[0] = _T('\\');
  q = f1;
  while (q[0]) {
    p = q;
    do {
      q++;
    } while (q[0] && q[0] != '\\');
  }
  if (p[0] = '\\') {
    TCHAR c = p[1];
    p[1] = 0;
    status = GetVolumeInformation(f1, NULL, 0, NULL, NULL, NULL,
                                  f2, sizeof(f2));
    p[1] = c;
  } else {
    status = GetVolumeInformation(NULL, NULL, 0, NULL, NULL, NULL,
                                  f2, sizeof(f2));
  }
  /* If it's NFS then we can create real symbolic link. */
  if (!lstrcmpi(f2, _T("NFS"))) {
    lstrcpy(f2, existingFileName);
    for (p = f2; p[0]; p++)
      if (p[0] == _T('\\'))
        p[0] = _T('/');
    if (!hNrDll)
      hNrDll = LoadLibrary(_T("NTDLL.DLL"));
    if (hNrDll) {
#if _MSC_VER >1500
     if (!fNtCreateFile)
        fNtCreateFile = GetProcAddress(hNrDll, "NtCreateFile");
      if (!fNtClose)
        fNtClose = GetProcAddress(hNrDll, "NtClose");
      if (fNtCreateFile && fNtClose) {
#else
		fNtCreateFile  ntCreate;
		ntCreate = (fNtCreateFile)GetProcAddress(hNrDll, "NtCreateFile");
		fNtClose ntClose;
		ntClose = (fNtClose)GetProcAddress(hNrDll, "NtClose");
		if (ntCreate && ntClose) {
#endif //_MSC_VER >1500
        struct _LINK_INFO{
          ULONG offset;
          UCHAR flags;
          UCHAR nameLength;
          USHORT valueLength;
          TCHAR name[21];
          /* To prevent troubles with alignment */
          TCHAR value[kLargeBuf*sizeof(WCHAR)];
        } *ea_info =(_LINK_INFO*) HeapAlloc(GetProcessHeap(),
                               HEAP_ZERO_MEMORY,
                               sizeof(*ea_info));
        WCHAR *fn = (WCHAR *)HeapAlloc(GetProcessHeap(),
                              0,
                              sizeof(TCHAR)*kLargeBuf);
        UNICODE_STRING n = { lstrlen(f1), kLargeBuf, fn };
        ea_info->nameLength = 20;
        lstrcpy(ea_info->name, _T("NfsSymlinkTargetName"));
#ifdef UNICODE
        lstrcpy(fn, f1);
        lstrcpy((LPWSTR)ea_info->value, existingFileName);
#else
        MultiByteToWideChar(CP_ACP, 0, f1, -1, fn, kLargeBuf);
        MultiByteToWideChar(CP_ACP, 0, existingFileName, -1,
                            (LPWSTR)ea_info->value,
                            sizeof(ea_info->value)/sizeof(WCHAR));
#endif
        ea_info->valueLength =
            lstrlenW((LPWSTR)ea_info->value)*sizeof(WCHAR);
        InitializeObjectAttributes(&obj_attr, &n, OBJ_CASE_INSENSITIVE,
                                   NULL, NULL);
#if _MSC_VER >1500       
		status = fNtCreateFile(
            &f, FILE_WRITE_DATA | FILE_WRITE_EA | SYNCHRONIZE, &obj_attr,
            &io_block, NULL, FILE_ATTRIBUTE_SYSTEM, FILE_SHARE_READ |
            FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_CREATE,
            FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
            &ea_info, 1024 * sizeof (WCHAR));
        if (NT_SUCCESS(status)) {
          fNtClose(f);
#else
		status = ntCreate(
			&f, FILE_WRITE_DATA | FILE_WRITE_EA | SYNCHRONIZE, &obj_attr,
			&io_block, NULL, FILE_ATTRIBUTE_SYSTEM, FILE_SHARE_READ |
			FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_CREATE,
			FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
			&ea_info, 1024 * sizeof (WCHAR));
		if (NT_SUCCESS(status)) {
			ntClose(f);
#endif //_MSC_VER >1500
          HeapFree(GetProcessHeap(), 0, fn);
          HeapFree(GetProcessHeap(), 0, ea_info);
          HeapFree(GetProcessHeap(), 0, f2);
          HeapFree(GetProcessHeap(), 0, f1);
          return TRUE;
        }
        HeapFree(GetProcessHeap(), 0, fn);
        HeapFree(GetProcessHeap(), 0, ea_info);
      }
    }
  }
  lstrcpy(f2, existingFileName);
  for (p = f2; p[0]; p++)
    if (p[0] == _T('/'))
      p[0] = _T('\\');
  if (!hKernel32)
    hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));
  if (hKernel32) {
#if _MSC_VER >1500
    if (!fCreateSymbolicLink)
      fCreateSymbolicLink = GetProcAddress(hKernel32, CreateSymbolicLink);
    if (fCreateSymbolicLink) {
      if (fCreateSymbolicLink(f1, f2,
                              dirLink ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0)) {
#else
	fCreateSymbolicLink createSynLink;
	createSynLink = (fCreateSymbolicLink)GetProcAddress(hKernel32, CreateSymbolicLink);
	  if (createSynLink) {
		  if (createSynLink(f1, f2,
			  dirLink ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0)) {
#endif // _MSC_VER >1500
        HeapFree(GetProcessHeap(), 0, f2);
        HeapFree(GetProcessHeap(), 0, f1);
        return TRUE;
      }
    }
  }
  if (dirLink) {
    /* Ignore errors - file may already exist */
    CreateDirectory(f1, NULL);
    f = CreateFile(f1, GENERIC_READ | GENERIC_WRITE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                   FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
                   NULL );
    if (f != INVALID_HANDLE_VALUE) {
#if _MSC_VER > 1500
		struct rp_info {
			DWORD tag;
			DWORD dataLength;
			WORD reserved1;
			WORD targetLength;
			WORD targetMaxLength;
			WORD reserved2;
			WCHAR target[kLargeBuf+4];
		}_rp_info, *rp_info = HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			sizeof(*rp_info));
		DWORD len;
		WCHAR *startlink, *endlink;
		rp_info->tag = IO_REPARSE_TAG_MOUNT_POINT;
		rp_info->target[0] = L'\\';
		rp_info->target[1] = L'?';
		rp_info->target[2] = L'?';
		rp_info->target[3] = L'\\';
#else
		struct _rp_info {
			DWORD tag;
			DWORD dataLength;
			WORD reserved1;
			WORD targetLength;
			WORD targetMaxLength;
			WORD reserved2;
			WCHAR target[kLargeBuf+4];
		};
		_rp_info *rp_info = (_rp_info*)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			sizeof(_rp_info));
		DWORD len;
		WCHAR *startlink, *endlink;
		rp_info->tag = IO_REPARSE_TAG_MOUNT_POINT;
		rp_info->target[0] = L'\\';
		rp_info->target[1] = L'?';
		rp_info->target[2] = L'?';
		rp_info->target[3] = L'\\';
#endif  //_MSC_VER > 1500

     
      if (((f2[0] == _T('\\')) && (f2[1] == _T('\\'))) ||
          ((f2[1] == _T(':')) && (f2[2] == _T('\\')))) {
#ifdef UNICODE
        lstrcpy(rp_info->target+4, f2);
#else
        MultiByteToWideChar(CP_ACP, 0, f2, -1,
                            rp_info->target+4, kLargeBuf);
#endif
      } else {
#ifdef UNICODE
        GetFullPathNameW(f1, 1024, rp_info->target+4, &startlink);
        lstrcpy(startlink, f2);
#else
        MultiByteToWideChar(CP_ACP, 0, f1, -1,
                            (LPWSTR)f1, kLargeBuf/sizeof(WCHAR));
        GetFullPathNameW((LPWSTR)f1, 1024, rp_info->target+4, &startlink);
        MultiByteToWideChar(CP_ACP, 0, f2, -1,
                            startlink, kLargeBuf+4-(startlink-rp_info->target));
#endif
      }
      /* Remove "XXX/../" and replace "/" with "\" */
      for (startlink = endlink = rp_info->target+4;
                                           endlink[0]; startlink++, endlink++) {
        startlink[0] = endlink[0];
        if ((startlink[0] == L'\\') &&
            (startlink[-1] == L'.') &&
            (startlink[-2] == L'.')) {
          for (startlink--; startlink > rp_info->target+4 &&
                                             startlink[0] != L'\\'; startlink--)
            { }
          for (startlink--; startlink > rp_info->target+4 &&
                                             startlink[0] != L'\\'; startlink--)
            { }
          if (startlink < rp_info->target+4)
            startlink = rp_info->target+4;
        }
      }
      startlink[0] = endlink[0];
      rp_info->targetLength = lstrlenW(rp_info->target)*sizeof(WCHAR);
      rp_info->targetMaxLength = rp_info->targetLength+sizeof(WCHAR);
      rp_info->dataLength = rp_info->targetMaxLength
                            +FIELD_OFFSET(struct _rp_info, target)
                            -FIELD_OFFSET(struct _rp_info, reserved1)
                            +sizeof(WCHAR);
      if (DeviceIoControl(f, 0x900A4, rp_info,
                          rp_info->dataLength
                                       +FIELD_OFFSET(struct _rp_info, reserved1),
                          NULL, 0, &len, NULL)) {
        CloseHandle(f);
        HeapFree(GetProcessHeap(), 0, rp_info);
        HeapFree(GetProcessHeap(), 0, f2);
        HeapFree(GetProcessHeap(), 0, f1);
        return TRUE;
      }
      CloseHandle(f);
      RemoveDirectory(f1);
      HeapFree(GetProcessHeap(), 0, rp_info);
    }
  }
  for (p = f2; p[0]; p++)
    if (p[0] == _T('\\'))
      p[0] = _T('/');
  f = CreateFile(f1, GENERIC_READ | GENERIC_WRITE,
                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                 CREATE_ALWAYS, FILE_ATTRIBUTE_SYSTEM, NULL);
  if (f != INVALID_HANDLE_VALUE) {
#if _MSC_VER > 1500
	  struct {
		  WCHAR sig[4];
		  WCHAR value[kLargeBuf];
	  } *link_info = HeapAlloc(GetProcessHeap(),
		  HEAP_ZERO_MEMORY,
		  sizeof(*link_info));
	  DWORD towrite, written;
	  link_info->sig[0] = 0x6e49;
	  link_info->sig[1] = 0x7874;
	  link_info->sig[2] = 0x4e4c;
	  link_info->sig[3] = 0x014b;
#else
	  struct _link_info{
		  WCHAR sig[4];
		  WCHAR value[kLargeBuf];
	  } ;
	  _link_info *link_info = (_link_info*)HeapAlloc(GetProcessHeap(),
		  HEAP_ZERO_MEMORY,
		  sizeof(_link_info));
	  DWORD towrite, written;
	  link_info->sig[0] = 0x6e49;
	  link_info->sig[1] = 0x7874;
	  link_info->sig[2] = 0x4e4c;
	  link_info->sig[3] = 0x014b;
#endif //_MSC_VER > 1500
   
#ifdef UNICODE
    lstrcpy(link_info->value, f2);
#else
    MultiByteToWideChar(CP_ACP, 0, f2, -1, link_info->value, kLargeBuf);
#endif
    towrite = lstrlenW(link_info->value)*sizeof(WCHAR)+sizeof(link_info->sig);
    WriteFile(f, link_info, towrite, &written, NULL);
    CloseHandle(f);
    if (written == towrite) {
      HeapFree(GetProcessHeap(), 0, link_info);
      HeapFree(GetProcessHeap(), 0, f2);
      HeapFree(GetProcessHeap(), 0, f1);
      return TRUE;
    }
    HeapFree(GetProcessHeap(), 0, link_info);
  }
  HeapFree(GetProcessHeap(), 0, f2);
  HeapFree(GetProcessHeap(), 0, f1);
  return FALSE;
}

bool CMLink::MakeLink(TCHAR* pTo, TCHAR* pForm,enum LINKTYPE type)
{
	BOOL res = false;
	switch (type) {
    case HARDLINK:
		res = MakeHardLink(pTo, pForm);
		break;
    case SOFTLINKD:
		res = MakeSymLink(pTo, pForm, TRUE);
		break;
    case SOFTLINKF:
		res = MakeSymLink(pTo, pForm, FALSE);
		break;
	}
	return res;
}

void CMLink::makelink(int string_size, TCHAR *variables,enum LINKTYPE type)
{
	TCHAR *msg = (TCHAR *)HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR)*kLargeBuf);
	TCHAR *from = (TCHAR *)HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR)*kSmallBuf);
	TCHAR *to = (TCHAR *)HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR)*kSmallBuf);
	TCHAR *msgFormat =
		type == HARDLINK ? _T("Link: \"%s\" to \"%s\"%s") :
	type == SOFTLINKD ? _T("Symbolic Directory Link: \"%s\" to \"%s\"%s") :
      _T("Symbolic Link: \"%s\" to \"%s\"%s");
	BOOL res;
	switch (type) {
    case HARDLINK:
		res = MakeHardLink(from, to);
		break;
    case SOFTLINKD:
		res = MakeSymLink(from, to, TRUE);
		break;
    case SOFTLINKF:
		res = MakeSymLink(from, to, FALSE);
		break;
	}
	wsprintf(msg, msgFormat, to, from, res ? _T("") : _T(" - fail..."));
	//NSISprint(msg);
	
	HeapFree(GetProcessHeap(), 0, to);
	HeapFree(GetProcessHeap(), 0, from);
	HeapFree(GetProcessHeap(), 0, msg);
}

