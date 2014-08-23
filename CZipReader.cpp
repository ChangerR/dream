#include "CZipReader.h"
#include "coreutil.h"
#include "os.h"
#include <stdio.h>
#include <wchar.h>
#define _DREAM_COMPILE_WITH_ZLIB_

#ifdef __DREAM_COMPILE_WITH_ZIP_ARCHIVE_LOADER_

#ifdef _DREAM_COMPILE_WITH_ZLIB_
#include "zlib/zlib.h"
#endif

//! Constructor
CArchiveLoaderZIP::CArchiveLoaderZIP(IFileSystem* fs)
	: FileSystem(fs)
{

}

//! returns true if the file maybe is able to be loaded by this class
bool CArchiveLoaderZIP::isALoadableFileFormat(const path& filename) const
{
	return hasFileExtension(filename, "zip","gz");
}


//! Check to see if the loader can create archives of this type.
bool CArchiveLoaderZIP::isALoadableFileFormat(E_FILE_ARCHIVE_TYPE fileType) const
{
	return (fileType == EFAT_ZIP || fileType == EFAT_GZIP);
}

//! Creates an archive from the filename
/** \param file File handle to check.
\return Pointer to newly created archive, or 0 upon error. */
IFileArchive* CArchiveLoaderZIP::createArchive(const path& filename, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{
		archive = createArchive(file, ignoreCase, ignorePaths);
		file->releaseRef();
	}

	return archive;
}

//! creates/loads an archive from the file.
//! \return Pointer to the created archive. Returns 0 if loading failed.
IFileArchive* CArchiveLoaderZIP::createArchive(IReadFile* file, bool ignoreCase, bool ignorePaths) const
{
	IFileArchive *archive = 0;
	if (file)
	{
		file->seek(0);

		u16 sig;
		file->read(&sig, 2);

		file->seek(0);

		bool isGZip = (sig == 0x8b1f);

		archive = new CZipReader(file, ignoreCase, ignorePaths, isGZip);
	}
	return archive;
}

//! Check if the file might be loaded by this class
/** Check might look into the file.
\param file File handle to check.
\return True if file seems to be loadable. */
bool CArchiveLoaderZIP::isALoadableFileFormat(IReadFile* file) const
{
	SZIPFileHeader header;

	file->read( &header.Sig, 4 );

	return header.Sig == 0x04034b50 || // ZIP
		(header.Sig&0xffff) == 0x8b1f; // gzip
}

CZipReader::CZipReader(IReadFile* file, bool ignoreCase, bool ignorePaths, bool isGZip)
	: CFileList((file ? file->getFileName() : path("")), ignoreCase, ignorePaths), File(file), IsGZip(isGZip)
{
	if (File)
	{
		File->addRef();

		// load file entries
		if (IsGZip)
			while (scanGZipHeader()) { }
		else
			while (scanZipHeader()) { }

		sort();
	}
}


CZipReader::~CZipReader()
{
	if (File)
		File->releaseRef();
}

//! get the archive type
E_FILE_ARCHIVE_TYPE CZipReader::getType() const
{
	return IsGZip ? EFAT_GZIP : EFAT_ZIP;
}

const IFileList* CZipReader::getFileList() const
{
	return this;
}

//! scans for a local header, returns false if there is no more local file header.
//! The gzip file format seems to think that there can be multiple files in a gzip file
//! but none
bool CZipReader::scanGZipHeader()
{
	SZipFileEntry entry;
	entry.Offset = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	// read header
	SGZIPMemberHeader header;
	if (File->read(&header, sizeof(SGZIPMemberHeader)) == sizeof(SGZIPMemberHeader))
	{
		// check header value
		if (header.sig != 0x8b1f)
			return false;

		// now get the file info
		if (header.flags & EGZF_EXTRA_FIELDS)
		{
			// read lenth of extra data
			u16 dataLen;

			File->read(&dataLen, 2);

			// skip it
			File->seek(dataLen, true);
		}

		path ZipFileName = "";

		if (header.flags & EGZF_FILE_NAME)
		{
			c8 c;
			File->read(&c, 1);
			while (c)
			{
				ZipFileName.append(c);
				File->read(&c, 1);
			}
		}
		else
		{
			// no file name?
			ZipFileName = Path;
			deletePathFromFilename(ZipFileName);

			// rename tgz to tar or remove gz extension
			if (hasFileExtension(ZipFileName, "tgz"))
			{
				ZipFileName[ ZipFileName.size() - 2] = 'a';
				ZipFileName[ ZipFileName.size() - 1] = 'r';
			}
			else if (hasFileExtension(ZipFileName, "gz"))
			{
				ZipFileName[ ZipFileName.size() - 3] = 0;
				ZipFileName.validate();
			}
		}

		if (header.flags & EGZF_COMMENT)
		{
			c8 c='a';
			while (c)
				File->read(&c, 1);
		}

		if (header.flags & EGZF_CRC16)
			File->seek(2, true);

		// we are now at the start of the data blocks
		entry.Offset = File->getPos();

		entry.header.FilenameLength = ZipFileName.size();

		entry.header.CompressionMethod = header.compressionMethod;
		entry.header.DataDescriptor.CompressedSize = (File->getSize() - 8) - File->getPos();

		// seek to file end
		File->seek(entry.header.DataDescriptor.CompressedSize, true);

		// read CRC
		File->read(&entry.header.DataDescriptor.CRC32, 4);
		// read uncompressed size
		File->read(&entry.header.DataDescriptor.UncompressedSize, 4);

		// now we've filled all the fields, this is just a standard deflate block
		addItem(ZipFileName, entry.Offset, entry.header.DataDescriptor.UncompressedSize, false, 0);
		FileInfo.push_back(entry);
	}

	// there's only one block of data in a gzip file
	return false;
}

