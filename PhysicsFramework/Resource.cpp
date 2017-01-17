#include "Resource.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

///////////////////////////////////////////////////////////////////////////////
//	PROTOTYPES

// Lump handling functions for raw data lumps.
static bool LoadRawLump(std::fstream *, pResourceLump);
static bool SaveRawLump(std::fstream *, pResourceLump);
static void UnloadRawLump(pResourceLump);

///////////////////////////////////////////////////////////////////////////////
//	FUNCTIONS

//- LoadRawLump ------------------------------------------------------------
// Description: Loads in the data for a raw data lump.
// Parameters:  aStream - pointer to the file stream object.
//              aLump   - pointer to the lump node.
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
static bool LoadRawLump(std::fstream *aStream, pResourceLump aLump)
{
	// First, allocate enough memory to the load the data into.
	if ((aLump->pData = (void *)(new char[aLump->Size])) == NULL)
		return false;

	// Next, load in the data.
	aStream->read((char*)(aLump->pData), aLump->Size);
	if (aStream->fail())
	{
		delete[] (char *)(aLump->pData);
		aLump->pData = nullptr;
		return false;
	}

	// It worked, so return true.
	return true;
}

//- SaveRawLump ------------------------------------------------------------
// Description: Saves the data of a raw data lump to a file.
// Parameters:  aStream - pointer to the file stream object.
//              aLump   - pointer to the lump node.
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
static bool SaveRawLump(std::fstream *aStream, pResourceLump aLump)
{
	// Write the data to the file.
	aStream->write((char *)(aLump->pData), aLump->Size);
	if (aStream->fail())
		return false;

	// It worked, so return true!
	return true;
}

//- UnloadRawLump ----------------------------------------------------------
// Description: Unloads the data of a raw data lump.
// Parameters:  aLump   - pointer to the lump node.
//-----------------------------------------------------------------------------
static void UnloadRawLump(pResourceLump aLump)
{
	// Delete the memory.
	delete[] (char *)(aLump->pData);
	aLump->pData = nullptr;
}

bool Resource::bHandlerActive = false;
Resource::pResourceLumpHandler Resource	::pLumpHandlerList = nullptr;

//- Resource::GetFileSize -----------------------------------------------------
// Description: Gets the file size of the active file.
// Returns:     Size of the file in bytes, -1 on error.
//-----------------------------------------------------------------------------
long Resource::GetFileSize(void)
{
	// Store the current position.
	std::fstream::pos_type p = fStream.tellp();
	if (fStream.fail())
		return -1;

	// Seek to the end of the file.
	fStream.seekp(0, std::ios_base::end);
	if (fStream.fail())
		return -1;

	// Get the file size.
	long lSize = long(fStream.tellp());
	bool bFail = bool(fStream.fail());

	// Seek back to the original position and return the size.
	fStream.seekp(p, std::ios_base::beg);
	return ((bFail) ? -1 : lSize);
}


//- Resource::GetLumpHandler --------------------------------------------------
// Description: Returns the pointer to the matching lump handler for a lump
//              type. If no match is found, it returns the raw data lump handler.
// Parameters:  dwType - value for the type of lump that our lump handler should
//                       handle.
// Returns:     Pointer to the correct lump handler.
//-----------------------------------------------------------------------------
Resource::pResourceLumpHandler Resource::GetLumpHandler(unsigned long Type)
{
	// Search through the lump handler list looking for a matching lump handler.
	pResourceLumpHandler pLumpHandler = pLumpHandlerList, pRawLumpHandler;
	while (pLumpHandler)
	{
		// if this is the raw data lump handler, cache a pointer to it for
		// possible future reference.
		if (pLumpHandler->Type == LumpType::RAW)
			pRawLumpHandler = pLumpHandler;

		// Check to see if this is a match, if so, return a pointer to the
		// lump handler.
		if (pLumpHandler->Type == Type)
			return pLumpHandler;

		// Move to next lump handler
		pLumpHandler = pLumpHandler->pNextHandler;
	}

	// No matching lump was found, so default to the raw data lump handler.
	return pRawLumpHandler;
}

