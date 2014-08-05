// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#include "CFileSystem.h"
#include "IReadFile.h"
#include "IWriteFile.h"
#include "CZipReader.h"
#include "CFileList.h"
#include "stdio.h"
#include "os.h"
#include "CMemoryFile.h"
#include "CLimitReadFile.h"

#if defined (_DREAM_WINDOWS_API)
		#include <direct.h> // for _chdir
		#include <io.h> // for _access
		#include <tchar.h>
#else
	#if (defined(_DREAM_POSIX_API_)
		#include <stdio.h>
		#include <stdlib.h>
		#include <string.h>
		#include <limits.h>
		#include <sys/types.h>
		#include <dirent.h>
		#include <sys/stat.h>
		#include <unistd.h>
	#endif
#endif

//! constructor
CFileSystem::CFileSystem()
{
	setFileListSystem(FILESYSTEM_NATIVE);
	//! reset current working directory
	getWorkingDirectory();

#ifdef __DREAM_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
	ArchiveLoader.push_back(new CArchiveLoaderZIP(this));
#endif

}


//! destructor
CFileSystem::~CFileSystem()
{
	u32 i;

	for ( i=0; i < FileArchives.size(); ++i)
	{
		FileArchives[i]->releaseRef();
	}

	for ( i=0; i < ArchiveLoader.size(); ++i)
	{
		ArchiveLoader[i]->releaseRef();
	}
}


//! opens a file for read access
IReadFile* CFileSystem::createAndOpenFile(const path& filename)
{
	IReadFile* file = 0;
	u32 i;

	for (i=0; i< FileArchives.size(); ++i)
	{
		file = FileArchives[i]->createAndOpenFile(filename);
		if (file)
			return file;
	}

	// Create the file using an absolute path so that it matches
	// the scheme used by CNullDriver::getTexture().
	return createReadFile(getAbsolutePath(filename));
}


//! Creates an IReadFile interface for treating memory like a file.
IReadFile* CFileSystem::createMemoryReadFile(void* memory, s32 len,
		const path& fileName, bool deleteMemoryWhenDropped)
{
	if (!memory)
		return 0;
	else
		return new CMemoryFile(memory, len, fileName, deleteMemoryWhenDropped);
			}


//! Creates an IReadFile interface for reading files inside files
IReadFile* CFileSystem::createLimitReadFile(const path& fileName,
		IReadFile* alreadyOpenedFile, long pos, long areaSize)
{
	if (!alreadyOpenedFile)
		return 0;
	else
		return new CLimitReadFile(alreadyOpenedFile, pos, areaSize, fileName);
}


//! Creates an IReadFile interface for treating memory like a file.
IWriteFile* CFileSystem::createMemoryWriteFile(void* memory, s32 len,
		const path& fileName, bool deleteMemoryWhenDropped)
{
	if (!memory)
		return 0;
	else
		return new CMemoryFile(memory, len, fileName, deleteMemoryWhenDropped);
}


//! Opens a file for write access.
IWriteFile* CFileSystem::createAndWriteFile(const path& filename, bool append)
{
	return createWriteFile(filename, append);
}


//! Adds an external archive loader to the engine.
void CFileSystem::addArchiveLoader(IArchiveLoader* loader)
{
	if (!loader)
		return;

	loader->addRef();
	ArchiveLoader.push_back(loader);
}

//! Returns the total number of archive loaders added.
u32 CFileSystem::getArchiveLoaderCount() const
{
	return ArchiveLoader.size();
}

//! Gets the archive loader by index.
IArchiveLoader* CFileSystem::getArchiveLoader(u32 index) const
{
	if (index < ArchiveLoader.size())
		return ArchiveLoader[index];
	else
		return 0;
}

