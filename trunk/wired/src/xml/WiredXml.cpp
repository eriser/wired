#include "WiredXml.h"

WiredXml::WiredXml()
{
	std::cout << "WiredXmlConstructor" << std::endl;
	_DocumentWriter = NULL;
	_DocumentFile = NULL;
	_DtdFile = NULL;
	_DocumentFileName = "";	
	_DtdFileName = "";
	_DocumentWriterName = "";
	std::cout << "WiredXml test lib" << std::endl;
	//LIBXML_TEST_VERSION
	std::cout << "WiredXmlConstructor end" << std::endl;
}

WiredXml::WiredXml(const WiredXml& copy)
{
	*this = copy;
}

WiredXml::~WiredXml()
{
	std::cout << "[WiredXml] destructor 1" << std::endl;
	CloseDocumentWriter();
	std::cout << "[WiredXml] destructor 2" << std::endl;
	CloseDocumentReader();
	std::cout << "[WiredXml] destructor 3" << std::endl;
    xmlCleanupParser();
    std::cout << "[WiredXml] destructor 4" << std::endl;
}

WiredXml				WiredXml::operator=(const WiredXml& right)
{
	if (this != &right)
	{
		_DocumentFileName = right._DocumentFileName;
		_DocumentFile = right._DocumentFile;
		_DtdFileName = right._DtdFileName;
		_DtdFile = right._DtdFile;
		_DocumentWriter = right._DocumentWriter;
		_DocumentWriter = right._DocumentWriter;	
		
	}
	return *this;
}

WiredXml				WiredXml::Clone()
{
	return *this;
}

bool					WiredXml::OpenDocument(const std::string& FileName)
{
	int					fd = INVALID_FD;
		
	if (FileName.size() > 0)
		_DocumentFileName = FileName;
	fd = open(_DocumentFileName.c_str(), FLAGS_OPEN_RDONLY);
	if (fd != INVALID_FD)
	{
		close(fd);
		
		//TODO Don't bypass document validation
		//_DocumentFile = xmlReaderForFile(_DocumentFileName.c_str(), NULL, 
		//								XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID);
		_DocumentFile = xmlReaderForFile(_DocumentFileName.c_str(), NULL, 0);
		if (_DocumentFile != NULL)
			return true;
	}
	return false;
}

bool					WiredXml::OpenDtd(const std::string& FileName)
{
	//TODO Add the Dtd to the project ressource
	return true;
	int					fd;

	if (FileName.size() > 0)
		_DtdFileName = FileName;
	if ((fd = open(_DtdFileName.c_str(), FLAGS_OPEN_RDONLY)) != INVALID_FD)
	{
		close(fd);
		_DtdFile = xmlParseDTD(NULL, (const xmlChar*) _DtdFileName.c_str());
		if (_DtdFile != NULL)
			return true;
	}
	return false;
}

bool					WiredXml::ValidDocument()
{
	//TODO Manage the document validation
	return true;
	xmlValidCtxtPtr 	ctxt;
	
	bool				Valid = false;

	if ((_DocumentFile != NULL) && (_DtdFile != NULL))
	{
		if (xmlTextReaderIsValid(_DocumentFile) == 1)
			Valid = true;
//		if ((ctxt = xmlNewValidCtxt()) != NULL)
//		{
//			if (xmlValidateDtd(ctxt, _DocumentFile, _DtdFile) == 1)
//				Valid = true;
//			xmlFreeValidCtxt(ctxt);
//		}
	}
    return Valid;
}

bool					WiredXml::CreateDocument(const std::string& DocName)
{
	CloseDocumentWriter();
	if (strcmp(DocName.c_str(), "") == 0)
	{
		_DocumentWriterName = _DocumentFileName + TEMP_EXTENSION;
	}
	else
		_DocumentWriterName = DocName;
	if ((_DocumentWriter = xmlNewTextWriterFilename(_DocumentWriterName.c_str(), 0)) != NULL)
		if (xmlTextWriterSetIndent(_DocumentWriter, 4) == 0)
			if (xmlTextWriterStartDocument(_DocumentWriter, NULL, NULL, NULL) >= 0)
				//if (xmlTextWriterWriteDTDExternalEntity(_DocumentWriter, FALSE, ) >= 0)
				return true;
	return false;
}

bool					WiredXml::StartElement(const std::string& Name)
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterStartElement(_DocumentWriter, (xmlChar *)Name.c_str()) >= 0;
	else
		return false;
}

bool					WiredXml::WriteElement(const std::string& Name, const std::string& Content, bool Ended)
{
	if (_DocumentWriter != NULL)
	{
		if (xmlTextWriterWriteElement(_DocumentWriter, (xmlChar*)Name.c_str(), (xmlChar*)Content.c_str()) >= 0)
		{
			if (Ended == true)
				return EndElement();
			return true;
		}
	}
	return false;
}

bool 					WiredXml::EndElement()
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterEndElement(_DocumentWriter) >= 0;
	return false;
}

bool					WiredXml::StartCDATA()
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterStartCDATA(_DocumentWriter) >= 0;
	else
		return false;
}	

