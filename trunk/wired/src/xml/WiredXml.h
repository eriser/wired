#ifndef __WIREDXML_H__
#define __WIREDXML_H__

#include <string>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/valid.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

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
#define DTD_FILENAME "WiredSession.dtd"
#define XML_EXTENSION ".xml"
#define DTD_EXTENSION ".dtd"
#define TEMP_EXTENSION "~"

class WiredXml
{
public:
	WiredXml();
	WiredXml(const WiredXml& copy);
	~WiredXml();
	WiredXml			operator=(const WiredXml& right);
	WiredXml			Clone();
	const std::string&	GetDocumentName();
protected:
	bool				OpenDocument(const std::string& FileName);
	bool				OpenDtd(const std::string& FileName);
	bool				ValidDocument();
	bool				CreateDocument(const std::string& DocNameName = "");
	bool				StartElement(const std::string& name);
	bool				WriteElement(const std::string& Name, const std::string& Content, bool Ended = true);
	bool				EndElement();
	bool				StartAttribute(const std::string& Name);
	bool				WriteAttribute(const std::string& Name, const std::string& Content, bool Ended = true);
	bool				EndAttribute();
	bool				StartCDATA();
	bool				WriteCDATA(const char* CData, bool Ended = true);
	bool				EndCDATA();
	bool				WriteBin(const char* Data, int start, int len);	
	bool				WriteString(const std::string& Content);
	bool				WriteString(const char* Content);
	bool				StartComment();
	bool				WriteComment(const std::string& Comment, bool Ended = true);
	bool				EndComment();
	bool				EndDocumentWriter(bool Save = true, bool Close = true);
	bool				CloseDocumentWriter();
	bool				CloseDocumentReader();
	
	bool				Read();
	char				*GetNodeName();
	bool				HasValue();
	char				*GetNodeValue();
	char				*GetAttribute(const char *Name);
	int					GetNodeType();

	std::string			_DocumentFileName;
	std::string			_DtdFileName;
	std::string			_DocumentWriterName;
	xmlTextReaderPtr	_DocumentFile;
	xmlDtdPtr			_DtdFile;
	xmlTextWriterPtr	_DocumentWriter;
//private:
};

#endif
