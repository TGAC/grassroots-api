#ifndef OBJTOOLS_IDMAPPER___IDMAPPER_IMPL__HPP
#define OBJTOOLS_IDMAPPER___IDMAPPER_IMPL__HPP

/*  $Id: idmapper.hpp 432036 2014-04-09 17:27:03Z grichenk $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Author:  Frank Ludwig
 *
 * File Description: Definition of the IIdMapper interface and its
 *          implementation
 *
 */

#include <corelib/ncbistd.hpp>
#include <objtools/readers/iidmapper.hpp>
#include <objmgr/seq_loc_mapper.hpp>
#include <objmgr/scope.hpp>
#include <objtools/readers/message_listener.hpp>
#include <objects/genomecoll/GC_Assembly.hpp>
#include <objects/genomecoll/GC_Sequence.hpp>

BEGIN_NCBI_SCOPE
BEGIN_objects_SCOPE // namespace ncbi::objects::

/// IdMapper base class implementation
///
/// Provides the means to set up and maintain an internal table of mappings
/// and to use such table for actual ID mapping.
/// Actual initialization of the internal table is left for derived classes to
/// implement.
///
class NCBI_XOBJREAD_EXPORT CIdMapper: public IIdMapper
{

public:
    /// Constructor specifying the mapping context, direction, and error
    /// handling.
    /// @param strContext
    ///   the mapping context or genome source IDs will belong to. Something
    ///   like "mm6" or "hg18".
    /// @param bInvert
    ///   Mapping direction. "true" will map in reverse direction.
    /// @param pErrors
    ///   Optional error container. If specified, mapping errors will be passed
    ///   to the error container for further processing. If not specified,
    ///   mapping errors result in exceptions that need to be handled.
    CIdMapper(const std::string& strContext = "",
              bool bInvert = false,
              IMessageListener* pErrors = 0 );

    virtual ~CIdMapper() {};

    /// Add a mapping to the internal mapping table.
    /// @param from
    ///   source handle, or target handle in the case of reverse mapping
    /// @param to
    ///   target handle, or source handle in the case of reverse mapping
    virtual void AddMapping(const CSeq_id_Handle& from,
                            const CSeq_id_Handle& to );

    virtual void AddMapping(const CSeq_loc& loc_from,
                            const CSeq_loc& loc_to);

    virtual CSeq_id_Handle Map(const CSeq_id_Handle&);

    virtual CRef<CSeq_loc> Map(const CSeq_loc& loc);

    /// Map all embedded IDs in a given object at once.
    virtual void MapObject(CSerialObject&);

    struct SMappingContext
    {
        string context;
        string map_from;
        string map_to;
    };

protected:
    static std::string
    MapErrorString(const CSeq_id_Handle& );

    static std::string
    MapErrorString(const CSeq_loc& );

    const std::string m_strContext;
    const bool m_bInvert;

    struct SMapper
    {
        CSeq_id_Handle dest_idh;
        CRef<CSeq_loc_Mapper> dest_mapper;
    };
    typedef std::map<CSeq_id_Handle, SMapper> TMapperCache;
    TMapperCache m_Cache;

    IMessageListener* m_pErrors;
};


/// IdMapper implementation combining multiple id mappers with the selected
/// priorities.
///
class NCBI_XOBJREAD_EXPORT CIdMapperComposite : public CIdMapper
{
public:

    /// Sub-mapper priority. Default is zero, positive values are for
    /// higher priorities. Mappers with the same priorities are checked
    /// in the order of their addition to the composite mapper.
    enum EPriority {
        kPriority_Default = 0
    };
    typedef int TPriority;

    CIdMapperComposite(void) {}

    /// Add sub-mapper. By default the composite mapper owns the added
    /// object.
    void AddMapper(IIdMapper* mapper,
                   TPriority  priority = kPriority_Default,
                   EOwnership ownership = eTakeOwnership);

    virtual CSeq_id_Handle Map(const CSeq_id_Handle&);

    virtual CRef<CSeq_loc> Map(const CSeq_loc& loc);

private:
    typedef int TOrder;
    struct SNode {
        typedef CIdMapperComposite::TPriority TPriority;
        typedef CIdMapperComposite::TOrder TOrder;

        AutoPtr<IIdMapper>    m_Mapper;
        TPriority             m_Priority;
        TOrder                m_Order;
        static CAtomicCounter_WithAutoInit sm_Counter;

        SNode(IIdMapper* mapper, TPriority priority, EOwnership ownership)
            : m_Mapper(mapper, ownership),
              m_Priority(priority)
        {
            m_Order = sm_Counter.Add(1);
        }

