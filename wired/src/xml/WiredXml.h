// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WIREDXML_H__
#define __WIREDXML_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/file.h>

#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// TODO: delete this line after using next wx release (> 2009/03/11)
// it avoid >=libxml2-2.7.3 name conflict
#undef ATTRIBUTE_PRINTF

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/valid.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

/*
#ifdef WIN32
#include <io.h>
#include <direct.h>
//#pragma comment(lib, "libxml2.lib")
#define FLAGS_OPEN_RDONLY _O_RDONLY
#else
*/
#include <unistd.h>
#define FLAGS_OPEN_RDONLY O_RDONLY
//#endif

#define INVALID_FD -1
#define DTD_FILENAME wxT("WiredSession.dtd")
#define XML_EXTENSION wxT(".xml")
#define DTD_EXTENSION wxT(".dtd")
#define TEMP_EXTENSION wxT("~")

/**
 * Interface with the libxml.
 * It can read and write XML files, based on the
 * <a href=" http://xmlsoft.org/">libxml</a>.<br>
 * Most of the methods are just interfacing the methods of the
 * <a href=" http://xmlsoft.org/html/libxml-xmlwriter.html">xmlwriter</a>
 * and <a href=" http://xmlsoft.org/html/libxml-xmlreader.html">xmlreader</a>
 * objects of the library. <br>
 * For further informations, you'd better have a look to this lib doc. <br>
 */
class WiredXml
{
 public:

        /** Default constructor. Inits everything to NULL or "" */
	WiredXml();

	/** Copy constructor. Duplicates an already existing object into a new one */
	WiredXml(const WiredXml& copy);

	/** Default destructor */
	~WiredXml();

	/** operator= overload.
	 * Duplicates the object on the right operand into this one
	 * \param right the WiredXml object to be copied.
	 * \return the current object.
	*/
	WiredXml			operator=(const WiredXml& right);

	/** Returns the currently opened document name.
	 * \return the name of the document.
	 */
	const wxString&	GetDocumentName();

	/** Opens a document.
	 * \param FileName The path to the document.
	 * \return true on success, else false.
	 */
	bool				OpenDocument(const wxString& FileName);

	/** Opens a DTD file.
	 * /!\ THIS METHOD IS PARTIALLY IMPLEMENTED /!\
	 * it will only return true.
	 * \param FileName The path to the document.
	 * \return true on success, else false.
	 */
	bool				OpenDtd(const wxString& FileName);

	/** Validates the opened document against the opened DTD.
	 *
	 * /!\ THIS METHOD IS PARTIALLY IMPLEMENTED /!\
	 * it will only return true.
	 * \return true on successful validation, else false.
	 */
	bool				ValidDocument();

	/** Creates a new XML document.
	 * \param DocName Path to the file to create. If empty, a default temp file name will be used.
	 * \return true on success, else false
	 */
	bool				CreateDocument(const wxString& DocName = wxT(""));

	/** Starts a new element.
	 * This is mainly a call to xmlTextWriterStartElement.
	 * \param name Name of the element.
	 * \return true on success, else false.
	 */
	bool				StartElement(const wxString& name);

	/** Writes an element.
	 * This is mainly a call to xmlTextWriterWriteElement.
	 * \param Name Name of the tag.
	 * \param Content COntent of the tag.
	 * \param Ended If true, the tag will also be closed by calling EndElement().
	 * \return true on success, else false.
	 * \see EndElement
	 */
	bool				WriteElement(const wxString& Name, const wxString& Content, bool Ended = true);

	/** Ends an element.
	 * This is mainly a call to xmlTextWriterEndElement.
	 * \return true on success, else false.
	 */
	bool				EndElement();

	/** Starts an attribute.
	 * This is mainly a call to xmlTextWriterStartAttribute.
	 * \param Name The nane of the attribute
	 * \return true on success, else false
	 */
	bool				StartAttribute(const wxString& Name);

	/** Writes an attribute.
	 * This is mainly a call to xmlTextWriterWriteAttribute.
	 * \param Name Name of the attribute to be written.
	 * \param Content Content of the attribute.
	 * \param Ended If true, the attribute will also be closed by calling EndAttribute().
	 * \return true on success, else false.
	 * \see EndAttribute
	 */
	bool				WriteAttribute(const wxString& Name, const wxString& Content, bool Ended = true);

	/** Ends an attribute
	 * This is mainly a call to xmlTextWriterEndAttribute.
	 * \return true on success, else false.
	 */
	bool				EndAttribute();

