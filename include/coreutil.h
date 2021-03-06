#ifndef __DREAMCORE_UTIL
#define __DREAMCORE_UTIL
#include "path.h"
/*! \file coreutil.h
	\brief File containing useful basic utility functions
*/

// ----------- some basic quite often used string functions -----------------

//! search if a filename has a proper extension
inline s32 isFileExtension (	const path& filename,
								const path& ext0,
								const path& ext1,
								const path& ext2)
{
	s32 extPos = filename.findLast ( '.' );
	if ( extPos < 0 )
		return 0;

	extPos += 1;
	if ( filename.equals_substring_ignore_case ( ext0, extPos ) ) return 1;
	if ( filename.equals_substring_ignore_case ( ext1, extPos ) ) return 2;
	if ( filename.equals_substring_ignore_case ( ext2, extPos ) ) return 3;
	return 0;
}

//! search if a filename has a proper extension
inline bool hasFileExtension (	const path& filename,
								const path& ext0,
								const path& ext1 = "",
								const path& ext2 = "")
{
	return isFileExtension ( filename, ext0, ext1, ext2 ) > 0;
}

//! cut the filename extension from a source file path and store it in a dest file path
inline path& cutFilenameExtension ( path &dest, const path &source )
{
	s32 endPos = source.findLast ( '.' );
	dest = source.subString ( 0, endPos < 0 ? source.size () : endPos );
	return dest;
}

//! get the filename extension from a file path
inline path& getFileNameExtension ( path &dest, const path &source )
{
	s32 endPos = source.findLast ( '.' );
	if ( endPos < 0 )
		dest = "";
	else
		dest = source.subString ( endPos, source.size () );
	return dest;
}

//! delete path from filename
inline path& deletePathFromFilename(path& filename)
{
	// delete path from filename
	const fschar_t* s = filename.c_str();
	const fschar_t* p = s + filename.size();

	// search for path separator or beginning
	while ( *p != '/' && *p != '\\' && p != s )
		p--;

	if ( p != s )
	{
		++p;
		filename = p;
	}
	return filename;
}

//! trim paths
inline path& deletePathFromPath(path& filename, s32 pathCount)
{
	// delete path from filename
	s32 i = filename.size();

	// search for path separator or beginning
	while ( i>=0 )
	{
		if ( filename[i] == '/' || filename[i] == '\\' )
		{
			if ( --pathCount <= 0 )
				break;
		}
		--i;
	}

	if ( i>0 )
	{
		filename [ i + 1 ] = 0;
		filename.validate();
	}
	else
		filename="";
	return filename;
}

//! looks if file is in the same directory of path. returns offset of directory.
//! 0 means in same directory. 1 means file is direct child of path
inline s32 isInSameDirectory ( const path& _path, const path& file )
{
	s32 subA = 0;
	s32 subB = 0;
	s32 pos;

	if ( _path.size() && !_path.equalsn ( file, _path.size() ) )
		return -1;

	pos = 0;
	while ( (pos = _path.findNext ( '/', pos )) >= 0 )
	{
		subA += 1;
		pos += 1;
	}

	pos = 0;
	while ( (pos = file.findNext ( '/', pos )) >= 0 )
	{
		subB += 1;
		pos += 1;
	}

	return subB - subA;
}

// splits a path into components
static inline void splitFilename(const path &name, path* _path=0,
		path* filename=0, path* extension=0, bool make_lower=false)
{
	s32 i = name.size();
	s32 extpos = i;

	// search for path separator or beginning
	while ( i >= 0 )
	{
		if ( name[i] == '.' )
		{
			extpos = i;
			if ( extension )
				*extension = name.subString ( extpos + 1, name.size() - (extpos + 1), make_lower );
		}
		else
		if ( name[i] == '/' || name[i] == '\\' )
		{
			if ( filename )
				*filename = name.subString ( i + 1, extpos - (i + 1), make_lower );
			if ( _path )
			{
				*_path = name.subString ( 0, i + 1, make_lower );
				_path->replace ( '\\', '/' );
			}
			return;
		}
		i -= 1;
	}
	if ( filename )
		*filename = name.subString ( 0, extpos, make_lower );
}

/*
//! some standard function ( to remove dependencies )
#undef isdigit
#undef isspace
#undef isupper
inline s32 isdigit(s32 c) { return c >= '0' && c <= '9'; }
inline s32 isspace(s32 c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
inline s32 isupper(s32 c) { return c >= 'A' && c <= 'Z'; }
*/
#endif