//! scans for a local header, returns false if there is no more local file header.
bool CZipReader::scanZipHeader(bool ignoreGPBits)
{
	path ZipFileName = "";
	SZipFileEntry entry;
	entry.Offset = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	File->read(&entry.header, sizeof(SZIPFileHeader));

	if (entry.header.Sig != 0x04034b50)
		return false; // local file headers end here.

	// read filename
	{
		c8 *tmp = new c8 [ entry.header.FilenameLength + 2 ];
		File->read(tmp, entry.header.FilenameLength);
		tmp[entry.header.FilenameLength] = 0;
		ZipFileName = tmp;
		delete [] tmp;
	}

	if ((entry.header.GeneralBitFlag & ZIP_FILE_ENCRYPTED) && (entry.header.CompressionMethod == 99))
	{
		Printer::log("cannot open encrypted zip");
		return false;
	}
	if (entry.header.ExtraFieldLength)
		File->seek(entry.header.ExtraFieldLength, true);

	// if bit 3 was set, use CentralDirectory for setup
	if (!ignoreGPBits && entry.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRIPTOR)
	{
		SZIPFileCentralDirEnd dirEnd;
		FileInfo.clear();
		Files.clear();
		// First place where the end record could be stored
		File->seek(File->getSize()-22);
		const char endID[] = {0x50, 0x4b, 0x05, 0x06, 0x0};
		char tmp[5]={'\0'};
		bool found=false;
		// search for the end record ID
		while (!found && File->getPos()>0)
		{
			int seek=8;
			File->read(tmp, 4);
			switch (tmp[0])
			{
			case 0x50:
				if (!strcmp(endID, tmp))
				{
					seek=4;
					found=true;
				}
				break;
			case 0x4b:
				seek=5;
				break;
			case 0x05:
				seek=6;
				break;
			case 0x06:
				seek=7;
				break;
			}
			File->seek(-seek, true);
		}
		File->read(&dirEnd, sizeof(dirEnd));
		FileInfo.reallocate(dirEnd.TotalEntries);
		File->seek(dirEnd.Offset);
		while (scanCentralDirectoryHeader()) { }
		return false;
	}

	// store position in file
	entry.Offset = File->getPos();
	// move forward length of data
	File->seek(entry.header.DataDescriptor.CompressedSize, true);

	addItem(ZipFileName, entry.Offset, entry.header.DataDescriptor.UncompressedSize, ZipFileName.lastChar()=='/', FileInfo.size());
	FileInfo.push_back(entry);

	return true;
}

//! scans for a local header, returns false if there is no more local file header.
bool CZipReader::scanCentralDirectoryHeader()
{
	path ZipFileName = "";
	SZIPFileCentralDirFileHeader entry;
	File->read(&entry, sizeof(SZIPFileCentralDirFileHeader));

	if (entry.Sig != 0x02014b50)
		return false; // central dir headers end here.

	const long pos = File->getPos();
	File->seek(entry.RelativeOffsetOfLocalHeader);
	scanZipHeader(true);
	File->seek(pos+entry.FilenameLength+entry.ExtraFieldLength+entry.FileCommentLength);
	FileInfo.getLast().header.DataDescriptor.CompressedSize=entry.CompressedSize;
	FileInfo.getLast().header.DataDescriptor.UncompressedSize=entry.UncompressedSize;
	FileInfo.getLast().header.DataDescriptor.CRC32=entry.CRC32;
	Files.getLast().Size=entry.UncompressedSize;
	return true;
}

//! opens a file by file name
IReadFile* CZipReader::createAndOpenFile(const path& filename)
{
	s32 index = findFile(filename, false);

	if (index != -1)
		return createAndOpenFile(index);

	return 0;
}

