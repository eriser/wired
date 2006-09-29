// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "WiredXml.h"

WiredXml::WiredXml()
{
	_DocumentWriter = NULL;
	_DocumentFile = NULL;
	_DtdFile = NULL;
	_DocumentFileName = wxT("");	
	_DtdFileName = wxT("");
	_DocumentWriterName = wxT("");
	//LIBXML_TEST_VERSION
}

WiredXml::WiredXml(const WiredXml& copy)
{
	*this = copy;
}

WiredXml::~WiredXml()
{
	CloseDocumentWriter();
	CloseDocumentReader();
    xmlCleanupParser();
}

WiredXml				WiredXml::operator=(const WiredXml& right)
{
  std::cerr << "WARNING : Soon, Wired will fail" << std::endl;

  // Ptr must NOT be copied, but content of data does.

	if (this != &right)
	{
		_DocumentFileName = right._DocumentFileName;
		_DocumentFile = right._DocumentFile;
		_DtdFileName = right._DtdFileName;
		_DtdFile = right._DtdFile;
		_DocumentWriter = right._DocumentWriter;
		_DocumentWriterName = right._DocumentWriterName;
		
	}
	return *this;
}

bool					WiredXml::OpenDocument(const wxString& FileName)
{
	if (FileName.size() > 0)
		_DocumentFileName = FileName;
	if (wxFile::Exists(_DocumentFileName.c_str()))
	{	
		//TODO Don't bypass document validation
		//_DocumentFile = xmlReaderForFile(_DocumentFileName.c_str(), NULL, 
		//								XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID);
		_DocumentFile = xmlReaderForFile(_DocumentFileName.mb_str(*wxConvCurrent), NULL, 0);
		if (_DocumentFile != NULL)
			return true;
	}
	return false;
}

bool					WiredXml::OpenDtd(const wxString& FileName)
{
	//TODO Add the Dtd to the project ressource
	return true;
	if (FileName.size() > 0)
		_DtdFileName = FileName;
	if (wxFile::Exists(_DtdFileName.c_str()))
	{
        _DtdFile = xmlParseDTD(NULL, ((const xmlChar*)(const char *)_DtdFileName.mb_str()));
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

bool					WiredXml::CreateDocument(const wxString& DocName)
{
	CloseDocumentWriter();
	if (DocName.Cmp(wxT("")) == 0)
	{
		_DocumentWriterName = _DocumentFileName + TEMP_EXTENSION;
	}
	else
		_DocumentWriterName = DocName;
	if ((_DocumentWriter = xmlNewTextWriterFilename(_DocumentWriterName.mb_str(*wxConvCurrent), 0)) != NULL)
		if (xmlTextWriterSetIndent(_DocumentWriter, 4) == 0)
			if (xmlTextWriterStartDocument(_DocumentWriter, NULL, NULL, NULL) >= 0)
				//if (xmlTextWriterWriteDTDExternalEntity(_DocumentWriter, FALSE, ) >= 0)
				return true;
	return false;
}

bool					WiredXml::StartElement(const wxString& Name)
{
  if (_DocumentWriter != NULL)
    return (xmlTextWriterStartElement(_DocumentWriter,
				      (xmlChar *)((const char*)Name.mb_str())) >= 0);
  else
    return false;
}

bool					WiredXml::WriteElement(const wxString& Name, const wxString& Content, bool Ended)
{
  if (_DocumentWriter != NULL)
    {
      if (xmlTextWriterWriteElement(_DocumentWriter, (xmlChar*)((const char*)Name.mb_str()),
				    (xmlChar*)((const char*)Content.mb_str())) >= 0)
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

bool 					WiredXml::WriteCDATA(const wxChar* CData, bool Ended)
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
    return (xmlTextWriterEndCDATA(_DocumentWriter) >= 0);
  return true;
}

// TODO : Need to use wxDataInputStream or wxDataOutputStream instead void *

bool					WiredXml::WriteBin(void* Data, int start, int len)
{
  if (_DocumentWriter != NULL)
    return (xmlTextWriterWriteBinHex(_DocumentWriter, (const char *)Data, start, len) >= 0);
  return false;
}

bool					WiredXml::WriteString(const wxString& Content)
{
  if (_DocumentWriter != NULL)
    return (xmlTextWriterWriteString(_DocumentWriter, (xmlChar*)((const char *)Content.mb_str())) >= 0);
  return false;
}

bool					WiredXml::StartComment()
{
  if (_DocumentWriter != NULL)
    return (xmlTextWriterStartComment(_DocumentWriter) >= 0);
  return false;
}

bool					WiredXml::WriteComment(const wxString& Comment, bool Ended)
{
  if (_DocumentWriter != NULL)
    {
      if (xmlTextWriterWriteComment(_DocumentWriter, ((xmlChar*)(const char *) Comment.mb_str())) >= 0)
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
    return (xmlTextWriterEndComment(_DocumentWriter) >= 0);
  return false;
}

bool					WiredXml::StartAttribute(const wxString& Name)
{
  if (_DocumentWriter != NULL)
    return (xmlTextWriterStartAttribute(_DocumentWriter, ((xmlChar *)(const char *) Name.mb_str())) >= 0);
  return false;
}
		
bool					WiredXml::WriteAttribute(const wxString& Name, const wxString& Content, bool Ended)
{
  if (_DocumentWriter != NULL)
    {
      if (xmlTextWriterWriteAttribute(_DocumentWriter,
				      ((xmlChar*)(const char *)Name.mb_str()),
				      ((xmlChar*)(const char *)Content.mb_str())) >= 0)
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
    return (xmlTextWriterEndAttribute(_DocumentWriter) >= 0);
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

wxString			       WiredXml::GetNodeName()
{
  if (_DocumentFile != NULL)
    return (wxString((const char*)xmlTextReaderConstName(_DocumentFile), wxConvUTF8));
  return wxString();
}

wxString				WiredXml::GetNodeValue()
{
  if (_DocumentFile != NULL)
    {
      if (HasValue() == true)
	return (wxString((const char*)xmlTextReaderConstValue(_DocumentFile), wxConvUTF8));
    }
  return wxString();
}

bool					WiredXml::HasValue()
{
	if (_DocumentFile != NULL)	
		if (xmlTextReaderHasValue(_DocumentFile) == 1)
			return true;
	return false;
}

wxString				WiredXml::GetAttribute(wxString Name)
{
  if (_DocumentFile != NULL)
    {
      return (wxString((const char*)xmlTextReaderGetAttribute(_DocumentFile,
						 (xmlChar*)((const char*)Name.mb_str())),
						 wxConvUTF8));
    }
  return wxString();
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

const wxString&		WiredXml::GetDocumentName()
{
	return _DocumentFileName;
}