//- Resource::LoadLumpList ----------------------------------------------------
// Description: Private method for loading in the lump description table from
//              a resource file into a lump information linked list.
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
bool Resource::LoadLumpList(void)
{
	// Make sure a file is open for read or modification mode.
	if ((!fStream.is_open()) || (cFileMode[0] != 'r'))
		return false;

	// Seek to the start of the lump list in the resource file.
	fStream.seekp(resFileHeader.LumpListOffset, std::ios_base::beg);
	if (fStream.fail())
		return false;

	// Read in each node into the linked list.
	pResourceLump *pLump = &pLumpList;
	bool bError = false;

	for (unsigned long i = 0; i < resFileHeader.NumLumps; ++i)
	{
		// Load in the lump information table.
		ResourceLumpInfo lumpInfo;
		fStream.read((char *)(&lumpInfo), sizeof(lumpInfo));
		if (fStream.fail())
		{
			bError = true;
			break;
		}

		// Allocate memory for the node.
		if ((*pLump = new ResourceLump) == nullptr)
		{
			bError = true;
			break;
		}

		// Set up the node.
		(*pLump)->Size = lumpInfo.Size;
		(*pLump)->Offset = lumpInfo.Offset;
		(*pLump)->Type = lumpInfo.Type;
		(*pLump)->bNoFree = false;
		(*pLump)->pData = nullptr;
		(*pLump)->pDataAddress = nullptr;
		(*pLump)->pNextLump = nullptr;

		// Allocate memory for the lump name string.
		if (((*pLump)->pName = new char[lumpInfo.NumChar + 1]) == NULL)
		{
			bError = true;
			break;
		}

		// Load in the name.
		fStream.read((char *)(*pLump)->pName, lumpInfo.NumChar);
		if (fStream.fail())
		{
			bError = true;
			break;
		}

		// Set the terminating NULL character for name string.
		(*pLump)->pName[lumpInfo.NumChar] = NULL;
		pLump = &(*pLump)->pNextLump;
	}

	// Check to see if there were any errors.
	if (bError == true)
	{
		// There was an error, unload what was loaded in.
		UnloadLumpList();
		return false;
	}

	// No errors, so return true.
	return true;
}


//- Resource::SaveLumpList ----------------------------------------------------
// Description: Outputs the lump list to the resource file. Only saves the lump
//              information, not the actual data. The actual data is saved
//              by messaging the method Resource::SaveLumps().
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
bool Resource::SaveLumpList(void)
{
	// Make sure a file is open for write or modification mode.
	if ((!fSaveStream.is_open()) || ((cFileMode[0] != 'w') && (cFileMode[1] != '+')))
		return false;

	// Go through the lump linked list, writing out the info for each node in
	// the list.
	pResourceLump lumpListIterator = pLumpList;
	while (lumpListIterator)
	{
		// Set up the lump info structure.
		ResourceLumpInfo lumpInfo;
		lumpInfo.Size = lumpListIterator->Size;
		lumpInfo.Offset = lumpListIterator->Offset;
		lumpInfo.Type = lumpListIterator->Type;
		lumpInfo.NumChar = unsigned char(strlen(lumpListIterator->pName));

		// Write the lump info structure to the file.
		fSaveStream.write((char *)(&lumpInfo), sizeof(lumpInfo));
		if (fSaveStream.fail())
			return false;

		// Write out the name.
		fSaveStream.write((char *)(lumpListIterator->pName), lumpInfo.NumChar);
		if (fSaveStream.fail())
			return false;

		// Traverse to the next lump on the list.
		lumpListIterator = lumpListIterator->pNextLump;
	}

	// No errors, so return TRUE.
	return true;
}