//! opens a file by index
IReadFile* CZipReader::createAndOpenFile(u32 index)
{
	// Irrlicht supports 0, 8, 12, 14, 99
	//0 - The file is stored (no compression)
	//1 - The file is Shrunk
	//2 - The file is Reduced with compression factor 1
	//3 - The file is Reduced with compression factor 2
	//4 - The file is Reduced with compression factor 3
	//5 - The file is Reduced with compression factor 4
	//6 - The file is Imploded
	//7 - Reserved for Tokenizing compression algorithm
	//8 - The file is Deflated
	//9 - Reserved for enhanced Deflating
	//10 - PKWARE Date Compression Library Imploding
	//12 - bzip2 - Compression Method from libbz2, WinZip 10
	//14 - LZMA - Compression Method, WinZip 12
	//96 - Jpeg compression - Compression Method, WinZip 12
	//97 - WavPack - Compression Method, WinZip 11
	//98 - PPMd - Compression Method, WinZip 10
	//99 - AES encryption, WinZip 9
	const SZipFileEntry &e = FileInfo[Files[index].ID];
	wchar_t buf[64];
	s16 actualCompressionMethod=e.header.CompressionMethod;
	IReadFile* decrypted=0;
	u8* decryptedBuf=0;
	u32 decryptedSize=e.header.DataDescriptor.CompressedSize;
	if ((e.header.GeneralBitFlag & ZIP_FILE_ENCRYPTED) && (e.header.CompressionMethod == 99)) {
		Printer::log("error encryption\n");
		return NULL;
	}
	switch(actualCompressionMethod) {
	
	case 0: // no compression
		{
			if (decrypted)
				return decrypted;
			else
				return createLimitReadFile(Files[index].FullName, File, e.Offset, decryptedSize);
		}
	case 8:
		{
  			#ifdef _DREAM_COMPILE_WITH_ZLIB_

			const u32 uncompressedSize = e.header.DataDescriptor.UncompressedSize;
			c8* pBuf = new c8[ uncompressedSize ];
			if (!pBuf)
			{
				swprintf ( buf, 64, L"Not enough memory for decompressing %s", Files[index].FullName.c_str() );
				Printer::log( buf, ELL_ERROR);
				if (decrypted)
					decrypted->releaseRef();
				return 0;
			}

			u8 *pcData = decryptedBuf;
			if (!pcData)
			{
				pcData = new u8[decryptedSize];
				if (!pcData)
				{
					swprintf ( buf, 64, L"Not enough memory for decompressing %s", Files[index].FullName.c_str() );
					Printer::log( buf, ELL_ERROR);
					delete [] pBuf;
					return 0;
				}

				//memset(pcData, 0, decryptedSize);
				File->seek(e.Offset);
				File->read(pcData, decryptedSize);
			}

			// Setup the inflate stream.
			z_stream stream;
			s32 err;

			stream.next_in = (Bytef*)pcData;
			stream.avail_in = (uInt)decryptedSize;
			stream.next_out = (Bytef*)pBuf;
			stream.avail_out = uncompressedSize;
			stream.zalloc = (alloc_func)0;
			stream.zfree = (free_func)0;

			// Perform inflation. wbits < 0 indicates no zlib header inside the data.
			err = inflateInit2(&stream, -MAX_WBITS);
			if (err == Z_OK)
			{
				err = inflate(&stream, Z_FINISH);
				inflateEnd(&stream);
				if (err == Z_STREAM_END)
					err = Z_OK;
				err = Z_OK;
				inflateEnd(&stream);
			}

			if (decrypted)
				decrypted->releaseRef();
			else
				delete[] pcData;

			if (err != Z_OK)
			{
				swprintf ( buf, 64, L"Error decompressing %s", Files[index].FullName.c_str() );
				Printer::log( buf, ELL_ERROR);
				delete [] pBuf;
				return 0;
			}
			else
				return createMemoryReadFile(pBuf, uncompressedSize, Files[index].FullName, true);

			#else
			return 0; // zlib not compiled, we cannot decompress the data.
			#endif
		}
	case 12:
		{
			Printer::log("bzip2 decompression not supported. File cannot be read.", ELL_ERROR);
			return 0;
		}
	case 14:
		{	
			Printer::log("lzma decompression not supported. File cannot be read.", ELL_ERROR);
			return 0;
		}
	case 99:
		{
			// If we come here with an encrypted file, decryption support is missing
			Printer::log("Decryption support not enabled. File cannot be read.", ELL_ERROR);
			return 0;
		}
	default:
		{
			swprintf ( buf, 64, L"file has unsupported compression method. %s", Files[index].FullName.c_str() );
			Printer::log( buf, ELL_ERROR);
			return 0;
		}
	}
}
#endif