        bool operator<(const SNode& node) const
        {
            // Higher priority goes first
            if (m_Priority != node.m_Priority) return m_Priority > node.m_Priority;
            // Lower order goes first
            return m_Order < node.m_Order;
        }
    };

    typedef set<SNode> TMappers;

    TMappers m_Mappers;
};


/// IdMapper implementation using an external configuration file
///
/// The internal mapping table will be initialized during IdMapper construction
/// from a given input stream (typically, an open configuration file).
///
class NCBI_XOBJREAD_EXPORT CIdMapperConfig: public CIdMapper
{

public:
    /// Constructor specifying the content of the mapping table, mapping
    /// context, direction, and error handling.
    ///
    /// The configuration-file-based mapper uses a config file to indicate how
    /// mapping should be performed.  Configuration-based mapping is suitable
    /// for simple id -> id mapping, and cannot generally be used to indicate
    /// mapping through a complex location, as would be needed when handling
    /// things such as mapping to UCSC chrRandom.
    ///
    /// The format of the configuration file is as a standard Windows .ini
    /// file, and it should be structured as follows:
    ///
    /// \code
    ///     [hg18]
    ///     map_from = UCSC HG18
    ///     map_to = NCBI Human build 36
    ///     89161185 = chr1 1
    ///     89161199 = chr2 2
    ///     89161205 = chr3 3
    ///     89161207 = chr4 4
    ///     51511721 = chr5 5
    /// \endcode
    ///
    /// Note that the config file appears backwards!  This is intentional, and
    /// is structured so as to capture a many-to-one synonymy that we often see
    /// in IDs.  The snippet above implies:
    ///   - We are mapping from UCSC build HG18 -> NCBI Human build 36
    ///   - The chromosomes are defined by bare integers, which represent gis
    ///   - The primary aliases all begin 'chr', as 'chr1', 'chr2', etc.
    ///   - Each chromosome is represented by multiple input aliases (chr1, 1,
    ///     etc)
    ///   - We map implicitly from lcl|chr1 -> 89161185, lcl|1 -> 89161185
    ///   - Because of a limitation in processing .ini files, we cannot use a
    ///     full FASTA representation for the key (the gi).  We can use one for
    ///     the aliases.  Since bare integers are interpreted as gis, it is
    ///     necessary to qualify bare integers as local IDs if you wish to have
    ///     a representation as something other than a gi
    ///
    /// @param istr
    ///   open input stream containing tabbed data specifying map sources and
    ///   targets.
    /// @param strContext
    ///   the mapping context or genome source IDs will belong to. Something
    ///   like "mm6" or "hg18".
    /// @param bInvert
    ///   Mapping direction. "true" will map in reverse direction.
    /// @param pErrors
    ///   Optional error container. If specified, mapping errors will be passed
    ///   to the error container for further processing. If not specified,
    ///   mapping errors result in exceptions that need to be handled.
    CIdMapperConfig(CNcbiIstream& istr,
                    const std::string& strContext = "",
                    bool bInvert = false,
                    IMessageListener* pErrors = 0);

    CIdMapperConfig(const std::string& strContext = "",
                    bool bInvert = false,
                    IMessageListener* pErrors = 0);

    void Initialize(CNcbiIstream& istr);
    static void DescribeContexts(CNcbiIstream& istr,
                                 list<SMappingContext>& contexts);

protected:

    void AddMapEntry(const std::string& );

    void SetCurrentContext(const std::string&,
                           std::string& );

    CSeq_id_Handle SourceHandle(const std::string& );

    CSeq_id_Handle TargetHandle(const std::string& );
};


/// IdMapper implementation using hardcoded values
///
/// Mapping targets are fixed at compile time and cannot be modified later.
/// Useful for self contained applications that should work without external
/// configuration files or databases.
///
class NCBI_XOBJREAD_EXPORT CIdMapperBuiltin: public CIdMapperConfig
{

public:
    /// Constructor specifying the mapping context, direction, and error
    /// handling.
    /// @param strContext
    ///   the mapping context or genome source IDs will belong to. Something
    ///   like "mm6" or "hg18".
    /// @param bInvert
    ///   Mapping direction. "true" will map in reverse direction.
    /// @param pErrors
    ///   Optional error container. If specified, mapping errors will be passed
    ///   to the error container for further processing. If not specified,
    ///   mapping errors result in exceptions that need to be handled.
    CIdMapperBuiltin(const std::string& strContext,
                     bool bInvert = false,
                     IMessageListener* pErrors = 0 );

    void Initialize();

protected:
    void AddMapEntry(const std::string&, int);
};