//! move the hirarchy of the filesystem. moves sourceIndex relative up or down
bool CFileSystem::moveFileArchive(u32 sourceIndex, s32 relative)
{
	bool r = false;
	const s32 dest = (s32) sourceIndex + relative;
	const s32 dir = relative < 0 ? -1 : 1;
	const s32 sourceEnd = ((s32) FileArchives.size() ) - 1;
	IFileArchive *t;

	for (s32 s = (s32) sourceIndex;s != dest; s += dir)
	{
		if (s < 0 || s > sourceEnd || s + dir < 0 || s + dir > sourceEnd)
			continue;

		t = FileArchives[s + dir];
		FileArchives[s + dir] = FileArchives[s];
		FileArchives[s] = t;
		r = true;
	}
	return r;
}


//! Adds an archive to the file system.
bool CFileSystem::addFileArchive(const path& filename, bool ignoreCase,
			  bool ignorePaths, E_FILE_ARCHIVE_TYPE archiveType,
			  const stringc& password,
			  IFileArchive** retArchive)
{
	IFileArchive* archive = 0;
	bool ret = false;

	// see if archive is already added
	if (changeArchivePassword(filename, password, retArchive))
		return true;

	s32 i;

	// do we know what type it should be?
	if (archiveType == EFAT_UNKNOWN || archiveType == EFAT_FOLDER)
	{
		// try to load archive based on file name
		for (i = ArchiveLoader.size()-1; i >=0 ; --i)
		{
			if (ArchiveLoader[i]->isALoadableFileFormat(filename))
			{
				archive = ArchiveLoader[i]->createArchive(filename, ignoreCase, ignorePaths);
				if (archive)
					break;
			}
		}

		// try to load archive based on content
		if (!archive)
		{
			IReadFile* file = createAndOpenFile(filename);
			if (file)
			{
				for (i = ArchiveLoader.size()-1; i >= 0; --i)
				{
					file->seek(0);
					if (ArchiveLoader[i]->isALoadableFileFormat(file))
					{
						file->seek(0);
						archive = ArchiveLoader[i]->createArchive(file, ignoreCase, ignorePaths);
						if (archive)
							break;
					}
				}
				file->releaseRef();
			}
		}
	}
	else
	{
		// try to open archive based on archive loader type

		IReadFile* file = 0;

		for (i = ArchiveLoader.size()-1; i >= 0; --i)
		{
			if (ArchiveLoader[i]->isALoadableFileFormat(archiveType))
			{
				// attempt to open file
				if (!file)
					file = createAndOpenFile(filename);

				// is the file open?
				if (file)
				{
					// attempt to open archive
					file->seek(0);
					if (ArchiveLoader[i]->isALoadableFileFormat(file))
					{
						file->seek(0);
						archive = ArchiveLoader[i]->createArchive(file, ignoreCase, ignorePaths);
						if (archive)
							break;
					}
				}
				else
				{
					// couldn't open file
					break;
				}
			}
		}

		// if open, close the file
		if (file)
			file->releaseRef();
	}

	if (archive)
	{
		FileArchives.push_back(archive);
		if (password.size())
			archive->Password=password;
		if (retArchive)
			*retArchive = archive;
		ret = true;
	}
	else
	{
		Printer::log("Could not create archive for", filename, ELL_ERROR);
	}
	return ret;
}

// don't expose!
bool CFileSystem::changeArchivePassword(const path& filename,
		const stringc& password,
		IFileArchive** archive)
{
	for (s32 idx = 0; idx < (s32)FileArchives.size(); ++idx)
	{
		// TODO: This should go into a path normalization method
		// We need to check for directory names with trailing slash and without
		const path absPath = getAbsolutePath(filename);
		const path arcPath = FileArchives[idx]->getFileList()->getPath();
		if ((absPath == arcPath) || ((absPath+_DREAM_TEXT("/")) == arcPath))
		{
			if (password.size())
				FileArchives[idx]->Password=password;
			if (archive)
				*archive = FileArchives[idx];
			return true;
		}
	}

	return false;
}

