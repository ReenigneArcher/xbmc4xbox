#pragma once

/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "coffldr.h"
#include "LibraryLoader.h"

class DllLoader;


typedef struct Export
{
  const char*   name;
  unsigned long ordinal;
  void*         function;
  void*         track_function;
} Export;

typedef struct ExportEntry
{
  Export exp;
  ExportEntry* next;
} ExportEntry;

typedef struct _LoadedList
{
  DllLoader* pDll;
  _LoadedList* pNext;
} LoadedList;
  
class DllLoader : public CoffLoader, public LibraryLoader
{
public:
  DllLoader(const char *dll, bool track = false, bool bSystemDll = false, bool bLoadSymbols = false, Export* exports = NULL);
  virtual ~DllLoader();

  virtual bool Load();
  virtual void Unload();

  virtual int ResolveExport(const char*, void** ptr);
  virtual int ResolveExport(unsigned long ordinal, void** ptr);
  virtual bool HasSymbols() { return m_bLoadSymbols && !m_bUnloadSymbols; }
  virtual bool IsSystemDll() { return m_bSystemDll; }
  virtual HMODULE GetHModule() { return (HMODULE)hModule; }  
  
protected:  
  int Parse();
  int ResolveImports();

  Export* GetExportByOrdinal(unsigned long ordinal);
  Export* GetExportByFunctionName(const char* sFunctionName);
  
  void AddExport(unsigned long ordinal, void* function, void* track_function = NULL);
  void AddExport(char* sFunctionName, unsigned long ordinal, void* function, void* track_function = NULL);
  void AddExport(char* sFunctionName, void* function, void* track_function = NULL);
  void SetExports(Export* exports) { m_pStaticExports = exports; }

protected:
  // Just pointers; dont' delete...
  ImportDirTable_t *ImportDirTable;
  ExportDirTable_t *ExportDirTable;
  bool m_bTrack;
  bool m_bSystemDll; // true if this dll should not be removed
  bool m_bLoadSymbols; // when true this dll should not be removed
  bool m_bUnloadSymbols;
  ExportEntry* m_pExportHead;
  Export* m_pStaticExports;
  LoadedList* m_pDlls;

  void PrintImportLookupTable(unsigned long ImportLookupTable_RVA);
  void PrintImportTable(ImportDirTable_t *ImportDirTable);
  void PrintExportTable(ExportDirTable_t *ExportDirTable);
  
  int ResolveOrdinal(char*, unsigned long, void**);
  int ResolveName(char*, char*, void **);
  char* ResolveReferencedDll(char* dll);
  int LoadExports();
  void LoadSymbols();
  void UnloadSymbols();
};
