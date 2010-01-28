<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:tb="http://www.literacybridge.org/2008/book">
    <xsl:output method="text" encoding="UTF-8"/>
    <xsl:template match="@* | text() | *"/>
    
    <xsl:template match="/tb:Package">VERSION:<xsl:value-of select="@Version"/>
TIME-PRECISION:<xsl:value-of select="@Precision"/>
---
P:<xsl:value-of select="@Name"/><xsl:apply-templates select="*"></xsl:apply-templates></xsl:template>
    
    <xsl:template match="tb:File">

  F:<xsl:value-of select="@Name"/>
    <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="tb:Block">
    B:<xsl:if test="tb:OnStart/tb:Stop | tb:OnStart/tb:Pause"><xsl:apply-templates select="tb:OnStart/*"></xsl:apply-templates></xsl:if>
        <xsl:value-of select="@Start"/> - <xsl:value-of select="@End"/>
        <xsl:if test="tb:OnEnd/tb:Stop | tb:OnEnd/tb:Pause"><xsl:apply-templates select="tb:OnEnd/*"></xsl:apply-templates></xsl:if>
        <xsl:if test="@Class"> {<xsl:value-of select="@Class"/>}</xsl:if>
        <xsl:if test="@Hyperlinked"> ? </xsl:if>
        <xsl:if test="@Name">   [<xsl:value-of select="@Name"/>]    </xsl:if>
        <xsl:if test="tb:OnEnter | tb:OnExit">    <xsl:apply-templates select="tb:OnEnter/*"/>/<xsl:apply-templates select="tb:OnExit/*"/></xsl:if>
        <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="tb:OnStart">
        A!:<xsl:if test="@InsertSoundBlock">I[<xsl:value-of select="@InsertSoundBlock"/>]</xsl:if><xsl:apply-templates select="*"/>
    </xsl:template>

    <xsl:template match="tb:Block/tb:OnStart[tb:Stop | tb:Pause]"><xsl:if test="@InsertSoundBlock">
        A!:I[<xsl:value-of select="@InsertSoundBlock"/>]</xsl:if>
    </xsl:template>
    
    <xsl:template match="tb:OnEnd">
        A$:<xsl:if test="@InsertSoundBlock">I[<xsl:value-of select="@InsertSoundBlock"/>]</xsl:if><xsl:apply-templates select="*"/>
    </xsl:template>
        
    <xsl:template match="tb:Block/tb:OnEnd[tb:Stop | tb:Pause]"><xsl:if test="@InsertSoundBlock">
        A$:I[<xsl:value-of select="@InsertSoundBlock"/>]</xsl:if>
    </xsl:template>
 
    <xsl:template match="tb:OnButtonClick | tb:OnButtonHold">
        A<xsl:call-template name="CodeForWord"><xsl:with-param name="WordToConvert" select="@Button1"/></xsl:call-template>
        <xsl:if test="local-name(current())='OnButtonHold'">_</xsl:if>
      <xsl:if test="@Button2"><xsl:call-template name="CodeForWord"><xsl:with-param name="WordToConvert" select="@Button2"/></xsl:call-template>
          <xsl:if test="local-name(current())='OnButtonHold'">_</xsl:if></xsl:if>:<xsl:if test="@InsertSoundBlock">I[<xsl:value-of select="@InsertSoundBlock"/>] </xsl:if>
        <xsl:if test="@SetSpeed">S<xsl:call-template name="CodeForWord"><xsl:with-param name="WordToConvert" select="@SetSpeed"/></xsl:call-template> </xsl:if>
        <xsl:apply-templates select="*"/>
    </xsl:template>
    
    <xsl:template match="tb:NoOp">~</xsl:template>
    <xsl:template match="tb:Stop">.</xsl:template>
    <xsl:template match="tb:Pause">,</xsl:template>
    <xsl:template match="tb:Goto">[<xsl:value-of select="@ContainerName"/>]</xsl:template>
    <xsl:template match="tb:LoadPackage">&lt;<xsl:value-of select="@Name"/>&gt;</xsl:template>
    <xsl:template match="tb:PlayPause">P</xsl:template>
    <xsl:template match="tb:CallBlock">([<xsl:value-of select="@BlockName"/>]) <xsl:value-of select="@ReturnRewindSeconds"/></xsl:template>
    <xsl:template match="tb:Return">)</xsl:template>
    <xsl:template match="tb:PreviousBlock">B<xsl:if test="@Class">{<xsl:value-of select="@Class"/>}</xsl:if></xsl:template>
    <xsl:template match="tb:NextBlock">F<xsl:if test="@Class">{<xsl:value-of select="@Class"/>}</xsl:if></xsl:template>
    <xsl:template match="tb:RelativeTimeJump">T<xsl:value-of select="@RelativeTime"/></xsl:template>
    <xsl:template match="tb:SetLight"><xsl:value-of select="substring(@Light,1,1)"/><xsl:if test="@Mode='Off'">-</xsl:if></xsl:template>
    <xsl:template match="tb:SetVolume">V<xsl:call-template name="CodeForWord"><xsl:with-param name="WordToConvert" select="@relative"/></xsl:call-template></xsl:template>
    <xsl:template match="tb:SetSpeed">S<xsl:call-template name="CodeForWord"><xsl:with-param name="WordToConvert" select="@relative"/></xsl:call-template></xsl:template>
    <xsl:template match="tb:SetUSBMode">U<xsl:if test="@Active=0"><xsl:value-of select="@Active"/></xsl:if></xsl:template>
    
    <xsl:template name="CodeForWord">
        <xsl:param name="WordToConvert"/>
        <xsl:choose>
            <xsl:when test="$WordToConvert='Home'">h</xsl:when>
            <xsl:when test="$WordToConvert='Left'">&lt;</xsl:when>
            <xsl:when test="$WordToConvert='Right'">&gt;</xsl:when>
            <xsl:when test="$WordToConvert='Up'">^</xsl:when>
            <xsl:when test="$WordToConvert='Down'">v</xsl:when>
            <xsl:when test="$WordToConvert='Plus'">+</xsl:when>
            <xsl:when test="$WordToConvert='Minus'">-</xsl:when>
            <xsl:when test="$WordToConvert='Star'">*</xsl:when>
            <xsl:when test="$WordToConvert='PlayPause'">p</xsl:when>
            <xsl:when test="$WordToConvert='Select'">o</xsl:when>
            <xsl:when test="$WordToConvert='up'">+</xsl:when>
            <xsl:when test="$WordToConvert='down'">-</xsl:when>
            <xsl:when test="$WordToConvert='normal'">N</xsl:when>
        </xsl:choose>
        
    </xsl:template>
</xsl:stylesheet>