bool CFileSystem::addFileArchive(IReadFile* file, bool ignoreCase,
		bool ignorePaths, E_FILE_ARCHIVE_TYPE archiveType,
		const stringc& password, IFileArchive** retArchive)
{
	if (!file || archiveType == EFAT_FOLDER)
		return false;

	if (file)
	{
		if (changeArchivePassword(file->getFileName(), password, retArchive))
			return true;

		IFileArchive* archive = 0;
		s32 i;

		if (archiveType == EFAT_UNKNOWN)
		{
			// try to load archive based on file name
			for (i = ArchiveLoader.size()-1; i >=0 ; --i)
			{
				if (ArchiveLoader[i]->isALoadableFileFormat(file->getFileName()))
				{
					archive = ArchiveLoader[i]->createArchive(file, ignoreCase, ignorePaths);
					if (archive)
						break;
				}
			}

			// try to load archive based on content
			if (!archive)
			{
				for (i = ArchiveLoader.size()-1; i >= 0; --i)
				{
					file->seek(0);
					if (ArchiveLoader[i]->isALoadableFileFormat(file))
					{
						file->seek(0);
						archive = ArchiveLoader[i]->createArchive(file, ignoreCase, ignorePaths);
						if (archive)
							break;
					}
				}
			}
		}
		else
		{
			// try to open archive based on archive loader type
			for (i = ArchiveLoader.size()-1; i >= 0; --i)
			{
				if (ArchiveLoader[i]->isALoadableFileFormat(archiveType))
				{
					// attempt to open archive
					file->seek(0);
					if (ArchiveLoader[i]->isALoadableFileFormat(file))
					{
						file->seek(0);
						archive = ArchiveLoader[i]->createArchive(file, ignoreCase, ignorePaths);
						if (archive)
							break;
					}
				}
			}
		}

		if (archive)
		{
			FileArchives.push_back(archive);
			if (password.size())
				archive->Password=password;
			if (retArchive)
				*retArchive = archive;
			return true;
		}
		else
		{
			Printer::log("Could not create archive for", file->getFileName(), ELL_ERROR);
		}
	}

	return false;
}


//! Adds an archive to the file system.
bool CFileSystem::addFileArchive(IFileArchive* archive)
{
	for (u32 i=0; i < FileArchives.size(); ++i)
	{
		if (archive == FileArchives[i])
		{
			
			return false;
		}
	}
	FileArchives.push_back(archive);
	return true;
}


//! removes an archive from the file system.
bool CFileSystem::removeFileArchive(u32 index)
{
	bool ret = false;
	if (index < FileArchives.size())
	{
		FileArchives[index]->releaseRef();
		FileArchives.erase(index);
		ret = true;
	}
	
	return ret;
}


//! removes an archive from the file system.
bool CFileSystem::removeFileArchive(const path& filename)
{
	const path absPath = getAbsolutePath(filename);
	for (u32 i=0; i < FileArchives.size(); ++i)
	{
		if (absPath == FileArchives[i]->getFileList()->getPath())
			return removeFileArchive(i);
	}
	
	return false;
}


//! Removes an archive from the file system.
bool CFileSystem::removeFileArchive(const IFileArchive* archive)
{
	for (u32 i=0; i < FileArchives.size(); ++i)
	{
		if (archive == FileArchives[i])
		{
			
			return removeFileArchive(i);
		}
	}
	
	return false;
}


//! gets an archive
u32 CFileSystem::getFileArchiveCount() const
{
	return FileArchives.size();
}


IFileArchive* CFileSystem::getFileArchive(u32 index)
{
	return index < getFileArchiveCount() ? FileArchives[index] : 0;
}


