// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WIREDXML_H__
#define __WIREDXML_H__

#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/valid.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/file.h>

//#ifdefined	WIN32
//#include <io.h>
//#include <direct.h>
//#pragma comment(lib, "libxml2.lib")
//#define FLAGS_OPEN_RDONLY _O_RDONLY
//#else
#include <unistd.h>
#define FLAGS_OPEN_RDONLY O_RDONLY
//#endif

#define INVALID_FD -1
#define DTD_FILENAME wxT("WiredSession.dtd")
#define XML_EXTENSION wxT(".xml")
#define DTD_EXTENSION wxT(".dtd")
#define TEMP_EXTENSION wxT("~")

class WiredXml
{
public:
	WiredXml();
	WiredXml(const WiredXml& copy);
	~WiredXml();
	WiredXml			operator=(const WiredXml& right);
	WiredXml			Clone();
	const wxString&	GetDocumentName();
protected:
	bool				OpenDocument(const wxString& FileName);
	bool				OpenDtd(const wxString& FileName);
	bool				ValidDocument();
	bool				CreateDocument(const wxString& DocNameName = wxT(""));
	bool				StartElement(const wxString& name);
	bool				WriteElement(const wxString& Name, const wxString& Content, bool Ended = true);
	bool				EndElement();
	bool				StartAttribute(const wxString& Name);
	bool				WriteAttribute(const wxString& Name, const wxString& Content, bool Ended = true);
	bool				EndAttribute();
	bool				StartCDATA();
	bool				WriteCDATA(const wxChar* CData, bool Ended = true);
	bool				EndCDATA();
	bool				WriteBin(void* Data, int start, int len);	
	bool				WriteString(const wxString& Content);
	bool				StartComment();
	bool				WriteComment(const wxString& Comment, bool Ended = true);
	bool				EndComment();
	bool				EndDocumentWriter(bool Save = true, bool Close = true);
	bool				CloseDocumentWriter();
	bool				CloseDocumentReader();
	
	bool				Read();
	wxString			GetNodeName();
	bool				HasValue();
	wxString			GetNodeValue();
	wxString			GetAttribute(wxString Name);
	int				GetNodeType();

	wxString			_DocumentFileName;
	wxString			_DtdFileName;
	wxString			_DocumentWriterName;
	xmlTextReaderPtr		_DocumentFile;
	xmlDtdPtr			_DtdFile;
	xmlTextWriterPtr		_DocumentWriter;
//private:
};

#endif
