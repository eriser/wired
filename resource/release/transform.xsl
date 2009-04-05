
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 <xsl:output
	method="text"
	omit-xml-declaration="yes"
	indent="no" />

 <!-- include Arthur de Jong's svn2cl -->
 <xsl:include href="svn2cl.xsl" />

  <xsl:template match="/log">
   <xsl:for-each select="logentry">
    <xsl:text>- </xsl:text>
    <xsl:call-template name="trim-newln">
     <xsl:with-param name="txt" select="msg" />
    </xsl:call-template>
    <!-- A new line -->
    <xsl:text>&#10;</xsl:text>
   </xsl:for-each>
   <!-- A new line -->
   <xsl:text>&#10;</xsl:text>
  </xsl:template>


</xsl:stylesheet>