//! Returns the string of the current working directory
const path& CFileSystem::getWorkingDirectory()
{
	EFileSystemType type = FileSystemType;

	if (type != FILESYSTEM_NATIVE)
	{
		type = FILESYSTEM_VIRTUAL;
	}
	else
	{
		#if defined(_DREAM_WINDOWS_API)
			fschar_t tmp[_MAX_PATH];
			#if defined(_DREAM_WCHAR_FILESYSTEM )
				_wgetcwd(tmp, _MAX_PATH);
				WorkingDirectory[FILESYSTEM_NATIVE] = tmp;
				WorkingDirectory[FILESYSTEM_NATIVE].replace(L'\\', L'/');
			#else
				_getcwd(tmp, _MAX_PATH);
				WorkingDirectory[FILESYSTEM_NATIVE] = tmp;
				WorkingDirectory[FILESYSTEM_NATIVE].replace('\\', '/');
			#endif
		#endif

		#if (defined(_DREAM_POSIX_API_)
			// getting the CWD is rather complex as we do not know the size
			// so try it until the call was successful
			// Note that neither the first nor the second parameter may be 0 according to POSIX

			#if defined(_DREAM_WCHAR_FILESYSTEM )
				u32 pathSize=256;
				wchar_t *tmpPath = new wchar_t[pathSize];
				while ((pathSize < (1<<16)) && !(wgetcwd(tmpPath,pathSize)))
				{
					delete [] tmpPath;
					pathSize *= 2;
					tmpPath = new char[pathSize];
				}
				if (tmpPath)
				{
					WorkingDirectory[FILESYSTEM_NATIVE] = tmpPath;
					delete [] tmpPath;
				}
			#else
				u32 pathSize=256;
				char *tmpPath = new char[pathSize];
				while ((pathSize < (1<<16)) && !(getcwd(tmpPath,pathSize)))
				{
					delete [] tmpPath;
					pathSize *= 2;
					tmpPath = new char[pathSize];
				}
				if (tmpPath)
				{
					WorkingDirectory[FILESYSTEM_NATIVE] = tmpPath;
					delete [] tmpPath;
				}
			#endif
		#endif

		WorkingDirectory[type].validate();
	}

	return WorkingDirectory[type];
}


//! Changes the current Working Directory to the given string.
bool CFileSystem::changeWorkingDirectoryTo(const path& newDirectory)
{
	bool success=false;

	if (FileSystemType != FILESYSTEM_NATIVE)
	{
		WorkingDirectory[FILESYSTEM_VIRTUAL] = newDirectory;
		// is this empty string constant really intended?
		flattenFilename(WorkingDirectory[FILESYSTEM_VIRTUAL], _DREAM_TEXT(""));
		success = true;
	}
	else
	{
		WorkingDirectory[FILESYSTEM_NATIVE] = newDirectory;
#if defined(_MSC_VER)
	#if defined(_DREAM_WCHAR_FILESYSTEM)
		success = (_wchdir(newDirectory.c_str()) == 0);
	#else
		success = (_chdir(newDirectory.c_str()) == 0);
	#endif
#else
    #if defined(_DREAM_WCHAR_FILESYSTEM)
		success = (_wchdir(newDirectory.c_str()) == 0);
    #else
        success = (chdir(newDirectory.c_str()) == 0);
    #endif
#endif
	}

	return success;
}


path CFileSystem::getAbsolutePath(const path& filename) const
{
#if defined(_DREAM_WINDOWS_API)
	fschar_t *p=0;
	fschar_t fpath[_MAX_PATH];
	#if defined(_DREAM_WCHAR_FILESYSTEM )
		p = _wfullpath(fpath, filename.c_str(), _MAX_PATH);
		stringw tmp(p);
		tmp.replace(L'\\', L'/');
	#else
		p = _fullpath(fpath, filename.c_str(), _MAX_PATH);
		stringc tmp(p);
		tmp.replace('\\', '/');
	#endif
	return tmp;
#elif (defined(_DREAM_POSIX_API_)
	c8* p=0;
	c8 fpath[4096];
	fpath[0]=0;
	p = realpath(filename.c_str(), fpath);
	if (!p)
	{
		// content in fpath is unclear at this point
		if (!fpath[0]) // seems like fpath wasn't altered, use our best guess
		{
			path tmp(filename);
			return flattenFilename(tmp);
		}
		else
			return path(fpath);
	}
	if (filename[filename.size()-1]=='/')
		return path(p)+_DREAM_TEXT("/");
	else
		return path(p);
#else
	return path(filename);
#endif
}