/// IdMapper implementation using an external database
///
/// Mappings will be retrived from an external database, then cached internally
/// for future reuse.
///
class NCBI_XOBJREAD_EXPORT CIdMapperDatabase: public CIdMapper
{
public:
    /// Constructor specifying a database containing the actual mapping, the
    /// mapping context, direction, and error handling.
    /// @param strServer
    ///   server on which the mapping database resides.
    /// @param strDatabase
    ///   the actual database on the specified server.
    /// @param strContext
    ///   the mapping context or genome source IDs will belong to. Something
    ///   like "mm6" or "hg18".
    /// @param bInvert
    ///   Mapping direction. "true" will map in reverse direction.
    /// @param pErrors
    ///   Optional error container. If specified, mapping errors will be passed
    ///   to the error container for further processing. If not specified,
    ///   mapping errors result in exceptions that need to be handled.
    CIdMapperDatabase(
        const std::string& strServer,
        const std::string& strDatabase,
        const std::string& strContext,
        bool bInvert = false,
        IMessageListener* pErrors = 0)
        : CIdMapper(strContext, bInvert, pErrors),
          m_strServer(strServer),
          m_strDatabase(strDatabase)
    {};

    virtual CSeq_id_Handle Map(const CSeq_id_Handle& from);

protected:
    const std::string m_strServer;
    const std::string m_strDatabase;
};


/// IdMapper implementaion using CScope.
///
/// Non-versioned ids are mapped to versioned ones referencing the focus
/// sequence or its parts.
///
class NCBI_XOBJREAD_EXPORT CIdMapperScope : public CIdMapper
{
public:
    /// Initialize the mapper using the scope and the focus seq-id handle.
    CIdMapperScope(CScope& scope, const CSeq_id_Handle& focus_idh);

    /// Initialize the mapper using the scope and the focus seq-id.
    CIdMapperScope(CScope& scope, const CSeq_id& focus_id);

private:
    void x_Init(const CSeq_id_Handle& focus_idh);
    void x_AddMappings(const CBioseq_Handle& bh);

    CRef<CScope> m_Scope;
};


/// IdMapper implementation using a GC-Assembly
///
/// Ids are mapped to the selected alias type.
///
class NCBI_XOBJREAD_EXPORT CIdMapperGCAssembly : public CIdMapper
{
public:
    typedef CIdMapper TParent;

    /// Mapping destination type.
    enum EAliasMapping {
        eGenBank,    ///< GenBank GI
        eGenBankAcc, ///< GenBank accession
        eRefSeq,     ///< RefSeq GI
        eRefSeqAcc,  ///< RefSeq accession
        eUCSC,       ///< External UCSC id
        eOther,      ///< Private id
        eAccVer      ///< Find versioned accession
    };

    /// Establish mappings based on a GC-Assembly
    ///
    /// Inputs for this are a full assembly and a concept of what you are
    /// mapping to.  "map to" means "convert to another ID space", defined by
    /// the enum above.  To allow for future extension, eOther maps to the
    /// GC-Assembly concept of "private" or the named scope provided in the
    /// extra string parameter (i.e., "UCSC")
    CIdMapperGCAssembly(CScope& scope,
                        const CGC_Assembly& assm,
                        EAliasMapping       mapping,
                        const string&       alias_scope = kEmptyStr);

    CIdMapperGCAssembly(CScope& scope);

    void AddAliasMappings(const CGC_Assembly& assm,
                          EAliasMapping       mapping,
                          const string&       alias_scope = kEmptyStr);

    /// Map seq-id, throw CIdMapperException if mapping fails.
    virtual CSeq_id_Handle Map(const CSeq_id_Handle&);

    using TParent::Map;

private:
    void x_AddUnversionedMapping(const CSeq_id&        src_id,
                                 const CSeq_id_Handle& dst_id);
    void x_AddAccessionMapping(const CSeq_id& id);
    void x_AddAliasMappings(const CGC_Assembly& seq,
                            EAliasMapping mapping,
                            const string& alias_scope);
    void x_AddAliasMappings(const CGC_AssemblyUnit& seq,
                            EAliasMapping mapping,
                            const string& alias_scope);
    void x_AddAliasMappings(const CGC_Sequence& seq,
                            EAliasMapping mapping,
                            const string& alias_scope);

    CRef<CScope> m_Scope;
};


class NCBI_XOBJREAD_EXPORT CIdMapperException : public CException
{
public:
    enum EErrCode {
        eBadSeqId,
        eOther
    };
    virtual const char* GetErrCodeString(void) const;
    NCBI_EXCEPTION_DEFAULT(CIdMapperException, CException);
};


END_objects_SCOPE
END_NCBI_SCOPE

#endif // OBJTOOLS_IDMAPPER___IDMAPPER_IMPL__HPP