//- Resource::UnloadLumpList --------------------------------------------------
// Description: Deallocates the resource file manager's lump list. Used when
//              the file is closed.
//-----------------------------------------------------------------------------
void Resource::UnloadLumpList(void)
{
	// Deallocate the entire lump list.
	pResourceLump *lumpListIterator = &pLumpList;
	while (*lumpListIterator)
	{
		// If the data has been loaded in, unload it.
		if (((*lumpListIterator)->pData != NULL) && ((*lumpListIterator)->bNoFree == false))
		{
			pResourceLumpHandler lpHandler = GetLumpHandler((*lumpListIterator)->Type);
			lpHandler->Unload(*lumpListIterator);
			if ((*lumpListIterator)->pDataAddress)
				*(*lumpListIterator)->pDataAddress = NULL;
		}

		// Deallocate the lump name.
		if ((*lumpListIterator)->pName)
			delete[] (char *)((*lumpListIterator)->pName);

		// Traverse to the next node and deallocate the current one.
		pResourceLump lpLump = *lumpListIterator;
		lumpListIterator = &(lpLump->pNextLump);
		delete lpLump;
	}

	// Set the pointer to NULL.
	pLumpList = NULL;
}

//- Resource::SaveLumps -------------------------------------------------------
// Description: Saves the data for each lump to the resource file. Called by
//              Resource::Save() method.
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
bool Resource::SaveLumps(void)
{
	// Make sure a file is open for write or modification mode.
	if ((!fSaveStream.is_open()) || ((cFileMode[0] != 'w') && (cFileMode[1] != '+')))
		return false;

	// Run through the lump linked list, writing the data for each lump to the
	// save resource file.
	pResourceLump pLumpListIterator = pLumpList;
	while (pLumpListIterator)
	{
		// If the data has been loaded in, save it to the new file.
		if (pLumpListIterator->pData)
		{
			// Get the new file offset for the lump.
			pLumpListIterator->Offset = (unsigned long)(fSaveStream.tellp());

			// Find the lump handler for this lump and save the lump.
			pResourceLumpHandler pLumpHandler = GetLumpHandler(pLumpListIterator->Type);
			if (pLumpHandler->Save(&fSaveStream, pLumpListIterator) == false)
				return false;

			// Get the lump's size.
			pLumpListIterator->Size = (unsigned long)(fSaveStream.tellp()) - pLumpListIterator->Offset;
		}
		else
		{
			// Otherwise, copy the lump's data from the old file to the new one.

			// Seek to the lump position in the old file and store the offset
			// of the lump in the new file.
			fStream.seekp(pLumpListIterator->Offset, std::ios_base::beg);
			pLumpListIterator->Offset = (unsigned long)(fSaveStream.tellp());

			// Load in the bytes from the old file and save them out in
			// the new file.
			unsigned long size = pLumpListIterator->Size;
			char cBuffer[2048];

			while (size != 0)
			{
				if (size >= 2048)
				{
					fStream.read(cBuffer, 2048);
					fSaveStream.write(cBuffer, 2048);
					size -= 2048;
				}
				else
				{
					fStream.read(cBuffer, size);
					fSaveStream.write(cBuffer, size);
					size = 0;
				}
			}
		}

		// Traverse to the next lump in the list.
		pLumpListIterator = pLumpListIterator->pNextLump;
	}

	// Return true, as no errors occured.
	return true;
}

void Resource::Shutdown(void)
{
	// Delete the lump handler list.
	pResourceLumpHandler lumpHandler = pLumpHandlerList, pTemp;
	while (lumpHandler)
	{
		pTemp = lumpHandler->pNextHandler;
		delete lumpHandler;
		lumpHandler = pTemp;
	}

	// Set the base node to NULL.
	pLumpHandlerList = NULL;
}

//- CResFile::Constructor -----------------------------------------------------
// Description: Default constructor of this class - clears out data structures.
//-----------------------------------------------------------------------------
Resource::Resource()
{
	// Clear out the object.
	memset(&resFileHeader, 0, sizeof(resFileHeader));
	pLumpList = NULL;
	memset(&cFileName, 0, sizeof(cFileName));
	memset(&cFileMode, 0, sizeof(cFileMode));

	// Register the raw lump handling routines.
	if (bHandlerActive == false)
	{
		RegisterLumpHandler(LumpType::RAW, LoadRawLump, SaveRawLump, UnloadRawLump);

		// Register the shutdown function.
		atexit(Resource::Shutdown);
		bHandlerActive = true;
	}
}