	/** Starts a CDATA section.
	 * This is mainly a call to xmlTextWriterStartCDATA.
	 * \return true on success, else false.
	 */
	bool				StartCDATA();

	/** Writes CDATA.
	 * \param CData the data to be written
	 * \param Ended If true, the CDATA section will also be closed by calling EndCDATA().
	 * \return true on success, else false.
	 */
	bool				WriteCDATA(const wxChar* CData, bool Ended = true);

	/** Ends a CDATA section.
	 * This is mainly a call to xmlTextWriterEndCDATA.
	 * \return true on success, else false
	 */
	bool				EndCDATA();

	/** Writes binary data.
	 * This is mainly a call to xmlTextWriterWriteBinHex.
	 * \param Data The chunk to be written.
	 * \param start The position within the data of the first byte to encode.
	 * \param len The number of bytes to encode.
	 * \return true on success, else false.
	 */
	bool				WriteBin(void* Data, int start, int len);

	/** Write an xml text.
	 * This is mainly a call to xmlTextWriterWriteString.
	 * \param Content the string to be written.
	 * \return true on success, else false.
	 */
	bool				WriteString(const wxString& Content);

	/** Starts a comment
	 * This is mainly a call to xmlTextWriterStartComment.
	 * \return true on success, else false.
	 */
	bool				StartComment();

	/** Writes a comment.
	 * This is mainly a call to xmlTextWriterWriteComment.
	 * \param Comment The comment to be written.
	 * \param Ended If true, the comment will also be closed by calling EndComment().
	 * \return true on success, else false.
	 * \see EndComment
	 */
	bool				WriteComment(const wxString& Comment, bool Ended = true);

	/** Ends a comment
	 * \return true on success, else false.
	 */
	bool				EndComment();


	/** Ends the document writer object.
	 * It closes the object, letting the possibility to save the remaining data and to close definitely the object.
	 * \param Save If true, data will be flushed using xmlTextWriterFlush.
	 * \param Close If true, the Document Writer will be destroyed using CloseDocumentWriter()
	 * \return true on success, else false.
	 * \see CloseDocumentWriter
	 */
	bool				EndDocumentWriter(bool Save = true, bool Close = true);

	/** Destroys the DocumentWriter object.
	 * \return true on success, else false.
	 */
	bool				CloseDocumentWriter();

	/** Destroys the DocumentReader.
	 * \return true on success, else false.

	 */
	bool				CloseDocumentReader();

	/** Reads one element in the document.
	 * If the data read is a white space (significant or not) this method is called recursively.
	 * \return true on success, else false.
	 */

	bool				Read();

	/** Gets the name of the current node.
	 * This is mainly a call to xmlTextReaderConstName.
	 * \return true on success, else false.
	 */
	wxString			GetNodeName();

	/** Checks whether the node can have a text value.
	 * This is mainly a call to xmlTextReaderHasValue.
	 * \return true if he can, else false.
	 */
	bool				HasValue();

	/** Gets the text value of the node.
	 * This is mainly a call to xmlTextReaderConstValue.
	 * \return the text value of the node, else an empty wxString.
	 */
	wxString			GetNodeValue();

	/** Gets an attribute value for the current node.
	 * This is mainly a call to xmlTextReaderGetAttribute.
	 * \param Name The name of the attribute.
	 * \return The value of the attribute, else an empty wxString
	 */
	wxString			GetAttribute(wxString Name);

	int				GetAttributeCount();

	wxString			GetAttributeName(int no);
	wxString			GetAttributeValue(int no);

	bool				HasChildren();
	int				GetDepth();

	/** Gets the node type of the current node.
	 * This is mainly a call to xmlTextReaderNodeType. <br>
	 * The different node types are discussed
	 * <a href="http://dotgnu.org/pnetlib-doc/System/Xml/XmlNodeType.html">there</a>
	 * return The node type on success, else 0.
	 */
	int				GetNodeType();

	/** The path to the opened XML file. */
	wxString			_DocumentFileName;

	/** The path to the opened DTD file. DTD are currently unsupported... */
	wxString			_DtdFileName;

	/** The path to the file to be written. */
	wxString			_DocumentWriterName;

	/** A pointer to the xmlTextReader object. This object is defined by the libxml. */
	xmlTextReaderPtr		_DocumentFile;

	/** A pointer to the xmlDtd object. This object is defined by the libxml. */
	xmlDtdPtr			_DtdFile;

	/** A pointer to the xmlTextWriter object. This object is defined by the libxml. */
	xmlTextWriterPtr		_DocumentWriter;
//private:
};

#endif