bool 					WiredXml::WriteCDATA(const char* CData, bool Ended)
{
	if (_DocumentWriter != NULL)
	{
		if (xmlTextWriterWriteCDATA(_DocumentWriter, (xmlChar *)CData) >= 0)
		{
			if (Ended == true)
				return EndCDATA();
			return true;
		}
	}
	return false;
}

bool					WiredXml::EndCDATA()
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterEndCDATA(_DocumentWriter) >= 0;
	return true;
}

bool					WiredXml::WriteBin(const char* Data, int start, int len)
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterWriteBinHex(_DocumentWriter, Data, start, len) >= 0;
	return false;
}

bool					WiredXml::WriteString(const std::string& Content)
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterWriteString(_DocumentWriter, (xmlChar*) Content.c_str()) >= 0;
	return false;
}

bool					WiredXml::WriteString(const char* Content)
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterWriteString(_DocumentWriter, (xmlChar*) Content) >= 0;
	return false;
}

bool					WiredXml::StartComment()
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterStartComment(_DocumentWriter) >= 0;
	return false;
}

bool					WiredXml::WriteComment(const std::string& Comment, bool Ended)
{
	if (_DocumentWriter != NULL)
	{
		if (xmlTextWriterWriteComment(_DocumentWriter, (xmlChar*) Comment.c_str()) >= 0)
		{
			if (Ended == true)
				return EndComment();
			return true;
		}		
	}
	return false;
}

bool					WiredXml::EndComment()
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterEndComment(_DocumentWriter) >= 0;
	return false;
}

bool					WiredXml::StartAttribute(const std::string& Name)
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterStartAttribute(_DocumentWriter, (xmlChar *) Name.c_str()) >= 0;	
	return false;
}
		
bool					WiredXml::WriteAttribute(const std::string& Name, const std::string& Content, bool Ended)
{
	if (_DocumentWriter != NULL)
	{
		if (xmlTextWriterWriteAttribute(_DocumentWriter, (xmlChar*)Name.c_str(), (xmlChar*)Content.c_str()) >= 0)
		{
			if (Ended == true)
				return EndAttribute();
			return true;
		}
	}
	return false;
}

bool					WiredXml::EndAttribute()
{
	if (_DocumentWriter != NULL)
		return xmlTextWriterEndAttribute(_DocumentWriter) >= 0;
	return false;
}

bool					WiredXml::EndDocumentWriter(bool Save, bool Close)
{
	if (_DocumentWriter != NULL)
	{
		if (xmlTextWriterEndDocument(_DocumentWriter) >= 0)
		{
			bool		HasSaved = true;
			bool		HasClosed = true;

			if (Save == true)
				HasSaved = xmlTextWriterFlush(_DocumentWriter) >= 0;
			if (Close == true)
				HasClosed = CloseDocumentWriter();
			if (HasClosed == true && HasSaved == true)
				return true;
		}
	}
	return  false;
}

bool					WiredXml::Read()
{
	if (_DocumentFile != NULL)
	{
		int				res = 0;

		res = xmlTextReaderRead(_DocumentFile);
		if (GetNodeType() == XML_READER_TYPE_WHITESPACE || 
			GetNodeType() == XML_READER_TYPE_SIGNIFICANT_WHITESPACE)
			return Read();
		return res == 1;		
	}
	return false;
}

char					*WiredXml::GetNodeName()
{
	if (_DocumentFile != NULL)
		return (char*)xmlTextReaderConstName(_DocumentFile);
	return NULL;
}

char					*WiredXml::GetNodeValue()
{
	if (_DocumentFile != NULL)
	{
		if (HasValue() == true)
			return (char*)xmlTextReaderConstValue(_DocumentFile);
	}
	return NULL;
}

bool					WiredXml::HasValue()
{
	if (_DocumentFile != NULL)	
		if (xmlTextReaderHasValue(_DocumentFile) == 1)
			return true;
	return false;
}
 
char					*WiredXml::GetAttribute(const char *Name)
{
	if (_DocumentFile != NULL)
		return (char*)xmlTextReaderGetAttribute(_DocumentFile, (xmlChar*)Name);
	return NULL;
}

bool					WiredXml::CloseDocumentWriter()
{
	if (_DocumentWriter != NULL)
	{
		_DocumentWriterName.erase();
		xmlFreeTextWriter(_DocumentWriter);
		_DocumentWriter = NULL;	
		return true;
	}
	return false;
}

bool					WiredXml::CloseDocumentReader()
{
	if (_DocumentFile != NULL)
	{
		_DocumentFileName.erase();
		if (xmlTextReaderIsValid(_DocumentFile))
			xmlTextReaderClose(_DocumentFile);
		xmlFreeTextReader(_DocumentFile);
		_DocumentFile = NULL;	
		return true;
	}
	return false;
}

int						WiredXml::GetNodeType()
{
	if (_DocumentFile != NULL)
		return xmlTextReaderNodeType(_DocumentFile);
	return 0;
}

const std::string&		WiredXml::GetDocumentName()
{
	return _DocumentFileName;
}