//- Resource::Open ------------------------------------------------------------
// Description: Opens a resource file either for reading from, writing to, or
//              modification. Read mode opens up a resource file for read only
//              operations, and in this mode, the resource file can not be
//              modified. Write mode creates a new resource file (or overwrites
//              any existing file with the same name) and allows the programmer
//              to add lumps to the file. Modification mode opens up an existing
//              file (or creates one if one doesn't exist) and allows the
//              programmer to add more lumps to the file or to load in the lumps.
// Parameters:  aFileName - name of the resource file (may include path) to open.
//              aFileMode - string containing the access mode, can be of the
//                           following contents:
//                          "r"  - Read Mode
//  						"w"  - Write Mode
//  						"r+" - Modification Mode
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
bool Resource::Open(const char * aFileName, const char * aFileMode)
{
	// If the file is already open, then return false.
	if (fStream.is_open())
		return false;

	// Set up the file name.
	if (aFileName != cFileName)
		strcpy(cFileName, aFileName);

	// Copy the file mode into our local buffer.
	memset(cFileMode, 0, sizeof(cFileMode));
	strcpy(cFileMode, aFileMode);

	if (stricmp(cFileMode, "w") == 0)
	{
		// Open up the file for read/write replace mode.
		fStream.open(cFileName, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
		// If opening file failed
		if (!fStream.is_open())
			return false;

		// Set up the header.
		memset(&resFileHeader, 0, sizeof(resFileHeader));
		strncpy(resFileHeader.Signature, RESFILE_SIGNATURE, 4);
		resFileHeader.Version++;

		// We're done as far as setting things up for a new resource file,
		// so return true...
		return true;
	}
	else
	{
		// Open the file for read only.
		if (stricmp(cFileMode, "r") == 0)
			fStream.open(cFileName, std::ios_base::in | std::ios_base::binary);
		// Open the file for modification.
		else if (stricmp(cFileMode, "r+") == 0)
			fStream.open(cFileName, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		// Invalid file mode
		else
			return false;

		// Make sure the file was opened properly.
		if (!fStream.is_open())
			return false;

		// Load in the header and make sure it's a valid resource file.
		fStream.seekp(0, std::ios_base::beg);
		fStream.read((char *)(&resFileHeader), sizeof(resFileHeader));
		if (fStream.fail())
		{
			fStream.close();
			return false;
		}

		// Check the signature and the checksum.
		if ((strnicmp(resFileHeader.Signature, RESFILE_SIGNATURE, sizeof(resFileHeader.Signature)) != 0) ||
			(((unsigned long)(GetFileSize()) ^ 0xFFFFFFFF) != resFileHeader.Checksum))
		{
			// Invalid resource file.
			fStream.close();
			return false;
		}

		//// Make sure the version of the file isn't greater than the library version.
		//unsigned long version = ((unsigned long)(resFileHeader.Version) << 8) | (unsigned long)(resFileHeader.Version);
		//if (version > ((RESFILE_VERHI << 8) | RESFILE_VERLO))
		//{
		//	fStream.close();
		//	return FALSE;
		//}

		// Load in the lump list (if any).
		if (LoadLumpList() == false)
		{
			fStream.close();
			return false;
		}

		// Everything has gone smoothly, so return true.
		return true;
	}
}


//- Resource::Save ------------------------------------------------------------
// Description: Saves the contents of a resource file in memory to disk. If
//              for some reason you want to save the file with an alternate
//              file name (ie. SaveAs operations), then pass the new file name
//              string pointer in the "aAltFileName" parameter. Otherwise,
//              you can just pass NULL for this parameter to keep the original
//              file name.
// Parameters:  aAltFileName - alternate file name.
// Returns:     TRUE if successful, FALSE otherwise.
//-----------------------------------------------------------------------------
bool Resource::Save(const char * aAltFileName)
{
	// Make sure a file is open for write or modification mode.
	if ((!fStream.is_open()) || ((cFileMode[0] != 'w') && (cFileMode[1] != '+')))
		return false;

	// Open up a temporary resource file to save the new contents to.
	fSaveStream.open(RESFILE_TEMPNAME, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	if (!fSaveStream.is_open())
		return false;

	// Write out the header. Note that we must later write it out again after
	// it's fields have been updated, but we're merely doing this so that the
	// lumps are aligned properly.
	fSaveStream.write((char *)(&resFileHeader), sizeof(resFileHeader));
	// If writing header failed
	if (fSaveStream.fail())
		goto fail;

	// Save all of the lump data to the resource file.
	if (SaveLumps() == false)
		goto fail;

	// Get the offset of the lump list and store it in the header.
	resFileHeader.LumpListOffset = (unsigned long)(fSaveStream.tellp());
	if (fSaveStream.fail())
		goto fail;

	// Save the lump list to the resource file.
	if (SaveLumpList() == false)
		goto fail;

	// Get the file size and create the checksum.
	resFileHeader.Checksum = 0xFFFFFFFF ^ (unsigned long)(fSaveStream.tellp());
	if (fSaveStream.fail())
		goto fail;

	// Seek back to the start of the file and write out the new updated header.
	fSaveStream.seekp(0, std::ios_base::beg);
	if (fSaveStream.fail())
		goto fail;
	fSaveStream.write((char *)(&resFileHeader), sizeof(resFileHeader));
	if (fSaveStream.fail())
		goto fail;

	// Close the files, and delete the old file if we're overwriting it and
	// rename the temporary file to our original or alternate name.
	fSaveStream.close();
	Close();

	if (aAltFileName)
	{
		// Aquire the full path name to the file using the Win32 API function.
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa364963(v=vs.85).aspx
		LPSTR pFilePart;
		if (GetFullPathName(aAltFileName, MAX_PATH, cFileName, &pFilePart) != MAX_PATH)
			return FALSE;
	}
	else
	{
		// Delete the old file.
		remove(cFileName);
	}

	// After renaming the temporary file, load it back in.
	rename(RESFILE_TEMPNAME, cFileName);
	return Open(cFileName, "r+");

	// If this method failed, close the save file and delete it.
fail:
	fSaveStream.close();
	remove(RESFILE_TEMPNAME);
	return FALSE;
}

//- Resource::Close -----------------------------------------------------------
// Description: Closes the resource file if one is currently open.
//-----------------------------------------------------------------------------
void Resource::Close(void)
{
	// If the file isn't open, return.
	if (!fStream.is_open())
		return;

	// Unload the lumps and the lump list.
	UnloadLumpList();

	// Close the file.
	fStream.close();
}

//- Resource::RegisterLumpHandler ---------------------------------------------
// Description: Registers a lump handler with the Resource class. A lump handler
//              consists of three functions, one each for loading, saving, and
//              unloading a lump of a particular type. You must also pass the
//              value which will used to identify whether lumps should be
//              handled by the lump handler or not - each value must be unique.
// Parameters:  Type   - the type of lumps this handler will handle.
//              pLoad   - pointer to the loading function.
//              pSave   - pointer to the saving function.
//              pUnload - pointer tote unloading function.
// Returns:     TRUE if successful, FALSE otherwise.
//----------------------------------------------------------------------------
bool Resource::RegisterLumpHandler(
	LumpType Type, 
	bool (*aLoad)(std::fstream *, pResourceLump), 
	bool (*aSave)(std::fstream *, pResourceLump), 
	void (*aUnload)(pResourceLump)	)
{
	// Traverse to the end of the lump handler list where we will add a new
	// lump handler node.
	pResourceLumpHandler *lplpHandler = &pLumpHandlerList;
	while (*lplpHandler)
	{
		// Make sure that a handler for this lump type hasn't already been
		// registered...
		if ((*lplpHandler)->Type == Type)
			return FALSE;

		// traverse to the next node on the list.
		lplpHandler = &((*lplpHandler)->pNextHandler);
	}

	// Allocate memory for the new lump handler and attach it to the list.
	if ((*lplpHandler = new ResourceLumpHandler) == NULL)
		return FALSE;

	// Set up the lump handler node.
	(*lplpHandler)->Type = Type;
	(*lplpHandler)->Load = aLoad;
	(*lplpHandler)->Save = aSave;
	(*lplpHandler)->Unload = aUnload;
	(*lplpHandler)->pNextHandler = NULL;

	// Return true now that we have successfully registered a new lump handler.
	return TRUE;
}