//! returns the directory part of a filename, i.e. all until the first
//! slash or backslash, excluding it. If no directory path is prefixed, a '.'
//! is returned.
path CFileSystem::getFileDir(const path& filename) const
{
	// find last forward or backslash
	s32 lastSlash = filename.findLast('/');
	const s32 lastBackSlash = filename.findLast('\\');
	lastSlash = lastSlash > lastBackSlash ? lastSlash : lastBackSlash;

	if ((u32)lastSlash < filename.size())
		return filename.subString(0, lastSlash);
	else
		return _DREAM_TEXT(".");
}


//! returns the base part of a filename, i.e. all except for the directory
//! part. If no directory path is prefixed, the full name is returned.
path CFileSystem::getFileBasename(const path& filename, bool keepExtension) const
{
	// find last forward or backslash
	s32 lastSlash = filename.findLast('/');
	const s32 lastBackSlash = filename.findLast('\\');
	lastSlash = max_(lastSlash, lastBackSlash);

	// get number of chars after last dot
	s32 end = 0;
	if (!keepExtension)
	{
		// take care to search only after last slash to check only for
		// dots in the filename
		end = filename.findLast('.');
		if (end == -1 || end < lastSlash)
			end=0;
		else
			end = filename.size()-end;
	}

	if ((u32)lastSlash < filename.size())
		return filename.subString(lastSlash+1, filename.size()-lastSlash-1-end);
	else if (end != 0)
		return filename.subString(0, filename.size()-end);
	else
		return filename;
}


//! flatten a path and file name for example: "/you/me/../." becomes "/you"
path& CFileSystem::flattenFilename(path& directory, const path& root) const
{
	directory.replace('\\', '/');
	if (directory.lastChar() != '/')
		directory.append('/');

	path dir;
	path subdir;

	s32 lastpos = 0;
	s32 pos = 0;
	bool lastWasRealDir=false;

	while ((pos = directory.findNext('/', lastpos)) >= 0)
	{
		subdir = directory.subString(lastpos, pos - lastpos + 1);

		if (subdir == _DREAM_TEXT("../"))
		{
			if (lastWasRealDir)
			{
				deletePathFromPath(dir, 2);
				lastWasRealDir=(dir.size()!=0);
			}
			else
			{
				dir.append(subdir);
				lastWasRealDir=false;
			}
		}
		else if (subdir == _DREAM_TEXT("/"))
		{
			dir = root;
		}
		else if (subdir != _DREAM_TEXT("./"))
		{
			dir.append(subdir);
			lastWasRealDir=true;
		}

		lastpos = pos + 1;
	}
	directory = dir;
	return directory;
}



//! Sets the current file systen type
EFileSystemType CFileSystem::setFileListSystem(EFileSystemType listType)
{
	EFileSystemType current = FileSystemType;
	FileSystemType = listType;
	return current;
}


//! Creates a list of files and directories in the current working directory
IFileList* CFileSystem::createFileList()
{
	CFileList* r = 0;
	path Path = getWorkingDirectory();
	Path.replace('\\', '/');
	if (Path.lastChar() != '/')
		Path.append('/');

	//! Construct from native filesystem
	if (FileSystemType == FILESYSTEM_NATIVE)
	{
		// --------------------------------------------
		//! Windows version
		#ifdef _DREAM_WINDOWS_API
		r = new CFileList(Path, true, false);

		// TODO: Should be unified once mingw adapts the proper types
#if defined(__GNUC__)
		long hFile; //mingw return type declaration
#else
		intptr_t hFile;
#endif

		struct _tfinddata_t c_file;
		if( (hFile = _tfindfirst( _T("*"), &c_file )) != -1L )
		{
			do
			{
				r->addItem(Path + c_file.name, 0, c_file.size, (_A_SUBDIR & c_file.attrib) != 0, 0);
			}
			while( _tfindnext( hFile, &c_file ) == 0 );

			_findclose( hFile );
		}
		#endif

		//TODO add drives
		//entry.Name = "E:\\";
		//entry.isDirectory = true;
		//Files.push_back(entry);
		#endif

		// --------------------------------------------
		//! Linux version
		#if (defined(_DREAM_POSIX_API_)


		r = new CFileList(Path, false, false);

		r->addItem(Path + _DREAM_TEXT(".."), 0, 0, true, 0);

		//! We use the POSIX compliant methods instead of scandir
		DIR* dirHandle=opendir(Path.c_str());
		if (dirHandle)
		{
			struct dirent *dirEntry;
			while ((dirEntry=readdir(dirHandle)))
			{
				u32 size = 0;
				bool isDirectory = false;

				if((strcmp(dirEntry->d_name, ".")==0) ||
				   (strcmp(dirEntry->d_name, "..")==0))
				{
					continue;
				}
				struct stat buf;
				if (stat(dirEntry->d_name, &buf)==0)
				{
					size = buf.st_size;
					isDirectory = S_ISDIR(buf.st_mode);
				}
				// only available on some systems
				else
				{
					isDirectory = dirEntry->d_type == DT_DIR;
				}
				#endif

				r->addItem(Path + dirEntry->d_name, 0, size, isDirectory, 0);
			}
			closedir(dirHandle);
		}
		#endif
	}
	else
	{
		//! create file list for the virtual filesystem
		r = new CFileList(Path, false, false);

		//! add relative navigation
		SFileListEntry e2;
		SFileListEntry e3;

		//! PWD
		r->addItem(Path + _DREAM_TEXT("."), 0, 0, true, 0);

		//! parent
		r->addItem(Path + _DREAM_TEXT(".."), 0, 0, true, 0);

		//! merge archives
		for (u32 i=0; i < FileArchives.size(); ++i)
		{
			const IFileList *merge = FileArchives[i]->getFileList();

			for (u32 j=0; j < merge->getFileCount(); ++j)
			{
				if (isInSameDirectory(Path, merge->getFullFileName(j)) == 0)
				{
					r->addItem(merge->getFullFileName(j), merge->getFileOffset(j), merge->getFileSize(j), merge->isDirectory(j), 0);
				}
			}
		}
	}

	if (r)
		r->sort();
	return r;
}

//! Creates an empty filelist
IFileList* CFileSystem::createEmptyFileList(const path& path, bool ignoreCase, bool ignorePaths)
{
	return new CFileList(path, ignoreCase, ignorePaths);
}


//! determines if a file exists and would be able to be opened.
bool CFileSystem::existFile(const path& filename) const
{
	for (u32 i=0; i < FileArchives.size(); ++i)
		if (FileArchives[i]->getFileList()->findFile(filename)!=-1)
			return true;
#if defined(_MSC_VER)
    #if defined(_DREAM_WCHAR_FILESYSTEM)
        return (_waccess(filename.c_str(), 0) != -1);
    #else
        return (_access(filename.c_str(), 0) != -1);
    #endif
#elif defined(F_OK)
    #if defined(_DREAM_WCHAR_FILESYSTEM)
        return (_waccess(filename.c_str(), F_OK) != -1);
    #else
        return (access(filename.c_str(), F_OK) != -1);
	#endif
#else
    return (access(filename.c_str(), 0) != -1);
#endif

}

//! creates a filesystem which is able to open files from the ordinary file system,
//! and out of zipfiles, which are able to be added to the filesystem.
IFileSystem* createFileSystem()
{
	return new CFileSystem();
}

